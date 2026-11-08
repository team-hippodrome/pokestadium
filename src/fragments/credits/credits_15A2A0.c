#include "credits.h"

/**
 * Returns the number of staff-roll lines (187): the size of sRoleLines and the number of
 * strings Credits_InitRoleList reads from textdata table 45.
 */
u16 Credits_GetRoleLineCount(void) {
    return 0xBB;
}
