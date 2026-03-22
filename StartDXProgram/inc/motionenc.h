#pragma once

#include <string>
#include <vector>

#include <dxcur.h>

#include <system.h>

typedef struct FBDF_Play_motion_len_s {
	bool n1 = false;
	bool n2 = false;
	bool n3 = false;
	bool n4 = false;
	bool n8 = false;
} FBDF_Play_motion_len_st;

/* TODO: goodモーション欲しい */
typedef struct FBDF_Play_motion_type_s {
	bool up = false; /* 方向関連は2個以上同時にtrueにならないかも */
	bool down = false;
	bool left = false;
	bool right = false;
	bool front = false;
	bool back = false;
	bool jump = false;
	bool clap = false;
	bool turn = false;
	bool n1 = false;
	bool n2 = false;
	bool n3 = false;
	bool n4 = false;
	bool vpose = false;
} FBDF_Play_motion_type_st;

typedef struct FBDF_Play_motion_s {
#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
	std::string image_path;
	dxcur_divpic_c pic;
#endif /* 画像 */
	FBDF_Play_motion_len_st len;
	FBDF_Play_motion_type_st type;
	bool extra = false; /* ポーズ指定の時のみ呼ばれるかどうか */
	std::vector<size_t> next;
} FBDF_Play_motion_st;

extern int FBDF_DancerMotionEnc(std::vector<FBDF_Play_motion_st> &motion, const char *folder_name);
