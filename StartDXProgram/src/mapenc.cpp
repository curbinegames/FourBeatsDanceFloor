
#include <vector>
#include <string>

#include <DxLib.h>

#include <sancur.h>
#include <strcur.h>
#include <datacur.h>
#include <UTF8_conv.h>

#include <save.h>
#include <system.h>

#include <mapenc.h>

#define ISNOTE(c) ( \
	(c) == '-' ||   \
	(c) == '.' ||   \
	(c) == 'u' ||   \
	(c) == 'd' ||   \
	(c) == 'l' ||   \
	(c) == 'r' ||   \
	(c) == 'f' ||   \
	(c) == 'b' ||   \
	(c) == 'j' ||   \
	(c) == 'c' ||   \
	(c) == '1' ||   \
	(c) == '2' ||   \
	(c) == '3' ||   \
	(c) == '4'      \
)

typedef struct FBDF_map_enc_s {
	double now_bpm      = 120;
	uint   now_block    = 4;
	uint   now_shutpos  = 0;
	double now_shuttime = 0;
	uint   measure      = 4; // tja nps 専用
	uint   measure_u    = 4; // tja nps 専用
	double scrool       = 1; // nps 専用
} FBDF_map_enc_t;

/**
 * @brief ノーツ情報を読み込む。ブロック版。
 * @param[out] map 格納先
 * @param[in] buf 読み込む文字列。ブロック単位
 * @param[out] option 譜面読み込みのオプション
 * @return FBDF_mapenc_error_et エラー情報
 */
static FBDF_mapenc_error_et GetNoteBlock(FBDF_map_t &map, char const *buf, FBDF_map_enc_t &option) {
	if (map.note.isfull()) { /* ノーツ数が2000に達していたらこれ以上読み込まない */
		FBDF_ErrorLogWrite("ノーツ数が多すぎます!");
		return FBDF_MAPENC_ERROR_NOTE_FULL;
	}
	if (option.now_block == 0) { /* ブロック数0とか意味わからんもの定義してないからダメ */
		FBDF_ErrorLogWrite("ブロック数に0を指定してノーツを読み込もうとしました。");
		return FBDF_MAPENC_ERROR_OPTION;
	}
	for (size_t ic = 0; ic < option.now_block; ic++) { /* ノーツに関係しない文字が一個でもあったらダメ */
		if (!ISNOTE(buf[ic])) {
			FBDF_ErrorLogWrite("ノーツに関係ない文字が混ざっています。");
			return FBDF_MAPENC_ERROR_INVALID_NOTE_CHAR;
		}
	}

	for (size_t ic = 0; ic < option.now_block; ic++) {
		FBDF_note_t buf_note;
		if (buf[ic] != '.') {
			switch (buf[ic]) {
			case 'u':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_UP;
				break;
			case 'd':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_DOWN;
				break;
			case 'l':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_LEFT;
				break;
			case 'r':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_RIGHT;
				break;
			case 'f':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_FRONT;
				break;
			case 'b':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_BACK;
				break;
			case 'j':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_JUMP;
				break;
			case 'c':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_CLAP;
				break;
			case '1':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_1;
				break;
			case '2':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_2;
				break;
			case '3':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_3;
				break;
			case '4':
				buf_note.motion = FBDF_NOTE_MOTION_ASSIGN_4;
				break;
			}
			buf_note.pos = option.now_shutpos + ic;
			switch (option.now_block) {
			case 2:
				buf_note.btn = (ic == 1) ? FBDF_PLAY_NOTE_BTN_3 : FBDF_PLAY_NOTE_BTN_1;
				break;
			case 3:
			case 4:
				buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic + 1);
				break;
			case 5:
				switch (ic) {
				case 4:
					buf_note.btn = FBDF_PLAY_NOTE_BTN_3;
					break;
				default:
					buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic + 1);
					break;
				}
				break;
			case 6:
				switch (ic) {
				case 4:
					buf_note.btn = FBDF_PLAY_NOTE_BTN_3;
					break;
				case 5:
					buf_note.btn = FBDF_PLAY_NOTE_BTN_2;
					break;
				default:
					buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic + 1);
					break;
				}
				break;
			case 7:
				if (ic <= 3) {
					buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic + 1);
				}
				else {
					buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic - 2);
				}
				break;
			case 9:
				if (ic <= 2) {
					buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic + 1);
				}
				else if (ic <= 5) {
					buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic - 2);
				}
				else {
					buf_note.btn = static_cast<FBDF_Play_note_btn_et>(ic - 5);
				}
				break;
			default:
				buf_note.btn = static_cast<FBDF_Play_note_btn_et>((ic) % 4 + 1);
				break;
			}
			buf_note.len = 99;
			if (!map.note.empty()) {
				FBDF_note_t before_note = map.note.lastData();
				map.note.pop_back();
				before_note.len = buf_note.pos - before_note.pos;
				map.note.push_back(before_note);
			}
			buf_note.time = option.now_shuttime + 60000 * 4 * ic / (option.now_bpm * option.scrool * option.measure_u * option.now_block) + game_option.note_offset_timing;
			buf_note.mtime = 750;
			if (!map.note.empty()) {
				FBDF_note_t before_note = map.note.lastData();
				map.note.pop_back();
				before_note.mtime = buf_note.time - before_note.time;
				map.note.push_back(before_note);
			}
			buf_note.bpm = option.now_bpm;
			map.note.push_back(buf_note);
			map.Etime = buf_note.time;
			map.note.stepNo();
			if (map.note.isfull()) {
				FBDF_ErrorLogWrite("ノーツ数が多すぎます!");
				return FBDF_MAPENC_ERROR_NOTE_FULL;
			}
		}
	}
	option.now_shutpos += option.now_block;
	option.now_shuttime += 60000 * 4 / (double)(option.now_bpm * option.scrool * option.measure_u);
	map.blockNo++;
	return FBDF_MAPENC_ERROR_NONE;
}

