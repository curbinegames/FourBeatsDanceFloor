#pragma once

/**
 * コーディング規約
 * 自分が忘れるので書いておく
 * 
 * ・文字数
 * 1行100文字まで
 * 
 * ・includeの順番
 * 標準関数
 * STL系
 * DxLib
 * curbinecodes
 * FBDF_main関連
 * FBDF_個別関連
 * 自分ヘッダ
 * 
 * ・変数指定の順番
 * bool
 * char
 * int
 * size_t
 * double
 * enum
 * struct
 * STL系(std::vectorとか)
 * FILE
 * class
 * DxPic_t関連
 * DxSnd_t関連
 * DxTime_t関連
 * cutin関連
 * 
 * ・引数指定の順番
 * 変数指定+可変ポインタを先頭に
 * Ntime関連は末尾に
 * 
 * ・構造体、関数名
 * 先頭にFBDF_をつける。
 * 
 * ・関数の引数の数
 * 5個以内が望ましい。5個を超えるなら構造体にすることを検討する。
 */

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
