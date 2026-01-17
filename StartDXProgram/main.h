#pragma once

#define WINDOW_SIZE_MODE 0 // 0~2 を切り替えながら使ってね

#define TOOL_NAME "Four Beat Dance Floor" // ツールの名前

#if WINDOW_SIZE_MODE == 0
#define WINDOW_SIZE_X 960 // ウィンドウの横のサイズ
#elif WINDOW_SIZE_MODE == 1
#define WINDOW_SIZE_X 640 // ウィンドウの横のサイズ
#elif WINDOW_SIZE_MODE == 2
#define WINDOW_SIZE_X 960 // ウィンドウの横のサイズ
#endif

#if WINDOW_SIZE_MODE == 0
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 3 / 4) // ウィンドウの縦のサイズ
#elif WINDOW_SIZE_MODE == 1
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 3 / 4) // ウィンドウの縦のサイズ
#elif WINDOW_SIZE_MODE == 2
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 9 / 20) // ウィンドウの縦のサイズ
#endif

typedef   signed int sint;
typedef unsigned int uint;

typedef enum view_num_e {
	VIEW_EXIT = -1,
	VIEW_TITLE,
	VIEW_SELECT,
	VIEW_PLAY,
	VIEW_RESULT,
} view_num_t;

typedef struct command_s {
	int x;
	int y;
} command_t;

/* main.cpp の最初のみ初期化され、他の場所では参照のみ許可 */
extern int FBDF_font_DSEG7Modern;
