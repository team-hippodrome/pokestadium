#ifndef _FRAGMENT29_H_
#define _FRAGMENT29_H_

#include "global.h"
#include "src/fragments/lab_widget_tree/lab_widget_tree.h"

typedef void (*func889000C4)(unk_func_889000C4*, s32, WidgetAnimatedPanel*, s32, char**, MemoryPool*);
typedef void (*func889006D4)(unk_func_889000C4*, s32);
typedef void (*func889007AC)(unk_func_889000C4*);
typedef s32 (*func88900808)(unk_func_889000C4*, Controller*, s32);
typedef void (*func88900970)(unk_func_889000C4*, s32);

void LabUI_BuildConfirmDialog(unk_func_889000C4*, s32, WidgetAnimatedPanel*, WidgetNode*, char**, MemoryPool*);
void LabUI_OpenConfirmDialog(unk_func_889000C4*, s32);
void LabUI_CloseConfirmDialog(unk_func_889000C4*);
s32 LabUI_RunConfirmDialog(unk_func_889000C4*, Controller*, s32);
void LabUI_ShowSavingPrompt(unk_func_889000C4*, s32);

#endif // _FRAGMENT29_H_
