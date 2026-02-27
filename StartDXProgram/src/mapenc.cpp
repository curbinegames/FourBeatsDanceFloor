
#include <vector>
#include <string>

#include <DxLib.h>

#include <sancur.h>
#include <stdcur.h>
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

#define ISLYRICS(c) ( \
	(c) == '.' ||   \
	(c) == 'f' ||   \
	(c) == 'a' ||   \
	(c) == 'i' ||   \
	(c) == 'u' ||   \
	(c) == 'e' ||   \
	(c) == 'o' ||   \
	(c) == 's' ||   \
	(c) == 't' ||   \
	(c) == 'm' ||   \
	(c) == 'w' ||   \
	(c) == 'b' ||   \
	(c) == 'p' ||   \
	(c) == 'n'      \
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

#if 1 /* ノーツデータ系 */

static FBDF_note_motion_assign_et GetMotionAssign(char c) {
	FBDF_note_motion_assign_et ret = FBDF_NOTE_MOTION_ASSIGN_NONE;
	switch (c) {
	case 'u':
		ret = FBDF_NOTE_MOTION_ASSIGN_UP;
		break;
	case 'd':
		ret = FBDF_NOTE_MOTION_ASSIGN_DOWN;
		break;
	case 'l':
		ret = FBDF_NOTE_MOTION_ASSIGN_LEFT;
		break;
	case 'r':
		ret = FBDF_NOTE_MOTION_ASSIGN_RIGHT;
		break;
	case 'f':
		ret = FBDF_NOTE_MOTION_ASSIGN_FRONT;
		break;
	case 'b':
		ret = FBDF_NOTE_MOTION_ASSIGN_BACK;
		break;
	case 'j':
		ret = FBDF_NOTE_MOTION_ASSIGN_JUMP;
		break;
	case 'c':
		ret = FBDF_NOTE_MOTION_ASSIGN_CLAP;
		break;
	case 'v':
		ret = FBDF_NOTE_MOTION_ASSIGN_VPOSE;
		break;
	case 't':
		ret = FBDF_NOTE_MOTION_ASSIGN_TURN;
		break;
	case '1':
		ret = FBDF_NOTE_MOTION_ASSIGN_1;
		break;
	case '2':
		ret = FBDF_NOTE_MOTION_ASSIGN_2;
		break;
	case '3':
		ret = FBDF_NOTE_MOTION_ASSIGN_3;
		break;
	case '4':
		ret = FBDF_NOTE_MOTION_ASSIGN_4;
		break;
	}
	return ret;
}

/**
 * 再帰関数。0 <= block <= 6 で終了。
 * block は再帰されるたびに半分または1/3になる。
 * block 26 -> 13,13 -> 7,6,7,6 -> 4,3,6,4,3,6
 * block 45 -> 15,15,15 -> 5,5,5,5,5,5,5,5,5
 * block 49 -> 24,25 -> 8,8,8,12,13 -> 4,4,4,4,4,4,4,4,4,7,6 -> 4,4,4,4,4,4,4,4,4,4,3,6
 */
