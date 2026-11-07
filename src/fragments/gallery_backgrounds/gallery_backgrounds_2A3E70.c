#include "gallery_backgrounds.h"
#include "src/text_system.h"

static char** gGalleryUiStrings;
static char** sTeamEntryText;

void Gallery_InitStringTables(void) {
    gGalleryUiStrings = Text_GetStringTable(0x2C);
    sTeamEntryText = Text_GetStringTable(0x1A);
}

char* Gallery_GetUiString(s32 arg0) {
    return Text_GetString(NULL, 0, gGalleryUiStrings, arg0);
}

char* Gallery_CopyUiString(char* arg0, s32 arg1, s32 arg2) {
    return Text_GetString(arg0, arg1, gGalleryUiStrings, arg2);
}

char* Gallery_GetCommonUiString(s32 arg0) {
    Text_GetString(NULL, 0, sTeamEntryText, arg0);
}

char* Gallery_CopyCommonUiString(char* arg0, s32 arg1, u32 arg2) {
    Text_GetString(arg0, arg1, sTeamEntryText, arg2);
}
