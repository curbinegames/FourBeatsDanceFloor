
#include <DxLib.h>

#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

#include <main.h>
#include <Title.h>
#include <Select.h>
#include <Play.h>
#include <Result.h>

#define DX_MAIN_DEF HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow

int FBDF_font_DSEG7Modern = -1;

static void GameMain() {
	FBDF::play_choose_music_st next_music;
	view_num_t next = VIEW_TITLE;
	FBDF_result_data_t result;

	while (next != VIEW_EXIT) {
		switch (next) {
		case VIEW_TITLE:
			next = FirstTitleView();
			break;
		case VIEW_SELECT:
			next = FirstSelectView(&next_music);
			break;
		case VIEW_PLAY:
			next = FirstPlayView(&result, &next_music);
			break;
		case VIEW_RESULT:
			next = FirstResultView(&result);
			break;
		default:
			return;
		}
		if (GetWindowUserCloseFlag(TRUE)) { // 閉じるボタンが押された
			break;
		}
		WaitTimer(10); // ループウェイト
	}
}

/* WinMain関数はもう編集する必要なし */
int WINAPI WinMain(DX_MAIN_DEF) {
	/* dxlibの初期化 */
	ChangeWindowMode(TRUE); // ウィンドウモードにする
	SetGraphMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 32); // ウィンドウサイズの変更
	SetAlwaysRunFlag(TRUE); // 非アクティブでも動くようにする
	SetWindowUserCloseEnableFlag(FALSE); // ×ボタンで勝手に閉じないようにする
	SetMainWindowText(TOOL_NAME); // ウィンドウの名前
	SetWindowSizeChangeEnableFlag(TRUE); // ウィンドウサイズを変えれるようにする
	if (DxLib_Init() == -1) { return -1; } // エラーで中断
	SetDrawScreen(DX_SCREEN_BACK); // 作画モード変更

	// フォント読み込み
	FBDF_font_DSEG7Modern = LoadFontDataToHandle(_T("font/DSEG7Modern_S32_T4_I.dft"));

	GameMain(); // ゲーム処理
	DxLib_End(); // DxLib終わり
	return 0; // プログラム終わり
}
