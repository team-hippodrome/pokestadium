# Human Involvement in Annotation

This is a disclaimer of what sections of the repo had the most human involvement. It is *not* meant to be a function-by-function audit of what functions are "AI" vs "Human." 

This disclaimer only covers annotation. We do not keep a trail of what functions were matched by a human vs matched by an AI because the distinction is not helpful given the availability of a deterministic oracle for matching. Annotation has no such oracle, which is why we record provence for it in this document.

## Substantial Human Involvement
These sections of the code involve a substantial degree of human verification for their annotation, or are planned to require such.
- `area_select`
- `battle_engine`
- `battle_order_selection`
- `cup_select`
- `event_battle`
- `free_battle_rules`
- `gym_leader_castle`
- `options`
- `rental_hub`
- `rental_rules`

## Planned for substantial human Involvement
These unannotated fragments are planned to require substantial human involvement.
- All code outside of a fragment
- Fragment 61 (still unnamed, likely pokemon team registration)
- `rental_team_selection`
- `stadium_select`
- `title_screen`
- `victory_palace` (hall of fame)


## Human Review and Cross-checking
These Fragments had a human in the loop verifying their status.
- `battle_prep` (Intro and results screen)
- `credits`
- `demo_scenes` (cutscenes)
- `transfer_pak_error`
- `gym_leader_castle_clear`
- `n64_logo`
- `pikachu_reward`
- `particle_data_library`

## AI-Generated
These fragments had annotation almost entirely done by AI agents with minimal human involvement. Their annotations may contain hallucinations but should be of higher quality than unassigned fragments.
- `gb_tower_emulator`
- `particle_math`

## Unassigned
All fragments not explicitly listed above have not been been taken up by a team member and thus have no expected policy at this time. Assume any annotation contained within unlisted fragments as unreliable guesses by our first-pass annotation with AI.
