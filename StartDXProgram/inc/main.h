#pragma once

/**
 * ・自分コーディングルール <own coding rules>
 * 自分が忘れるので書いておく
 * 
 * ・文字数
 * 1行100文字まで。改行するときは外括弧優先。
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
 * ・class内の順番
 * 変数定義
 * コンスト関連
 * その他系
 * init系
 * update系
 * draw系
 * set系
 * get系
 * is系
 * 
 * ・構造体、関数名
 * 先頭にFBDF_をつける。
 * 関数の単語の頭は大文字、  スペースは入れない。                         ※例、FBDF_Play_GetNowScore
 * 列挙型の単語の頭は小文字、アンダーバーでスペースを入れる。接尾辞は_et。※例、FBDF_result_rank_mat_et
 * 構造体の単語の頭は小文字、アンダーバーでスペースを入れる。接尾辞は_st。※例、FBDF_select_list_st
 * クラスの単語の頭は小文字、アンダーバーでスペースを入れる。接尾辞は_c 。※例、FBDF_play_judge_c
 * 
 * ・関数の引数の数
 * 5個以内が望ましい。5個を超えるなら構造体にすることを検討する。
 * 
 * ・#if 1 <コメント> について
 * エディタで見た時、#ifで挟まれている箇所は表示を閉じることができる。
 * これをしたいだけ。以上。
 * 
 * ・namespaceについて
 * エディタで文字列検索するとき、名前の重複が気になって嫌なので使わない。好みじゃ。大体のものに"FBDF_"ってついてるから許して。
 * 
 * ・用語集
 * 絶対時間: GetNowCount()で取ってきた値そのもの。内部的には13879507とか言うデカい数値になってる。
 * 相対時間: ある基準点から経過した時間。[GetNowCount() - <基準となる絶対時間>]となっていることが多い。
 */

#define WINDOW_SIZE_MODE 0 // 0~4 を切り替えながら使ってね

#define TOOL_NAME "Four Beat Dance Floor" // ツールの名前

#if WINDOW_SIZE_MODE == 0 /* いつもの */
#define WINDOW_SIZE_X 960 // ウィンドウの横のサイズ
#elif WINDOW_SIZE_MODE == 1 /* ちいさめ */
#define WINDOW_SIZE_X 640 // ウィンドウの横のサイズ
#elif WINDOW_SIZE_MODE == 2 /* 横長 */
#define WINDOW_SIZE_X 960 // ウィンドウの横のサイズ
#elif WINDOW_SIZE_MODE == 3 /* でかい */
#define WINDOW_SIZE_X 1920 // ウィンドウの横のサイズ
#elif WINDOW_SIZE_MODE == 4 /* チビ */
#define WINDOW_SIZE_X 320 // ウィンドウの横のサイズ
#endif

#if WINDOW_SIZE_MODE == 0
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 3 / 4) // ウィンドウの縦のサイズ
#elif WINDOW_SIZE_MODE == 1
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 3 / 4) // ウィンドウの縦のサイズ
#elif WINDOW_SIZE_MODE == 2
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 9 / 20) // ウィンドウの縦のサイズ
#elif WINDOW_SIZE_MODE == 3
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 9 / 16) // ウィンドウの縦のサイズ
#elif WINDOW_SIZE_MODE == 4
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 3 / 4) // ウィンドウの縦のサイズ
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
