#pragma once

#include <main.h>
#include <Result.h>

#define FBDF_PLAY_SCOREBAR_COLOR_RED60     0xFFEB3324 /* 70  - 60 */
#define FBDF_PLAY_SCOREBAR_COLOR_RED0      0xFF962117 /* 60  -  0 */
#define FBDF_PLAY_SCOREBAR_COLOR_YELLOW70  0xFFBDBB3F /* 70  - 80 */
#define FBDF_PLAY_SCOREBAR_COLOR_YELLOW80  0xFFEDEB4F /* 80  - 90 */
#define FBDF_PLAY_SCOREBAR_COLOR_GREEN90   0xFF5BC23C /* 90  - 95 */
#define FBDF_PLAY_SCOREBAR_COLOR_GREEN95   0xFF78FF4F /* 95  - 97 */
#define FBDF_PLAY_SCOREBAR_COLOR_BLUE97    0xFF3282F6 /* 97  - 98 */
#define FBDF_PLAY_SCOREBAR_COLOR_BLUE98    0xFF73FBFD /* 98  - 99 */
#define FBDF_PLAY_SCOREBAR_COLOR_PURPLE99  0xFFA349A4 /* 99  -100 */
#define FBDF_PLAY_SCOREBAR_COLOR_PURPLE100 0xFFEA3FF7 /* 99.5-100 */

namespace FBDF {
	typedef struct play_choose_music_s {
		std::string folder_name;
		std::string music_name;
		std::string map_file_name;
		FBDF_dif_type_ec dif_type = FBDF_dif_type_ec::NONE;
	} play_choose_music_st;
}

extern view_num_t FirstPlayView(FBDF_result_data_t *result_data, const FBDF::play_choose_music_st *next);
