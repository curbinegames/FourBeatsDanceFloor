
#include <tchar.h>

#include <string>
#include <vector>

#include <sancur.h>

typedef enum FBDF_Play_note_button_e {
	FBDF_PLAY_NOTE_BTN_NONE,
	FBDF_PLAY_NOTE_BTN_1,
	FBDF_PLAY_NOTE_BTN_2,
	FBDF_PLAY_NOTE_BTN_3,
	FBDF_PLAY_NOTE_BTN_4
} FBDF_Play_note_btn_et;

typedef enum FBDF_mapenc_error_e {
	FDF_MAPENC_ERROR_NONE = 0,
	FDF_MAPENC_ERROR_FILE = 1, /* ファイルを読み込めなかった */
	FDF_MAPENC_ERROR_NOTE_FULL, /* ノーツ数が多すぎる */
	FDF_MAPENC_ERROR_OPTION, /* オプションに変な数が入ってた */
	FDF_MAPENC_ERROR_INVALID_NOTE_CHAR, /* ノーツに変な文字が混ざってた */
} FBDF_mapenc_error_et;

typedef struct FBDF_note_s {
	uint time = 0; /* 押すタイミング 0: none, [ms] */
	uint pos = 0; /* %4 */
	FBDF_Play_note_btn_et btn = FBDF_PLAY_NOTE_BTN_NONE;
	uint len = 99;
	uint mtime = 0; /* ダンスの長さ[ms] */
	uint bpm = 120;
} FBDF_note_t;

typedef struct FBDF_map_s {
	std::vector<FBDF_note_t>note;
	std::string artist;
	uint   noteNo  = 0; /* 今見ているノート番号 */
	uint   noteN   = 0; /* ノートの数。note.sizeで取れそうな気が…… */
	uint   blockNo = 0;
	double bpm     = 120;
	int    offset  = 2000; /* 譜面スタート時間。相対時間 */
	int    song_offset = 2000; // nps 専用
	uint   Stime   = 0; /* 曲開始時間。絶対時間 */
	uint   Ntime   = 0; /* 今の時間。相対時間 */
	uint   Etime   = 0; /* 譜面が終わる時間。相対時間 */
	TCHAR  music_file[256]=_T("music.mp3"); // rrs osu tja nps 専用
} FBDF_map_t;

extern int MapLoadOne(FBDF_map_t *map, const char *nex_music);
