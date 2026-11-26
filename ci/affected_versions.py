#!/usr/bin/env python3
"""Print the ROM versions a change needs the gate to build.

usage: ci/affected_versions.py --base REV --head REV [--repo-root DIR]
       ci/affected_versions.py --all [--repo-root DIR]
       git diff --name-only BASE HEAD | ci/affected_versions.py [--repo-root DIR]

A version is buildable when yamls/<version>/rom.yaml exists. Output is one
version per line; nothing is printed when no build is affected (docs only).

  - files under yamls/, linker_scripts/, baseroms/, asm/, assets/ in a version
    directory affect only that version
  - C sources and headers (src/, include/) are compared per version: the
    lines each version actually compiles (its #if/#ifdef/#elif/#else
    branches, evaluated with that version's -D defines from the Makefile,
    without the directives themselves) before and after the change.
    A version whose compiled lines did not change needs no build. If US's
    changed, US also covers every version whose change is identical (a
    global edit); a version with a different change, or a change while US
    has none (say inside #if defined(VERSION_JP), or a US_REV2 match that
    drops its GLOBAL_ASM wrapper), is built itself. A src/<name>.c only
    counts for the versions whose rom.yaml builds <name> from C. Without
    the file contents (file names on stdin) a code file counts as a US-only
    change.
  - documentation, COVERAGE.md and editor-only files (.vscode/, .clangd, its
    workflow, .gitignore, compile_flags.txt, the clangd helper and tool unit
    tests) affect nothing: neither the Makefile nor any build tool reads them
  - everything else (Makefile, tools, lib, ci, shared linker scripts) is a
    global change and needs only US

--all prints every buildable version (the gate's manual "all" input).
"""
import argparse
import difflib
import json
import re
import subprocess
import sys
from pathlib import Path

VERSIONED = {"yamls", "linker_scripts", "baseroms", "asm", "assets"}
DOCS = re.compile(
    r"(^docs/|\.md$|^COVERAGE\.md$|^LICENSE"
    r"|^\.vscode/|^\.github/workflows/clangd\.yml$|^\.clangd$|^\.gitignore$|^compile_flags\.txt$"
    r"|^tools/clangd_config\.py$|^tools/test_[^/]*\.py$)"
)
CODE = re.compile(r"^(src|include)/.*\.(c|h|inc)$")
CENTRY = re.compile(r"^\s*-\s*\[0x[0-9A-Fa-f]+,\s*c,\s*([^\],\s]+)", re.MULTILINE)

# The VERSION_* macros the Makefile's BUILD_DEFINES passes for each version.
# Keep in sync with the Makefile. VERSION_<NAME> (e.g. VERSION_ES) is added
# for every version too, so a future per-version macro is understood even
# before it is listed here; that only ever selects more versions, not fewer.
PAL = ("eu", "eu_rev1", "fr", "de", "es", "it")
DEFINES = {
    "us": {"VERSION_US"},
    "us_rev1": {"VERSION_US", "VERSION_US_REV1"},
    "us_rev2": {"VERSION_US", "VERSION_US_REV2"},
    "jp": {"VERSION_JP"},
    "eu": {"VERSION_PAL", "VERSION_EU"},
    "eu_rev1": {"VERSION_PAL", "VERSION_EU", "VERSION_EU_REV1"},
    **{v: {"VERSION_PAL", f"VERSION_{v.upper()}"} for v in ("fr", "de", "es", "it")},
}


def defines(version):
    return DEFINES.get(version, set()) | {f"VERSION_{version.upper()}"}


def buildable(root: Path):
    return sorted(p.parent.name for p in (root / "yamls").glob("*/rom.yaml"))


def known_regions(root: Path):
    path = root / "ci/regions.json"
    regions = json.loads(path.read_text(encoding="utf-8")) if path.exists() else {}
    return {k for k in regions if not k.startswith("_")} | set(buildable(root))


# --- preprocessor conditions -------------------------------------------------
# Three-valued: True, False, or None when a condition depends on something
# other than VERSION_* macros (include guards, __STDC_VERSION__, ...). None
# counts as "may be compiled", which only ever selects more versions.

DIRECTIVE = re.compile(r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)$")
TOKEN = re.compile(r"\s*(defined\s*\(\s*\w+\s*\)|defined\s+\w+|&&|\|\||!|\(|\)|[01]\b)")


def _not(a):
    return None if a is None else not a


def _and(a, b):
    if a is False or b is False:
        return False
    return True if a and b else None


def _or(a, b):
    if a or b:
        return True
    return False if a is False and b is False else None


def evaluate(expr, macros):
    """Value of an #if expression under `macros`, or None when unknown."""
    expr = re.sub(r"/\*.*?\*/|//.*$", " ", expr).strip()
    tokens, pos = [], 0
    while pos < len(expr):
        m = TOKEN.match(expr, pos)
        if not m:
            return None  # comparisons, arithmetic, other macros: unknown
        tokens.append(m.group(1))
        pos = m.end()
        while pos < len(expr) and expr[pos].isspace():
            pos += 1
    if not tokens:
        return None

    def atom(i):
        t = tokens[i]
        if t == "!":
            v, i = atom(i + 1)
            return _not(v), i
        if t == "(":
            v, i = disjunction(i + 1)
            if i >= len(tokens) or tokens[i] != ")":
                raise ValueError
            return v, i + 1
        if t in ("0", "1"):
            return t == "1", i + 1
        if t.startswith("defined"):
            name = re.sub(r"defined|[()\s]", "", t)
            return (name in macros) if name.startswith("VERSION_") else None, i + 1
        raise ValueError

    def conjunction(i):
        v, i = atom(i)
        while i < len(tokens) and tokens[i] == "&&":
            w, i = atom(i + 1)
            v = _and(v, w)
        return v, i

    def disjunction(i):
        v, i = conjunction(i)
        while i < len(tokens) and tokens[i] == "||":
            w, i = conjunction(i + 1)
            v = _or(v, w)
        return v, i

    try:
        value, end = disjunction(0)
    except (ValueError, IndexError):
        return None
    return value if end == len(tokens) else None


