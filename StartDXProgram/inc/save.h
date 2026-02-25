#pragma once

#include <tchar.h>
#include <system.h>

typedef struct FBDF_file_music_score_s {
	int score = 0;
	double acc = 0.0;
	FBDF_clear_type_et clear_type = FBDF_CLEAR_TYPE_NOPLAY;
} FBDF_file_music_score_st;

extern bool FBDF_Save_ReadScoreAllDif(        FBDF_file_music_score_st  dest[], const TCHAR *music_folder_name);
extern bool FBDF_Save_WriteScoreAllDif( const FBDF_file_music_score_st  src[],  const TCHAR *music_folder_name);
extern bool FBDF_Save_ReadScoreOneDif(        FBDF_file_music_score_st &dest,   const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);
extern bool FBDF_Save_WriteScoreOneDif( const FBDF_file_music_score_st &src,    const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);
extern bool FBDF_Save_UpdateScoreOneDif(const FBDF_file_music_score_st &src,    const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);

extern bool FBDF_Save_ReadOption(       FBDF_game_option_st *dest);
extern bool FBDF_Save_WriteOption(const FBDF_game_option_st *src);

extern bool FBDF_ErrorLogWrite(const char *message);
