#pragma once

#include <mapenc.h>

#define MOST_COLORPAT_NUM 10

typedef enum FBDF_color_pat_mat_e {
	COLOR_PAT_NONE = 0,
	COLOR_PAT_111,
	COLOR_PAT_112,
	COLOR_PAT_113,
	COLOR_PAT_114,
	COLOR_PAT_121,
	COLOR_PAT_122,
	COLOR_PAT_123,
	COLOR_PAT_124,
	COLOR_PAT_131,
	COLOR_PAT_132,
	COLOR_PAT_133,
	COLOR_PAT_134,
	COLOR_PAT_141,
	COLOR_PAT_142,
	COLOR_PAT_143,
	COLOR_PAT_144,
	COLOR_PAT_211,
	COLOR_PAT_212,
	COLOR_PAT_213,
	COLOR_PAT_214,
	COLOR_PAT_221,
	COLOR_PAT_222,
	COLOR_PAT_223,
	COLOR_PAT_224,
	COLOR_PAT_231,
	COLOR_PAT_232,
	COLOR_PAT_233,
	COLOR_PAT_234,
	COLOR_PAT_241,
	COLOR_PAT_242,
	COLOR_PAT_243,
	COLOR_PAT_244,
	COLOR_PAT_311,
	COLOR_PAT_312,
	COLOR_PAT_313,
	COLOR_PAT_314,
	COLOR_PAT_321,
	COLOR_PAT_322,
	COLOR_PAT_323,
	COLOR_PAT_324,
	COLOR_PAT_331,
	COLOR_PAT_332,
	COLOR_PAT_333,
	COLOR_PAT_334,
	COLOR_PAT_341,
	COLOR_PAT_342,
	COLOR_PAT_343,
	COLOR_PAT_344,
	COLOR_PAT_411,
	COLOR_PAT_412,
	COLOR_PAT_413,
	COLOR_PAT_414,
	COLOR_PAT_421,
	COLOR_PAT_422,
	COLOR_PAT_423,
	COLOR_PAT_424,
	COLOR_PAT_431,
	COLOR_PAT_432,
	COLOR_PAT_433,
	COLOR_PAT_434,
	COLOR_PAT_441,
	COLOR_PAT_442,
	COLOR_PAT_443,
	COLOR_PAT_444
} FBDF_color_pat_mat_t;

typedef struct FBDF_music_most_colorpat_s {
	FBDF_color_pat_mat_t mat = COLOR_PAT_NONE;
	uint count = 0;
} FBDF_music_most_colorpat_t;

typedef struct FBDF_music_colorcount_s {
	double c1 = 0;
	double c2 = 0;
	double c3 = 0;
	double c4 = 0;
} FBDF_music_colorcount_t;

extern double FBDF_CalMapNotesDif(const cvec<FBDF_note_t> &notes);
extern double FBDF_CalMapColorDif(const cvec<FBDF_note_t> &notes);
extern double FBDF_CalMapTrickDif(const cvec<FBDF_note_t> &notes);
extern void FBDF_CalMapMostColorPat(FBDF_music_most_colorpat_t *mostpat, const FBDF_map_t *map);
extern void FBDF_CountMapColor(FBDF_music_colorcount_t *count, const cvec<FBDF_note_t> &notes, uint Length);
