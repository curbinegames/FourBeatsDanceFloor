#pragma once

#include <string>
#include <vector>

#include <dxcur.h>

#include <system.h>

typedef struct FBDF_Play_motion_s {
#if FBDF_DANCER_MAT_TYPE == 0 /* ‰æ‘œ */
	std::string image_path;
	dxcur_divpic_c pic;
#endif /* ‰æ‘œ */
	bool len_1 = false;
	bool len_2 = false;
	bool len_3 = false;
	bool len_4 = false;
	bool len_8 = false;
	bool type_u = false; /* •ûŒüŠÖ˜A‚Í2ŒÂˆÈã“¯‚Étrue‚É‚È‚ç‚È‚¢‚©‚à */
	bool type_d = false;
	bool type_l = false;
	bool type_r = false;
	bool type_f = false;
	bool type_b = false;
	bool type_j = false;
	bool type_c = false;
	bool type_1 = false;
	bool type_2 = false;
	bool type_3 = false;
	bool type_4 = false;
	bool extra = false;
	std::vector<size_t> next;
} FBDF_Play_motion_st;

extern int FBDF_DancerMotionEnc(std::vector<FBDF_Play_motion_st> &motion, const char *folder_name);
