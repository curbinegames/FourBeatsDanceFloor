#pragma once

#include <dxcur.h>

typedef struct FBDF_option_pic_s {
    dxcur_pic_c back{ _T("pic/black.png") };
    dxcur_pic_c box{ _T("pic/select/option_box.png") };
    dxcur_pic_c detail{ _T("pic/select/option_detail.png") };
} FBDF_option_pic_st;

extern void FBDF_Option_ReloadPic(void);
extern void FBDF_Option_KeyAction(int &cmd, bool &option_fg);
extern void FBDF_Option_Draw(int cmd, const FBDF_option_pic_st &pic, const FBDF_usage_c &usage);