static FBDF_Play_note_btn_et GetNoteButton(uint block, uint ic) {
	FBDF_Play_note_btn_et ret = FBDF_PLAY_NOTE_BTN_1;
	switch (block) {
	case 0: /* ???? */
	case 1: /* 1 */
		ret = FBDF_PLAY_NOTE_BTN_1;
		break;
	case 2: /* 13 */
		switch (ic) {
		case 0:
			ret = FBDF_PLAY_NOTE_BTN_1;
			break;
		case 1:
			ret = FBDF_PLAY_NOTE_BTN_3;
			break;
		}
		break;
	case 3: /* 123 */
	case 4: /* 1234 */
		switch (ic) {
		case 0:
			ret = FBDF_PLAY_NOTE_BTN_1;
			break;
		case 1:
			ret = FBDF_PLAY_NOTE_BTN_2;
			break;
		case 2:
			ret = FBDF_PLAY_NOTE_BTN_3;
			break;
		case 3:
			ret = FBDF_PLAY_NOTE_BTN_4;
			break;
		}
		break;
	case 5: /* 12343 */
		switch (ic) {
		case 0:
			ret = FBDF_PLAY_NOTE_BTN_1;
			break;
		case 1:
			ret = FBDF_PLAY_NOTE_BTN_2;
			break;
		case 2:
			ret = FBDF_PLAY_NOTE_BTN_3;
			break;
		case 3:
			ret = FBDF_PLAY_NOTE_BTN_4;
			break;
		case 4:
			ret = FBDF_PLAY_NOTE_BTN_3;
			break;
		}
		break;
	case 6: /* 123432 */
		switch (ic) {
		case 0:
			ret = FBDF_PLAY_NOTE_BTN_1;
			break;
		case 1:
			ret = FBDF_PLAY_NOTE_BTN_2;
			break;
		case 2:
			ret = FBDF_PLAY_NOTE_BTN_3;
			break;
		case 3:
			ret = FBDF_PLAY_NOTE_BTN_4;
			break;
		case 4:
			ret = FBDF_PLAY_NOTE_BTN_3;
			break;
		case 5:
			ret = FBDF_PLAY_NOTE_BTN_2;
			break;
		}
		break;
	default:
		if ((block % 3) == 0) {
			/* 3等分して再計算 */
			uint next_block = block / 3;
			while (next_block <= ic) { ic -= next_block; }
			ret = GetNoteButton(next_block, ic);
		}
		else if ((block % 2) == 0) {
			/* 半分に分けて再計算 */
			uint next_block = block / 2;
			if (next_block <= ic) { ic -= next_block; }
			ret = GetNoteButton(next_block, ic);
		}
		else {
			/* 半分に分けて再計算。余った分は前半に適用 */
			uint next_block = block / 2 + 1;
			if (next_block <= ic) {
				ic -= next_block;
				next_block - 1;
			}
			ret = GetNoteButton(next_block, ic);
		}
		break;
	}
	return ret;
}

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
			buf_note.motion = GetMotionAssign(buf[ic]);
			buf_note.pos = option.now_shutpos + ic;
			buf_note.btn = GetNoteButton(option.now_block, ic);
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
static FBDF_mapenc_error_et FBDF_MapLoadOneCap(FBDF_map_t &map, const char *nex_music) {
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
			map.artist_name = buf;
			/* 改行消し */
			for (int ic = 0; ic < map.artist_name.size(); ic++) {
				if (map.artist_name[ic] == '\n') {
					map.artist_name.pop_back();
				}
			}
			/* 日本語補正 */
			map.artist_name = UTF8_converter(map.artist_name);
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

/**
 * @brief 譜面を読み込む
 * @param[out] map 格納先
 * @param[in] folder_name フォルダーの名前
 * @param[in] dif_type 難易度
 * @return FBDF_mapenc_error_et エラー情報
 */
FBDF_mapenc_error_et FBDF_MapLoadOne(
	FBDF_map_t &map, const char *folder_name, FBDF_dif_type_ec dif_type
) {
	bool match_dif = true;
	char str_buf[256] = "";
	FILE *fp = NULL;
	std::string base_path = "";
	std::string map_path = "";

	map.music_name = folder_name; /* 初期値として登録 */

	base_path = "music/";
	base_path += folder_name;
	base_path += "/base.txt";
	fopen_s(&fp, base_path.c_str(), "r");

	if (fp == NULL) { /* base.txtがなかったら、map.txtをNORMAL譜面として読み込む */
		if (dif_type != FBDF_dif_type_ec::NORMAL) { return FBDF_MAPENC_ERROR_FILE; }
		map_path = "music/";
		map_path += folder_name;
		map_path += "/map.txt";
		return FBDF_MapLoadOneCap(map, map_path.c_str());
	}

	while (fgets(str_buf, 256, fp) != NULL) {
		if (strands(str_buf, "[all]")) {
			match_dif = true;
		}
		else if (strands(str_buf, "[light]")) {
			match_dif = (dif_type == FBDF_dif_type_ec::LIGHT);
		}
		else if (strands(str_buf, "[normal]")) {
			match_dif = (dif_type == FBDF_dif_type_ec::NORMAL);
		}
		else if (strands(str_buf, "[hyper]")) {
			match_dif = (dif_type == FBDF_dif_type_ec::HYPER);
		}

		if (!match_dif) { continue; }

		if (strands(str_buf, "NAME:")) {
			strmods(str_buf, 5);
			map.music_name = str_buf;
			if (map.music_name.back() == '\n') { map.music_name.pop_back(); }
			map.music_name = UTF8_converter(map.music_name);
		}
		else if (strands(str_buf, "ARTIST:")) {
			strmods(str_buf, 7);
			map.artist_name = str_buf;
			if (map.artist_name.back() == '\n') { map.artist_name.pop_back(); }
			map.artist_name = UTF8_converter(map.artist_name);
		}
		else if (strands(str_buf, "BPM:")) {
			strmods(str_buf, 4);
			map.bpm = strtod(str_buf, NULL);
		}
		else if (strands(str_buf, "OFFSET:")) {
			strmods(str_buf, 7);
			map.offset = strtol(str_buf, NULL, 10);
		}
		else if (strands(str_buf, "MAP:")) {
			strmods(str_buf, 4);
			map.map_file_name = str_buf;
			if (map.map_file_name.back() == '\n') { map.map_file_name.pop_back(); }
			map.map_file_name = UTF8_converter(map.map_file_name);
		}
		else if (strands(str_buf, "LEVEL:")) {
			strmods(str_buf, 6);
			map.user_level = strtol(str_buf, NULL, 10);
		}
	}

	fclose(fp);

	map_path = "music/";
	map_path += folder_name;
	map_path += '/';
	map_path += map.map_file_name;

	return FBDF_MapLoadOneCap(map, map_path.c_str());
}

#endif /* ノーツデータ系 */

#if 1 /* 歌詞データ系 */

static FBDF_mapenc_error_et GetLyricsBlock(
	datacur_cursor_vector<FBDF_mapenc_lyrics_st> &lyrics, char const *buf, FBDF_map_enc_t &option
) {
	if (lyrics.isfull()) { /* 歌詞数が2000に達していたらこれ以上読み込まない */
		FBDF_ErrorLogWrite("歌詞数が多すぎます!");
		return FBDF_MAPENC_ERROR_NOTE_FULL;
	}
	if (option.now_block == 0) { /* ブロック数0とか意味わからんもの定義してないからダメ */
		FBDF_ErrorLogWrite("ブロック数に0を指定してノーツを読み込もうとしました。");
		return FBDF_MAPENC_ERROR_OPTION;
	}
	for (size_t ic = 0; ic < option.now_block; ic++) { /* 歌詞に関係しない文字が一個でもあったらダメ */
		if (!ISLYRICS(buf[ic])) {
			std::string log = "歌詞に関係ない文字が混ざっています。 (";
			log += buf[ic];
			log += ")";
			FBDF_ErrorLogWrite(log.c_str());
			return FBDF_MAPENC_ERROR_INVALID_NOTE_CHAR;
		}
	}

	for (size_t ic = 0; ic < option.now_block; ic++) {
		FBDF_mapenc_lyrics_st buf_lyrics;
		if (buf[ic] != '.') {
			switch (buf[ic]) {
			case 'a':
				buf_lyrics.mat = FBDF_LYRICS_MAT_A;
				break;
			case 'i':
			case 's':
			case 't':
				buf_lyrics.mat = FBDF_LYRICS_MAT_I;
				break;
			case 'u':
			case 'w':
				buf_lyrics.mat = FBDF_LYRICS_MAT_U;
				break;
			case 'e':
				buf_lyrics.mat = FBDF_LYRICS_MAT_E;
				break;
			case 'o':
				buf_lyrics.mat = FBDF_LYRICS_MAT_O;
				break;
			case 'n':
			case 'm':
			case 'b':
			case 'p':
				buf_lyrics.mat = FBDF_LYRICS_MAT_N;
				break;
			case 'f':
				buf_lyrics.mat = FBDF_LYRICS_MAT_FREE;
				break;
			}
			buf_lyrics.time = (
				60000 * 4 * ic /
				(option.now_bpm * option.scrool * option.measure_u * option.now_block) +
				game_option.note_offset_timing + option.now_shuttime
			);

			if (lyrics.lastData().mat != buf_lyrics.mat) { lyrics.push_back(buf_lyrics); }

			if (lyrics.isfull()) {
				FBDF_ErrorLogWrite("歌詞数が多すぎます!");
				return FBDF_MAPENC_ERROR_NOTE_FULL;
			}
		}
	}
	option.now_shutpos += option.now_block;
	option.now_shuttime += 60000 * 4 / (double)(option.now_bpm * option.scrool * option.measure_u);
	return FBDF_MAPENC_ERROR_NONE;
}

static FBDF_mapenc_error_et GetLyricsLine(
	datacur_cursor_vector<FBDF_mapenc_lyrics_st> &lyrics, const char *buf, FBDF_map_enc_t &option
) {
	FBDF_mapenc_error_et err = FBDF_MAPENC_ERROR_NONE;
	std::string strbuf = buf;

	while (0 < strbuf.size()) {
		err = GetLyricsBlock(lyrics, strbuf.c_str(), option);
		if (err != FBDF_MAPENC_ERROR_NONE) {
			return err;
		}
		strbuf.erase(0, option.now_block);
	}

	return FBDF_MAPENC_ERROR_NONE;
}

FBDF_mapenc_error_et FBDF_Mapenc_LyricsEnc(
	datacur_cursor_vector<FBDF_mapenc_lyrics_st> &lyrics, const char *file_path
) {
	char buf[256] = "";
	FILE *fp;
	FBDF_mapenc_error_et err = FBDF_MAPENC_ERROR_NONE;
	FBDF_map_enc_t option;

	lyrics.clear();
	lyrics.push_back({FBDF_LYRICS_MAT_FREE, 0});

	fopen_s(&fp, file_path, "r");
	if (fp == nullptr) { return FBDF_MAPENC_ERROR_FILE; }
	
	while (fgets(buf, 256, fp) != NULL) {
		if (buf[0] == _T(';')) {
			; // nothing
		}
		else if (strands(buf, "BPM:")) {
			strmods(buf, 4);
			option.now_bpm = strsansD(buf, 256);
		}
		else if (strands(buf, "OFFSET:")) {
			strmods(buf, 7);
			option.now_shuttime = strtol(buf, NULL, 10);
		}
		else if (strands(buf, "BLOCK:")) {
			strmods(buf, 6);
			option.now_block = strtol(buf, NULL, 10);
		}
		else {
			for (size_t i = 0; buf[i] != '\0'; i++) {
				if (buf[i] == '\n') {
					buf[i] = '\0';
					break;
				}
			}
			FBDF_mapenc_error_et err_buf = GetLyricsLine(lyrics, buf, option);
			if (err_buf != FBDF_MAPENC_ERROR_NONE) { err = err_buf; } /* エラーあっても最後まで読み切る */
		}
	}

	fclose(fp);

	return FBDF_MAPENC_ERROR_NONE;
}

#endif /* 歌詞データ系 */
