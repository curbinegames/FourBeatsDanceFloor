
#include <dxcur.h>

#include <datacur.h>
#include <mapenc.h>

#include <CalDif.h>

/* numがtarget±gap以内であればtrueを返すdefine */
#define IS_NEAR_NUM(num, target, gap) (((target) - (gap)) <= (num) && (num) <= ((target) + (gap)))

typedef struct FBDF_music_colorpat_count_s {
	uint pat111 = 0;
	uint pat112 = 0;
	uint pat113 = 0;
	uint pat114 = 0;
	uint pat121 = 0;
	uint pat122 = 0;
	uint pat123 = 0;
	uint pat124 = 0;
	uint pat131 = 0;
	uint pat132 = 0;
	uint pat133 = 0;
	uint pat134 = 0;
	uint pat141 = 0;
	uint pat142 = 0;
	uint pat143 = 0;
	uint pat144 = 0;
	uint pat211 = 0;
	uint pat212 = 0;
	uint pat213 = 0;
	uint pat214 = 0;
	uint pat221 = 0;
	uint pat222 = 0;
	uint pat223 = 0;
	uint pat224 = 0;
	uint pat231 = 0;
	uint pat232 = 0;
	uint pat233 = 0;
	uint pat234 = 0;
	uint pat241 = 0;
	uint pat242 = 0;
	uint pat243 = 0;
	uint pat244 = 0;
	uint pat311 = 0;
	uint pat312 = 0;
	uint pat313 = 0;
	uint pat314 = 0;
	uint pat321 = 0;
	uint pat322 = 0;
	uint pat323 = 0;
	uint pat324 = 0;
	uint pat331 = 0;
	uint pat332 = 0;
	uint pat333 = 0;
	uint pat334 = 0;
	uint pat341 = 0;
	uint pat342 = 0;
	uint pat343 = 0;
	uint pat344 = 0;
	uint pat411 = 0;
	uint pat412 = 0;
	uint pat413 = 0;
	uint pat414 = 0;
	uint pat421 = 0;
	uint pat422 = 0;
	uint pat423 = 0;
	uint pat424 = 0;
	uint pat431 = 0;
	uint pat432 = 0;
	uint pat433 = 0;
	uint pat434 = 0;
	uint pat441 = 0;
	uint pat442 = 0;
	uint pat443 = 0;
	uint pat444 = 0;
} FBDF_music_colorpat_count_t;

/* notes難易度を計算する */
double FBDF_CalMapNotesDif(const cvec<FBDF_note_t> &notes) {
	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (notes.size() < 3) { return 0; }

	for (uint i = 2; i < notes.size(); i++) {
		int FirstTime  = notes[i].time     - notes[i - 1].time;
		int SecondTime = notes[i - 1].time - notes[i - 2].time;

		uint BasePoint = 10;

		DxTime_t TimeGap = 0;
		TimeGap = 2000 / maxs_2(1, notes[i].time - notes[i - 1].time);
		BasePoint *= TimeGap;
		BasePointQueue.push_back(BasePoint);
		if (50 <= BasePointQueue.size()) {
			BasePointQueue.erase(BasePointQueue.begin());
		}

		double NowDif = 0.0;
		double mlp = 100.0;
		for (int iq = BasePointQueue.size() - 1; 0 <= iq; iq--) {
			NowDif += BasePointQueue[iq] * mlp;
			mlp *= 0.95;
		}

		if (ret < NowDif) {
			ret = NowDif;
		}
	}

	ret = lins(110000, 1, 310000, 10, ret);

	return ret;
}

