#include "credits.h"

/**
 * Returns the number of staff-roll lines (187; 188 in Spanish, 186 in Italian): the size of sRoleLines and the number of
 * strings Credits_InitRoleList reads from textdata table 45.
 */
u16 Credits_GetRoleLineCount(void) {
#if defined(VERSION_ES)
    return 188;
#elif defined(VERSION_IT)
    return 186;
#else
    return 0xBB;
#endif
}
