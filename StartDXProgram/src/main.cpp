
#include <DxLib.h>

#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

#include <main.h>
#include <save.h>
#include <Title.h>
#include <Select.h>
#include <Play.h>
#include <Result.h>

#define DX_MAIN_DEF HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow

int FBDF_font_DSEG7Modern = -1;

static void GameMain(void) {
	FBDF_play_choose_music_st next_music;
	view_num_t next = VIEW_TITLE;
	FBDF_result_data_t result;

	while (next != VIEW_EXIT) {
		switch (next) {
		case VIEW_TITLE:
			next = FBDF_TitleView();
			break;
		case VIEW_SELECT:
			next = FBDF_SelectView(next_music);
			break;
		case VIEW_PLAY:
			next = FBDF_PlayView(result, next_music);
			break;
		case VIEW_RESULT:
			next = FirstResultView(result);
			break;
		default:
			return;
		}
		if (GetWindowUserCloseFlag()) { // 閉じるボタンが押された
			FBDF_LOG_INFO("ウィンドウのxボタンで終了しました。");
			break;
		}
		WaitTimer(10); // ループウェイト
	}
}

typedef struct raw_s {
	int score = 0;
	double acc = 0.0;
	FBDF_clear_type_et clear_type = FBDF_CLEAR_TYPE_NOPLAY;
} raw_st;

static bool raw_read(raw_st dest[], const TCHAR *music_folder_name) {
	std::string save_path;
	FILE *fp;

	save_path  = "save/";
	save_path += music_folder_name;
	save_path += ".dat";

	fopen_s(&fp, save_path.c_str(), "rb");
	if (fp == NULL) { return false; }
	fread(dest, sizeof(raw_st), 3, fp);
	fclose(fp);

	return true;
}

static void update_save(const TCHAR *music_folder_name) {
	raw_st dest[3];
	FBDF_file_music_score_st new_data[3];
	if (raw_read(dest, music_folder_name) == false) {
		return;
	}
	for (size_t i = 0; i < 3; i++) {
		new_data[i].acc = dest[i].acc;
		new_data[i].blanc_clear_type = FBDF_CLEAR_TYPE_NOPLAY;
		new_data[i].clear_type = dest[i].clear_type;
		new_data[i].score = dest[i].score;
	}
	FBDF_Save_WriteScoreAllDif(new_data, music_folder_name);
}

static void update_all_save(void) {
	update_save(_T("Ghost_Tango"));
	update_save(_T("Sky_Modulation"));
	update_save(_T("Your_Story"));
	update_save(_T("アンゴラたちの踊り"));
	update_save(_T("ハレクラニ"));
	update_save(_T("横顔"));
	update_save(_T("流桜"));
}

/* WinMain関数はもう編集する必要なし */
int WINAPI WinMain(DX_MAIN_DEF) {
	// update_all_save(); /* セーブデータ更新用 */
	/* dxlibの初期化 */
	// SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	ChangeWindowMode(TRUE); // ウィンドウモードにする
	SetGraphMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 32); // ウィンドウサイズの変更
	SetAlwaysRunFlag(TRUE); // 非アクティブでも動くようにする
	SetWindowUserCloseEnableFlag(FALSE); // ×ボタンで勝手に閉じないようにする
	SetMainWindowText(TOOL_NAME); // ウィンドウの名前
	SetWindowSizeChangeEnableFlag(TRUE); // ウィンドウサイズを変えれるようにする
	if (DxLib_Init() == -1) { /* エラーで中断 */
		FBDF_LOG_EMERG("DXライブラリの初期化に失敗しました。");
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK); // 作画モード変更
	
	/* 3Dライト設定 */ {
		/* (左/右 ,上/下 , 手前/奥) */
		/* ゲーム中に変更/削除する予定がないのでハンドルは破棄 */
		ChangeLightTypeDir(  VGet( 0, -1,  1)); // 下から
		CreateDirLightHandle(VGet( 1,  1,  1)); // 左上から
		CreateDirLightHandle(VGet(-1,  1,  1)); // 右上から
		CreateDirLightHandle(VGet( 0,  0, -1)); // 奥から
	}
	/* 3Dカメラ設定 */
	SetCameraScreenCenter(lins(0, 0, 2000, WINDOW_SIZE_X, 1230), lins(0, 0, 2000, WINDOW_SIZE_Y, 600));
	SetupCamera_Perspective(0.6);

	// フォント読み込み
	FBDF_font_DSEG7Modern = LoadFontDataToHandle(_T("font/DSEG7Modern_S32_T4_I.dft"));

	FBDF_Save_ReadOption(&game_option);

	GameMain(); // ゲーム処理
	DxLib_End(); // DxLib終わり
	return 0; // プログラム終わり
}