/* color難易度を計算する */
double FBDF_CalMapColorDif(const cvec<FBDF_note_t> &notes) {
	const int point_stairLen =  2;
	const int point_step_11  = 10;
	const int point_step_13  = 12;
	const int point_step_33  = 13;
	const int point_3_hop1   = 14;
	const int point_stair1   = 17;
	const int point_3_hop3   = 21;
	const int point_3_hop4   = 21;
	const int point_step_22  = 24;
	const int point_step_44  = 24;
	const int point_stair3   = 27;
	const int point_step_24  = 31;
	const int point_3_hop2   = 34;
	const int point_stair2   = 40;
	const int point_stair4   = 40;

	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (notes.size() < 3) { return 0; }

	for (uint i = 2; i < notes.size(); i++) {
		int FirstTime  = notes[i].time     - notes[i - 1].time;
		int SecondTime = notes[i - 1].time - notes[i - 2].time;
		uint BasePoint;
		bool skipFG = false;

		switch (notes[i].btn) {
		case FBDF_PLAY_NOTE_BTN_1:
			switch (notes[i - 1].btn) {
			case 1: /* 11ステップ */
				BasePoint = point_step_11;
				break;
			case 2: /* 3空き */
				BasePoint = point_3_hop1;
				break;
			case 3: /* 13ステップ */
				BasePoint = point_step_13;
				break;
			case 4: /* 階段 */
				if (notes[i - 2].btn != 3) {
					BasePoint = point_stair4;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		case FBDF_PLAY_NOTE_BTN_2:
			switch (notes[i - 1].btn) {
			case 2: /* 22ステップ */
				BasePoint = point_step_22;
				break;
			case 3: /* 3空き */
				BasePoint = point_3_hop2;
				break;
			case 4: /* 24ステップ */
				BasePoint = point_step_24;
				break;
			case 1: /* 階段 */
				if (notes[i - 2].btn != 4) {
					BasePoint = point_stair1;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		case FBDF_PLAY_NOTE_BTN_3:
			switch (notes[i - 1].btn) {
			case 3: /* 33ステップ */
				BasePoint = point_step_33;
				break;
			case 4: /* 3空き */
				BasePoint = point_3_hop3;
				break;
			case 1: /* 13ステップ */
				BasePoint = point_step_13;
				break;
			case 2: /* 階段 */
				if (notes[i - 2].btn != 3) {
					BasePoint = point_stair2;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		case FBDF_PLAY_NOTE_BTN_4:
			switch (notes[i - 1].btn) {
			case 4: /* 44ステップ */
				BasePoint = point_step_44;
				break;
			case 1: /* 3空き */
				BasePoint = point_3_hop4;
				break;
			case 2: /* 24ステップ */
				BasePoint = point_step_24;
				break;
			case 3: /* 階段 */
				if (notes[i - 2].btn != 2) {
					BasePoint = point_stair3;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		default:
			BasePoint = 0;
		}

#if 1
		DxTime_t TimeGap = 0;
		TimeGap = 2000 / maxs_2(1, notes[i].time - notes[i - 1].time);
		BasePoint *= TimeGap;
#endif
		BasePointQueue.push_back(BasePoint);
		if (50 <= BasePointQueue.size()) {
			BasePointQueue.erase(BasePointQueue.begin());
		}

		double NowDif = 0.0;
		double mlp = 100.0;
		for (int iq = BasePointQueue.size() - 1; 0 <= iq; iq--) {
			NowDif += BasePointQueue[iq] * mlp;
			mlp *= 0.95;
		}

		if (ret < NowDif) {
			ret = NowDif;
		}
	}

	ret = lins(130000, 1, 570000, 8, ret);

	return ret;
}

/* trick難易度を計算する */
double FBDF_CalMapTrickDif(const cvec<FBDF_note_t> &notes) {
	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (notes.size() < 3) { return 0; }

	for (uint i = 2; i < notes.size(); i++) {
		int FirstTime  = notes[i].time     - notes[i - 1].time;
		int SecondTime = notes[i - 1].time - notes[i - 2].time;
		uint BasePoint;
		float Gap = 0.0;

		if (FirstTime < SecondTime) {
			int temp = FirstTime;
			FirstTime = SecondTime;
			SecondTime = temp;
		}

		Gap = (float)FirstTime / (float)SecondTime;

		/* レコランのtrickから取ってきたけど、リズム難の比重が大きすぎたから下方修正してる */
		if (7 < Gap) {
			BasePoint = 2; /* 2 */
		}
		else if (IS_NEAR_NUM(Gap, 1.000, 0.1)) {
			BasePoint = 5; /* 5 */
		}
		else if (IS_NEAR_NUM(Gap, 2.000, 0.1)) {
			BasePoint = 10; /* 10 */
		}
		else if (IS_NEAR_NUM(Gap, 1.500, 0.1)) {
			BasePoint = 13; /* 13 */
		}
		else if (IS_NEAR_NUM(Gap, 6.000, 0.1)) {
			BasePoint = 14; /* 14 */
		}
		else if (IS_NEAR_NUM(Gap, 3.000, 0.1)) {
			BasePoint = 15; /* 15 */
		}
		else if (IS_NEAR_NUM(Gap, 7.000, 0.1)) {
			BasePoint = 17; /* 17 */
		}
		else if (IS_NEAR_NUM(Gap, 4.000, 0.1)) {
			BasePoint = 19; /* 19 */
		}
		else if (IS_NEAR_NUM(Gap, 1.333, 0.1)) {
			BasePoint = 19; /* 19 */
		}
		else if (IS_NEAR_NUM(Gap, 5.000, 0.1)) {
			BasePoint = 23; /* 23 */
		}
		else if (IS_NEAR_NUM(Gap, 2.500, 0.1)) {
			BasePoint = 23; /* 23 */
		}
		else if (IS_NEAR_NUM(Gap, 1.667, 0.1)) {
			BasePoint = 27; /* 27 */
		}
		else {
			BasePoint = 30; /* 30 */
		}

#if 1
		DxTime_t TimeGap = 0;
		TimeGap = 2000 / maxs_2(1, notes[i].time - notes[i - 1].time);
		BasePoint *= TimeGap;
#endif
		BasePointQueue.push_back(BasePoint);
		if (50 <= BasePointQueue.size()) {
			BasePointQueue.erase(BasePointQueue.begin());
		}

		double NowDif = 0.0;
		double mlp = 100.0;
		for (int iq = BasePointQueue.size() - 1; 0 <= iq; iq--) {
			NowDif += BasePointQueue[iq] * mlp;
			mlp *= 0.95;
		}

		if (ret < NowDif) {
			ret = NowDif;
		}
	}

	ret = lins(86000, 1, 240000, 8, ret);

	return ret;
}

/* カラーパターンを数える */
static void FBDF_CountMapColorPat(FBDF_music_colorpat_count_t *pat, const cvec<FBDF_note_t> &notes) {
	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (notes.size() < 3) { return; }

	for (uint i = 2; i < notes.size(); i++) {
		switch (notes[i - 2].btn) {
		case 1:
			switch (notes[i - 1].btn) {
			case 1:
				switch (notes[i].btn) {
				case 1:
					pat->pat111++;
					break;
				case 2:
					pat->pat112++;
					break;
				case 3:
					pat->pat113++;
					break;
				case 4:
					pat->pat114++;
					break;
				}
				break;
			case 2:
				switch (notes[i].btn) {
				case 1:
					pat->pat121++;
					break;
				case 2:
					pat->pat122++;
					break;
				case 3:
					pat->pat123++;
					break;
				case 4:
					pat->pat124++;
					break;
				}
				break;
			case 3:
				switch (notes[i].btn) {
				case 1:
					pat->pat131++;
					break;
				case 2:
					pat->pat132++;
					break;
				case 3:
					pat->pat133++;
					break;
				case 4:
					pat->pat134++;
					break;
				}
				break;
			case 4:
				switch (notes[i].btn) {
				case 1:
					pat->pat141++;
					break;
				case 2:
					pat->pat142++;
					break;
				case 3:
					pat->pat143++;
					break;
				case 4:
					pat->pat144++;
					break;
				}
				break;
			}
			break;
		case 2:
			switch (notes[i - 1].btn) {
			case 1:
				switch (notes[i].btn) {
				case 1:
					pat->pat211++;
					break;
				case 2:
					pat->pat212++;
					break;
				case 3:
					pat->pat213++;
					break;
				case 4:
					pat->pat214++;
					break;
				}
				break;
			case 2:
				switch (notes[i].btn) {
				case 1:
					pat->pat221++;
					break;
				case 2:
					pat->pat222++;
					break;
				case 3:
					pat->pat223++;
					break;
				case 4:
					pat->pat224++;
					break;
				}
				break;
			case 3:
				switch (notes[i].btn) {
				case 1:
					pat->pat231++;
					break;
				case 2:
					pat->pat232++;
					break;
				case 3:
					pat->pat233++;
					break;
				case 4:
					pat->pat234++;
					break;
				}
				break;
			case 4:
				switch (notes[i].btn) {
				case 1:
					pat->pat241++;
					break;
				case 2:
					pat->pat242++;
					break;
				case 3:
					pat->pat243++;
					break;
				case 4:
					pat->pat244++;
					break;
				}
				break;
			}
			break;
		case 3:
			switch (notes[i - 1].btn) {
			case 1:
				switch (notes[i].btn) {
				case 1:
					pat->pat311++;
					break;
				case 2:
					pat->pat312++;
					break;
				case 3:
					pat->pat313++;
					break;
				case 4:
					pat->pat314++;
					break;
				}
				break;
			case 2:
				switch (notes[i].btn) {
				case 1:
					pat->pat321++;
					break;
				case 2:
					pat->pat322++;
					break;
				case 3:
					pat->pat323++;
					break;
				case 4:
					pat->pat324++;
					break;
				}
				break;
			case 3:
				switch (notes[i].btn) {
				case 1:
					pat->pat331++;
					break;
				case 2:
					pat->pat332++;
					break;
				case 3:
					pat->pat333++;
					break;
				case 4:
					pat->pat334++;
					break;
				}
				break;
			case 4:
				switch (notes[i].btn) {
				case 1:
					pat->pat341++;
					break;
				case 2:
					pat->pat342++;
					break;
				case 3:
					pat->pat343++;
					break;
				case 4:
					pat->pat344++;
					break;
				}
				break;
			}
			break;
		case 4:
			switch (notes[i - 1].btn) {
			case 1:
				switch (notes[i].btn) {
				case 1:
					pat->pat411++;
					break;
				case 2:
					pat->pat412++;
					break;
				case 3:
					pat->pat413++;
					break;
				case 4:
					pat->pat414++;
					break;
				}
				break;
			case 2:
				switch (notes[i].btn) {
				case 1:
					pat->pat421++;
					break;
				case 2:
					pat->pat422++;
					break;
				case 3:
					pat->pat423++;
					break;
				case 4:
					pat->pat424++;
					break;
				}
				break;
			case 3:
				switch (notes[i].btn) {
				case 1:
					pat->pat431++;
					break;
				case 2:
					pat->pat432++;
					break;
				case 3:
					pat->pat433++;
					break;
				case 4:
					pat->pat434++;
					break;
				}
				break;
			case 4:
				switch (notes[i].btn) {
				case 1:
					pat->pat441++;
					break;
				case 2:
					pat->pat442++;
					break;
				case 3:
					pat->pat443++;
					break;
				case 4:
					pat->pat444++;
					break;
				}
				break;
			}
			break;
		}
	}

	return;
}

/* 頻出のカラーパターンを調べる */
void FBDF_CalMapMostColorPat(FBDF_music_most_colorpat_t *mostpat, const FBDF_map_t *map) {
	FBDF_music_colorpat_count_t pat;
	FBDF_CountMapColorPat(&pat, map->note);
	for (int ip = 0; ip < 63; ip++) {
		switch (ip) {

#define cat(num, pattan)                                               \
	case num:                                                          \
		if (mostpat[MOST_COLORPAT_NUM - 1].count <  pat.pat##pattan) { \
			mostpat[MOST_COLORPAT_NUM - 1].count =  pat.pat##pattan;   \
			mostpat[MOST_COLORPAT_NUM - 1].mat = COLOR_PAT_##pattan;   \
		}                                                              \
	break

			//cat( 0, 111);
			cat( 1, 112);
			//cat( 2, 113);
			cat( 3, 114);
			cat( 4, 121);
			cat( 5, 122);
			//cat( 6, 123);
			cat( 7, 124);
			//cat( 8, 131);
			cat( 9, 132);
			//cat(10, 133);
			cat(11, 134);
			cat(12, 141);
			cat(13, 142);
			cat(14, 143);
			cat(15, 144);
			cat(16, 211);
			cat(17, 212);
			cat(18, 213);
			cat(19, 214);
			cat(20, 221);
			cat(21, 222);
			cat(22, 223);
			cat(23, 224);
			cat(24, 231);
			cat(25, 232);
			cat(26, 233);
			//cat(27, 234);
			cat(28, 241);
			cat(29, 242);
			cat(30, 243);
			cat(31, 244);
			//cat(32, 311);
			cat(33, 312);
			//cat(34, 313);
			cat(35, 314);
			cat(36, 321);
			cat(37, 322);
			cat(38, 323);
			cat(39, 324);
			//cat(40, 331);
			cat(41, 332);
			//cat(42, 333);
			cat(43, 334);
			//cat(44, 341);
			cat(45, 342);
			cat(46, 343);
			cat(47, 344);
			cat(48, 411);
			//cat(49, 412);
			cat(50, 413);
			cat(51, 414);
			cat(52, 421);
			cat(53, 422);
			cat(54, 423);
			cat(55, 424);
			cat(56, 431);
			cat(57, 432);
			cat(58, 433);
			cat(59, 434);
			cat(60, 441);
			cat(61, 442);
			cat(62, 443);
			cat(63, 444);
		default:
			break;

#undef cat

		}
		for (int in = MOST_COLORPAT_NUM - 1; 1 <= in; in--) {
			if (mostpat[in].count <= mostpat[in - 1].count) { break; }
			FBDF_music_most_colorpat_t temp = mostpat[in];
			mostpat[in] = mostpat[in - 1];
			mostpat[in - 1] = temp;
		}
	}
	return;
}

/* 色の出現頻度を数える */
void FBDF_CountMapColor(
	FBDF_music_colorcount_t *count, const cvec<FBDF_note_t> &notes, uint Length
) {
	if (notes.size() < 3) { return; }

	for (uint i = 0; i < notes.size(); i++) {
		switch (notes[i].btn) {
		case 1:
			count->c1++;
			break;
		case 2:
			count->c2++;
			break;
		case 3:
			count->c3++;
			break;
		case 4:
			count->c4++;
			break;
		}
	}

	count->c1 = DIV_AVOID_ZERO(count->c1, Length / 10000.0, 0);
	count->c2 = DIV_AVOID_ZERO(count->c2, Length / 10000.0, 0);
	count->c3 = DIV_AVOID_ZERO(count->c3, Length / 10000.0, 0);
	count->c4 = DIV_AVOID_ZERO(count->c4, Length / 10000.0, 0);

	return;
}
