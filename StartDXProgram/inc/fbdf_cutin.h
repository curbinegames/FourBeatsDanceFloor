#pragma once

#include <dxcur.h>

/* TODO: enum化 */
#define CUT_FRAG_OUT 0
#define CUT_FRAG_IN  1

typedef enum cutin_tips_e {
	CUTIN_TIPS_NONE = 0,
	CUTIN_TIPS_ON,
	CUTIN_TIPS_SONG,
} cutin_tips_e;

class FBDF_cutin_c {
private:
	dxcur_pic_c leftpic  = dxcur_pic_c(_T("pic/cutinLeft.png"));
	dxcur_pic_c rightpic = dxcur_pic_c(_T("pic/cutinRight.png"));
	dxcur_snd_c sound    = dxcur_snd_c(_T("SE/cutin.wav"));

	int sequence = 0; /* 0で全開、1000で封鎖 */

	int cutIoFg = CUT_FRAG_OUT;
	int cutStime = 0;

	int x_window_size = 640;
	int y_window_size = 480;

	bool acs_sound = false;

public:

	/* カットイン描画 */
	void DrawCut() const;

	void SetIo(int val);
	void SetWindowSize(int x, int y);

	void update();
	int IsClosing() const;
	bool IsEndAnim() const;
};
