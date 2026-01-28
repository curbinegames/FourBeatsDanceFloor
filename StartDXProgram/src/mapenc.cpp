
#include <vector>
#include <string>

#include <DxLib.h>

#include <sancur.h>
#include <strcur.h>
#include <UTF8_conv.h>

#include <mapenc.h>

#define ISNOTE(c) ((c) == '-' || (c) == '.')

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
int GetNoteBlock(FBDF_map_t *map, char const *buf, FBDF_map_enc_t *option) {
	if (2000 <= map->noteNo) { return 1; }
	if (option->now_block == 0) { return 1; }
	for (int ic = 0; ic < option->now_block; ic++) {
		if (!ISNOTE(buf[ic])) {
			return 1;
		}
	}
	for (int ic = 0; ic < option->now_block; ic++) {
		FBDF_note_t buf_note;
		if (buf[ic] == '-') {
			buf_note.pos = option->now_shutpos + ic;
			switch (option->now_block) {
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
			if (map->noteNo != 0) {
				map->note[map->note.size() - 1].len = buf_note.pos - map->note[map->note.size() - 1].pos;
			}
			buf_note.time = option->now_shuttime + 60000 * 4 * ic / (option->now_bpm * option->scrool * option->measure_u * option->now_block);
			buf_note.mtime = 0;
			if (map->noteNo != 0) {
				map->note[map->note.size() - 1].mtime = buf_note.time - map->note[map->note.size() - 1].time;
			}
			buf_note.bpm = option->now_bpm;
			map->note.push_back(buf_note);
			map->Etime = buf_note.time;
			map->noteNo++;
			map->noteN++;
			if (2000 <= map->noteNo) { return 1; }
		}
	}
	option->now_shutpos += option->now_block;
	option->now_shuttime += 60000 * 4 / (double)(option->now_bpm * option->scrool * option->measure_u);
	map->blockNo++;
	return 0;
}

/**
 * @brief ノーツ情報を読み込む。文字列版。
 * @param[out] map 格納先
 * @param[in] buf 読み込む文字列
 * @param[out] option 譜面読み込みのオプション
 * @return FBDF_mapenc_error_et エラー情報
 */
int GetNoteLine(FBDF_map_t *map, char *buf, FBDF_map_enc_t *option) {
	while (GetNoteBlock(map, buf, option) == 0) {
		strmods(buf, option->now_block);
	}
	return 0;
}

/**
 * @brief 譜面を読み込む
 * @param[out] map 格納先
 * @param[in] nex_music 譜面ファイルのパス
 * @return FBDF_mapenc_error_et エラー情報
 */
int MapLoadOne(FBDF_map_t *map, const char *nex_music) {
	char buf[256];
	char musicPath[96];

	FBDF_map_enc_t option;

	FILE *fp = NULL;

	strcpy_s(musicPath, sizeof(musicPath), nex_music);

	fopen_s(&fp, musicPath, "r");
	if (fp == NULL) { return -1; }

	while (fgets(buf, 256, fp) != NULL) {
		if (strands(buf, "BPM:")) {
			strmods(buf, 4);
			map->bpm = strsansD(buf, 256);
			option.now_bpm = map->bpm;
		}
		else if (strands(buf, "OFFSET:")) {
			strmods(buf, 7);
			map->offset = strtol(buf, NULL, 10);
			option.now_shuttime = map->offset;
		}
		else if (strands(buf, "ARTIST:")) {
			strmods(buf, 7);
			map->artist = buf;
			/* 改行消し */
			for (int ic = 0; ic < map->artist.size(); ic++) {
				if (map->artist[ic] == '\n') {
					map->artist.pop_back();
				}
			}
			/* 日本語補正 */
			map->artist = UTF8_converter(map->artist);
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
			GetNoteLine(map, buf, &option);
		}
	}
	fclose(fp);

	/* ラストノート挿入 */ {
		FBDF_note_t buf_note;
		map->note.push_back(buf_note);
	}

	return 0;
}
