#pragma once

#include <main.h>
#include <Play.h>
#include <CalDif.h>

typedef struct FBDF_music_dif_s {
	double notes  = 0;
	double color  = 0;
	double trick  = 0;
	double length = 0;
	double all    = 0;
} FBDF_music_dif_t;

typedef struct FBDF_music_detail_s {
	std::string folder_name;
	std::string map_file_name;
	std::string music_name;
	std::string music_file_name;
	std::string artist;
	std::string jucket_name;
	uint Length = 0;
	uint pre_time = 10000;
	uint sample_rate = 44100;
	FBDF_music_dif_t auto_cal_dif;
	int user_dif = 0;
	int level_list[3] = { -1,-1,-1 };
	FBDF_dif_type_ec dif_type = FBDF_dif_type_ec::LIGHT;
	FBDF_music_most_colorpat_t most_colorpat[MOST_COLORPAT_NUM];
	FBDF_music_colorcount_t color_count;
	double note_density = 0;
	FBDF_file_music_score_st user_highscore;
} FBDF_music_detail_t;

extern view_num_t FBDF_SelectView(FBDF_play_choose_music_st &nex_music);
