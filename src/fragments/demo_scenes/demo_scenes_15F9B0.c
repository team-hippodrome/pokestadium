#include "demo_scenes.h"
#include "game_state.h"

/**
 * Runs the demo scene selected by gSessionContext.mode and returns its exit code.
 * Mode 18 is the boot diorama shown after the N64 logo, 17 the cup-clear ceremony, 21 and 26 the
 * Pokémon present after clearing Gym Leader Castle or the Hall of Fame, 22/23 the first-clear bonus
 * scene, and 24 the Mewtwo intro.
 */
s32 Stage_DispatchDemoScene(UNUSED s32 arg0, UNUSED s32 arg1) {
    switch (gSessionContext.mode) {
        case 18:
            return Stage_ShowBootDiorama();

        case 17:
            return Stage_ShowClearCeremony();

        case 21:
            return Stage_ShowCompletionScene();

        case 26:
            return Stage_ShowCompletionScene();

        case 22:
        case 23:
            return Stage_ShowFirstClearBonusScene();

        case 24:
            return Stage_ShowMewtwoIntro();
    }
}
