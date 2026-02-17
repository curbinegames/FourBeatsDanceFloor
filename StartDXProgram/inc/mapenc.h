#pragma once

#include <tchar.h>

#include <string>
#include <vector>

#include <sancur.h>
#include <dxcur.h>
#include <datacur.h>

typedef enum FBDF_Play_note_button_e {
	FBDF_PLAY_NOTE_BTN_NONE,
	FBDF_PLAY_NOTE_BTN_1,
	FBDF_PLAY_NOTE_BTN_2,
	FBDF_PLAY_NOTE_BTN_3,
	FBDF_PLAY_NOTE_BTN_4
} FBDF_Play_note_btn_et;

typedef enum FBDF_mapenc_error_e {
	FBDF_MAPENC_ERROR_NONE = 0,
	FBDF_MAPENC_ERROR_FILE = 1, /* ファイルを読み込めなかった */
	FBDF_MAPENC_ERROR_NOTE_FULL, /* ノーツ数が多すぎる */
	FBDF_MAPENC_ERROR_OPTION, /* オプションに変な数が入ってた */
	FBDF_MAPENC_ERROR_INVALID_NOTE_CHAR, /* ノーツに変な文字が混ざってた */
} FBDF_mapenc_error_et;

typedef enum FBDF_note_motion_assign_e {
	FBDF_NOTE_MOTION_ASSIGN_NONE,  /* ポーズ無指定 */
	FBDF_NOTE_MOTION_ASSIGN_UP,    /* 上向きポーズ */
	FBDF_NOTE_MOTION_ASSIGN_DOWN,  /* 下向きポーズ */
	FBDF_NOTE_MOTION_ASSIGN_LEFT,  /* 左向きポーズ(観客から見て) */
	FBDF_NOTE_MOTION_ASSIGN_RIGHT, /* 右向きポーズ(観客から見て) */
	FBDF_NOTE_MOTION_ASSIGN_FRONT, /* 前向きポーズ(観客向き) */
	FBDF_NOTE_MOTION_ASSIGN_BACK,  /* 後ろ向きポーズ(観客に背を向ける) */
	FBDF_NOTE_MOTION_ASSIGN_JUMP,  /* ジャンプ */
	FBDF_NOTE_MOTION_ASSIGN_CLAP,  /* 手を叩く */
	FBDF_NOTE_MOTION_ASSIGN_TURN,  /* 周る */
	FBDF_NOTE_MOTION_ASSIGN_1,     /* 指で1を作る */
	FBDF_NOTE_MOTION_ASSIGN_2,     /* 指で2を作る */
	FBDF_NOTE_MOTION_ASSIGN_3,     /* 指で3を作る */
	FBDF_NOTE_MOTION_ASSIGN_4,     /* 指で4を作る */
	FBDF_NOTE_MOTION_ASSIGN_VPOSE  /* ピースサインを作る。2とは区別される、一応ね */
} FBDF_note_motion_assign_et;

typedef struct FBDF_note_s {
	DxTime_t time = 0; /* 押すタイミング 0: none, [ms] */
	uint pos = 0; /* %4 */
	FBDF_Play_note_btn_et btn = FBDF_PLAY_NOTE_BTN_NONE;
	uint len = 99;
	uint mtime = 0; /* ダンスの長さ[ms] */
	uint bpm = 120;
	FBDF_note_motion_assign_et motion = FBDF_NOTE_MOTION_ASSIGN_NONE;
} FBDF_note_t;

typedef struct FBDF_map_s {
	datacur_cursor_vector<FBDF_note_t>note;
	std::string artist;
	uint     blockNo = 0;
	double   bpm     = 120;
	int      offset  = 2000; /* 譜面スタート時間。相対時間 */
	int      song_offset = 2000; // nps 専用
	uint     Stime   = 0; /* 曲開始時間。絶対時間 */
	DxTime_t Ntime   = 0; /* 今の時間。相対時間 */
	uint     Etime   = 0; /* 譜面が終わる時間。相対時間 */
	TCHAR    music_file[256]=_T("music.mp3"); // rrs osu tja nps 専用
} FBDF_map_t;

extern FBDF_mapenc_error_et MapLoadOne(FBDF_map_t *map, const char *nex_music);
