#pragma once

#include <main.h>
#include <Result.h>

typedef struct play_choose_music_s {
	std::string folder_name;
	std::string music_name;
	std::string map_file_name;
	FBDF_dif_type_ec dif_type = FBDF_dif_type_ec::NONE;
} FBDF_play_choose_music_st;

extern view_num_t FBDF_PlayView(FBDF_result_data_t &result_data, const FBDF_play_choose_music_st &next);