/**
 * @brief ノーツ情報を読み込む。文字列版。
 * @param[out] map 格納先
 * @param[in] buf 読み込む文字列
 * @param[out] option 譜面読み込みのオプション
 * @return FBDF_mapenc_error_et エラー情報
 */
static FBDF_mapenc_error_et GetNoteLine(FBDF_map_t &map, const char *buf, FBDF_map_enc_t &option) {
	FBDF_mapenc_error_et err = FBDF_MAPENC_ERROR_NONE;
	std::string strbuf = buf;

	while (0 < strbuf.size()) {
		err = GetNoteBlock(map, strbuf.c_str(), option);
		if (err != FBDF_MAPENC_ERROR_NONE) {
			return err;
		}
		strbuf.erase(0, option.now_block);
	}

	return FBDF_MAPENC_ERROR_NONE;
}

/**
 * @brief 譜面を読み込む
 * @param[out] map 格納先
 * @param[in] nex_music 譜面ファイルのパス
 * @return FBDF_mapenc_error_et エラー情報
 */
FBDF_mapenc_error_et FBDF_MapLoadOne(FBDF_map_t &map, const char *nex_music) {
	char buf[256];
	char musicPath[96];
	FBDF_mapenc_error_et err = FBDF_MAPENC_ERROR_NONE;

	FBDF_map_enc_t option;

	FILE *fp = NULL;

	strcpy_s(musicPath, sizeof(musicPath), nex_music);

	fopen_s(&fp, musicPath, "r");
	if (fp == NULL) {
		std::string buf = "couldn't open file: ";
		buf += musicPath;
		FBDF_ErrorLogWrite(buf.c_str());
		return FBDF_MAPENC_ERROR_FILE;
	}

	while (fgets(buf, 256, fp) != NULL) {
		if (strands(buf, "BPM:")) {
			strmods(buf, 4);
			map.bpm = strsansD(buf, 256);
			option.now_bpm = map.bpm;
		}
		else if (strands(buf, "OFFSET:")) {
			strmods(buf, 7);
			map.offset = strtol(buf, NULL, 10);
			option.now_shuttime = map.offset;
		}
		else if (strands(buf, "ARTIST:")) {
			strmods(buf, 7);
			map.artist = buf;
			/* 改行消し */
			for (int ic = 0; ic < map.artist.size(); ic++) {
				if (map.artist[ic] == '\n') {
					map.artist.pop_back();
				}
			}
			/* 日本語補正 */
			map.artist = UTF8_converter(map.artist);
		}
		else {
			break;
		}
	}

	while (fgets(buf, 256, fp) != NULL) {
		if (buf[0] == _T(';')) {
			; // nothing
		}
		else if (strands(buf, "BLOCK:")) {
			strmods(buf, 6);
			option.now_block = strtol(buf, NULL, 10);
		}
		else if (strands(buf, "BPM:")) {
			strmods(buf, 4);
			option.now_bpm = strtod(buf, NULL);
		}
		else {
			for (size_t i = 0; buf[i] != '\0'; i++) {
				if (buf[i] == '\n') {
					buf[i] = '\0';
					break;
				}
			}
			FBDF_mapenc_error_et err_buf = GetNoteLine(map, buf, option);
			if (err_buf != FBDF_MAPENC_ERROR_NONE) { err = err_buf; } /* エラーあっても最後まで読み切る */
		}
	}
	fclose(fp);

	/* ラストノート挿入 */ {
		FBDF_note_t buf_note;
		map.note.push_back(buf_note);
	}

	return err;
}
