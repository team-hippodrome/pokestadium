import tempfile
import unittest
from pathlib import Path

from affected_versions import affected


def tree(**yamls):
    root = Path(tempfile.mkdtemp())
    for v, cs in yamls.items():
        d = root / "yamls" / v
        d.mkdir(parents=True)
        (d / "rom.yaml").write_text("".join(f"    - [0x{i:X}, c, {n}]\n" for i, n in enumerate(cs)))
    return root


class Affected(unittest.TestCase):
    def setUp(self):
        self.root = tree(us=["a", "b"], jp=["a"])

    def test_version_paths_gate_only_that_version(self):
        self.assertEqual(affected(self.root, ["yamls/jp/rom.yaml", "linker_scripts/jp/x.ld"]), ["jp"])

    def test_shared_source_without_line_info_needs_only_us(self):
        self.assertEqual(affected(self.root, ["src/a.c"]), ["us"])
        self.assertEqual(affected(self.root, ["src/b.c"]), ["us"])

    def test_source_only_another_version_builds_needs_that_version(self):
        root = tree(us=["b"], jp=["a"])
        self.assertEqual(affected(root, ["src/a.c"]), ["jp"])

    def test_new_source_defaults_to_us(self):
        self.assertEqual(affected(self.root, ["src/new.c"]), ["us"])

    def test_docs_gate_nothing(self):
        self.assertEqual(affected(self.root, ["COVERAGE.md", "docs/regions/x.md"]), [])

    def test_editor_and_tool_test_files_gate_nothing(self):
        files = [".vscode/settings.json", ".github/workflows/clangd.yml", ".clangd", ".gitignore", "compile_flags.txt",
                 "tools/clangd_config.py", "tools/test_clangd_config.py", "CONTRIBUTING.md"]
        self.assertEqual(affected(self.root, files), [])

    def test_global_tools_and_workflows_need_only_us(self):
        self.assertEqual(affected(self.root, ["tools/port_version.py"]), ["us"])
        self.assertEqual(affected(self.root, [".github/workflows/pr-rom-gate.yml"]), ["us"])
        self.assertEqual(affected(self.root, [".vscode/settings.json", "tools/port_version.py"]), ["us"])

    def test_global_build_system_needs_only_us(self):
        self.assertEqual(affected(self.root, ["Makefile"]), ["us"])
        self.assertEqual(affected(self.root, ["linker_scripts/common_undef_syms.ld"]), ["us"])

    def test_unstarted_region_path_gates_nothing_buildable(self):
        (self.root / "ci").mkdir()
        (self.root / "ci/regions.json").write_text('{"fr": {}}')
        self.assertEqual(affected(self.root, ["baseroms/fr/checksum.md5"]), [])


SOURCE = """\
#include "common.h"
void shared(void) {
    a();
#if defined(VERSION_JP)
    jp_only();
#elif defined(VERSION_US_REV2)
    rev2_only();
#else
    us_and_pal();
#endif
#if !defined(VERSION_JP)
    not_jp();
#endif
#if defined(VERSION_US_REV1) || defined(VERSION_US_REV2)
    revs();
#endif
#if BUILD_VERSION >= VERSION_J
    unknown();
#endif
#ifdef VERSION_ES
    spanish();
#endif
}
"""


REV2_WRAPPED = """\
#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/f.s")
#else
void f(void) {
    a();
}
#endif
"""

REV2_MATCHED = """\
void f(void) {
#if defined(VERSION_US_REV2)
    rev2();
#else
    a();
#endif
}
"""


class ByCompiledCode(unittest.TestCase):
    """SOURCE lines: 3 a(), 4 #if JP, 5 jp_only(), 7 rev2_only(), 9 us_and_pal(),
    12 not_jp(), 15 revs(), 18 unknown(), 21 spanish()."""

    def setUp(self):
        versions = ["us", "us_rev1", "us_rev2", "jp", "es", "eu"]
        self.root = tree(**{v: ["shared", "f"] for v in versions})

    def edit(self, **lines):
        new = SOURCE.splitlines()
        for n, text in lines.items():
            new[int(n[1:]) - 1] = text
        return affected(self.root, ["src/shared.c"], {"src/shared.c": (SOURCE, "\n".join(new) + "\n")})

    def test_unconditional_line_needs_only_us(self):
        self.assertEqual(self.edit(L3="    b();"), ["us"])

    def test_jp_block_needs_only_jp(self):
        self.assertEqual(self.edit(L5="    jp_changed();"), ["jp"])

    def test_elif_branch_needs_only_its_version(self):
        self.assertEqual(self.edit(L7="    rev2_changed();"), ["us_rev2"])

    def test_else_branch_us_compiles_needs_only_us(self):
        self.assertEqual(self.edit(L9="    changed();"), ["us"])

    def test_negated_block_us_compiles_needs_only_us(self):
        self.assertEqual(self.edit(L12="    changed();"), ["us"])

    def test_block_for_two_revisions_needs_both(self):
        self.assertEqual(self.edit(L15="    changed();"), ["us_rev1", "us_rev2"])

    def test_unknown_condition_counts_as_compiled_everywhere(self):
        self.assertEqual(self.edit(L18="    changed();"), ["us"])

    def test_per_version_macro_selects_that_version(self):
        self.assertEqual(self.edit(L21="    changed();"), ["es"])

    def test_edits_in_different_blocks_combine(self):
        self.assertEqual(self.edit(L3="    b();", L5="    x();", L7="    y();"), ["jp", "us", "us_rev2"])

    def test_widened_condition_needs_only_the_version_it_adds(self):
        self.assertEqual(self.edit(L4="#if defined(VERSION_JP) || defined(VERSION_ES)"), ["es"])

    def test_matching_a_rev2_function_needs_only_rev2(self):
        texts = {"src/f.c": (REV2_WRAPPED, REV2_MATCHED)}
        self.assertEqual(affected(self.root, ["src/f.c"], texts), ["us_rev2"])

    def test_headers_are_compared_too(self):
        new = SOURCE.replace("jp_only();", "jp_changed();")
        self.assertEqual(affected(self.root, ["include/x.h"], {"include/x.h": (SOURCE, new)}), ["jp"])

    def test_new_file_nothing_builds_needs_us(self):
        self.assertEqual(affected(self.root, ["src/brand_new.c"], {"src/brand_new.c": ("", SOURCE)}), ["us"])


if __name__ == "__main__":
    unittest.main()
