#pragma once

#include <tchar.h>

#define FBDF_SCORE_RANK_P_BORDER 100.0
#define FBDF_SCORE_RANK_XP_BORDER 99.5
#define FBDF_SCORE_RANK_X_BORDER  99.0
#define FBDF_SCORE_RANK_SP_BORDER 98.0
#define FBDF_SCORE_RANK_S_BORDER  97.0
#define FBDF_SCORE_RANK_AP_BORDER 95.0
#define FBDF_SCORE_RANK_A_BORDER  90.0
#define FBDF_SCORE_RANK_B_BORDER  80.0
#define FBDF_SCORE_RANK_C_BORDER  70.0
#define FBDF_SCORE_RANK_D_BORDER  60.0

typedef enum FBDF_clear_type_e {
    FBDF_CLEAR_TYPE_NOPLAY,
    FBDF_CLEAR_TYPE_FAILED,
    FBDF_CLEAR_TYPE_CLEARED,
    FBDF_CLEAR_TYPE_FULLCOMBO,
    FBDF_CLEAR_TYPE_PERFECT
} FBDF_clear_type_et;

typedef struct FBDF_file_music_score_s {
	int score = 0;
	double acc = 0.0;
	FBDF_clear_type_et clear_type = FBDF_CLEAR_TYPE_NOPLAY;
} FBDF_file_music_score_st;

enum class FBDF_dif_type_ec {
    NONE,
    LIGHT,
    NORMAL,
    HYPER
};

FBDF_dif_type_ec &operator++(FBDF_dif_type_ec &val);
FBDF_dif_type_ec &operator--(FBDF_dif_type_ec &val);

extern bool FBDF_Save_ReadScoreAllDif(        FBDF_file_music_score_st  dest[], const TCHAR *music_folder_name);
extern bool FBDF_Save_WriteScoreAllDif( const FBDF_file_music_score_st  src[],  const TCHAR *music_folder_name);
extern bool FBDF_Save_ReadScoreOneDif(        FBDF_file_music_score_st *dest,   const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);
extern bool FBDF_Save_WriteScoreOneDif( const FBDF_file_music_score_st *src,    const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);
extern bool FBDF_Save_UpdateScoreOneDif(const FBDF_file_music_score_st *src,    const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);
