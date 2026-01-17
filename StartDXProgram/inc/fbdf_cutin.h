#pragma once

#include <dxcur.h>

/* TODO: enumâª */
#define CUT_FRAG_OUT 0
#define CUT_FRAG_IN  1

typedef enum cutin_tips_e {
	CUTIN_TIPS_NONE = 0,
	CUTIN_TIPS_ON,
	CUTIN_TIPS_SONG,
} cutin_tips_e;

class fbdf_cutin_c {
private:
	dxcur_pic_c leftpic;
	dxcur_pic_c rightpic;

	int sound = 0;

	int sequence = 0; /* 0Ç≈ëSäJÅA1000Ç≈ïïçΩ */

	int cutIoFg = CUT_FRAG_OUT;
	int cutStime = 0;

	int x_window_size = 640;
	int y_window_size = 480;

	bool acs_sound = false;

public:
	fbdf_cutin_c();
	~fbdf_cutin_c();
	void DrawCut() const;

	void SetIo(int val);
	void SetWindowSize(int x, int y);

	void update();
	int IsClosing() const;
	bool IsEndAnim() const;
};