def logical_lines(text):
    """(first physical line number, joined text) per logical line."""
    out, buf, start = [], [], None
    for n, line in enumerate(text.splitlines(), 1):
        if start is None:
            start = n
        if line.endswith("\\"):
            buf.append(line[:-1])
            continue
        buf.append(line)
        out.append((start, n, " ".join(buf)))
        buf, start = [], None
    if buf:
        out.append((start, start + len(buf) - 1, " ".join(buf)))
    return out


def compiled_lines(text, macros):
    """The lines a version compiles: (may compile it, is a conditional directive) per line."""
    result = {}
    stack = []  # frames: [parent_may, taken_for_sure, all_previous_false, branch_may]
    may = True
    for first, last, line in logical_lines(text):
        m = DIRECTIVE.match(line)
        kind = m.group(1) if m else None
        if kind in ("if", "ifdef", "ifndef"):
            arg = m.group(2).strip()
            if kind == "ifdef":
                arg = f"defined({arg.split()[0]})" if arg else ""
            elif kind == "ifndef":
                arg = f"!defined({arg.split()[0]})" if arg else ""
            c = evaluate(arg, macros)
            stack.append([may, c is True, c is False, c is not False])
            directive_may, may = may, may and c is not False
        elif kind in ("elif", "else") and stack:
            frame = stack[-1]
            c = True if kind == "else" else evaluate(m.group(2), macros)
            frame[3] = (not frame[1]) and c is not False
            frame[1] = frame[1] or (frame[2] and c is True)
            frame[2] = frame[2] and c is False
            directive_may, may = frame[0], frame[0] and frame[3]
        elif kind == "endif" and stack:
            may = stack.pop()[0]
            directive_may = may
        else:
            for n in range(first, last + 1):
                result[n] = (may, False)
            continue
        for n in range(first, last + 1):
            result[n] = (directive_may, True)
    return result


def compiled_text(text, version):
    """The non-directive lines `version` compiles, in order."""
    lines = text.splitlines()
    info = compiled_lines(text, defines(version))
    return [line for n, line in enumerate(lines, 1) if info.get(n, (True, False)) == (True, False)]


def change(old, new):
    """The +/- lines between two line lists, without positions."""
    return tuple(line for line in difflib.unified_diff(old, new, n=0, lineterm="")
                 if line[:1] in "+-" and not line.startswith(("+++", "---")))


def versions_for_change(old_text, new_text, candidates):
    """Versions a code change needs, given the file before and after."""
    changes = {v: change(compiled_text(old_text, v), compiled_text(new_text, v)) for v in candidates}
    changed = {v for v in candidates if changes[v]}
    if "us" not in changed:
        return changed
    return {"us"} | {v for v in changed if changes[v] != changes["us"]}


def affected(root: Path, files, texts=None):
    """Versions to build. `texts` maps a code file to its (old, new) contents, "" when
    absent on that side; code files without an entry count as US-only changes."""
    built = buildable(root)
    known = known_regions(root)
    texts = texts or {}
    us = {"us"} & set(built)
    c_files = {v: set(CENTRY.findall((root / f"yamls/{v}/rom.yaml").read_text(encoding="utf-8"))) for v in built}
    out = set()
    for f in files:
        f = f.strip()
        if not f or DOCS.search(f):
            continue
        parts = f.split("/")
        if parts[0] in VERSIONED and len(parts) > 2 and parts[1] in known:
            if parts[1] in built:
                out.add(parts[1])
        elif CODE.match(f):
            candidates = set(built)
            if f.startswith("src/") and f.endswith(".c"):
                name = re.sub(r"^src/|\.c$", "", f)
                candidates = {v for v in built if name in c_files[v]}
                if not candidates:  # a new file nothing builds yet
                    out |= us
                    continue
            if f in texts:
                out |= versions_for_change(*texts[f], candidates)
            else:
                out |= us if us & candidates else candidates
        else:
            out |= us
    return sorted(out)


# --- git ------------------------------------------------------------------------

def git(root, *args, check=True):
    return subprocess.run(["git", "-C", str(root), *args], check=check,
                          capture_output=True, text=True).stdout


def contents(root, base, head, path):
    return (git(root, "show", f"{base}:{path}", check=False),
            git(root, "show", f"{head}:{path}", check=False))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--repo-root", type=Path, default=Path.cwd())
    ap.add_argument("--base")
    ap.add_argument("--head")
    ap.add_argument("--all", action="store_true")
    args = ap.parse_args()
    root = args.repo_root.resolve()
    if args.all:
        result = buildable(root)
    elif args.base and args.head:
        files = git(root, "diff", "--name-only", args.base, args.head).splitlines()
        texts = {f: contents(root, args.base, args.head, f) for f in files if CODE.match(f)}
        result = affected(root, files, texts)
    else:
        result = affected(root, sys.stdin.read().splitlines())
    for v in result:
        print(v)


if __name__ == "__main__":
    main()
