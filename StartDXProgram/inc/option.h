#pragma once

#include <dxcur.h>

typedef struct FBDF_option_pic_s {
    dxcur_pic_c back = dxcur_pic_c(_T("pic/black.png"));
    dxcur_pic_c box  = dxcur_pic_c(_T("pic/select/option_box.png"));
} FBDF_option_pic_st;

extern void FBDF_Option_ReloadPic(void);
extern void FBDF_Option_KeyAction(int &cmd, bool &option_fg);
extern void FBDF_Option_Draw(int cmd, const FBDF_option_pic_st &pic);
