
#include <string>
#include <vector>

#include <strcur.h>
#include <dxcur.h>

#include <system.h>

#include <motionenc.h>

/**
 * @brief ダンサーの振り付けを読み込む
 * @param[out] motion 格納先
 * @param[in] folder_path ダンサーのフォルダー名
 * @return FBDF_mapenc_error_et エラー情報
 */
int FBDF_DancerMotionEnc(std::vector<FBDF_Play_motion_st> &motion, const char *folder_name) {
	char buf[256];

	FBDF_Play_motion_st motion_buf;

    std::string folder_path = "dancer/";
    folder_path += folder_name;
    folder_path += '/';

    std::string file_path = folder_path;
    file_path += "data.txt";

	FILE *fp = NULL;

	fopen_s(&fp, file_path.c_str(), "r");

	if (fp == NULL) { return -1; }

	while (fgets(buf, 256, fp) != NULL) {
		if (buf[0] == _T(';')) {
			; // nothing
		}
		else if (buf[0] == _T('\n')) {
			; // nothing
		}
#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
		else if (strands(buf, "IMAGE:")) {
			strmods(buf, 6);
			motion_buf.image_path = buf;
			/* 改行消し */
			for (int ic = 0; ic < motion_buf.image_path.size(); ic++) {
				if (motion_buf.image_path[ic] == '\n') {
					motion_buf.image_path.pop_back();
				}
			}
		}
#endif /* 画像 */
		else if (strands(buf, "LEN:")) {
			FBDF_Play_motion_len_st &insert_buf = motion_buf.len;
			strmods(buf, 4);
			while (buf[0] != '\0') {
				switch (buf[0]) {
				case '1':
					insert_buf.n1 = true;
					break;
				case '2':
					insert_buf.n2 = true;
					break;
				case '3':
					insert_buf.n3 = true;
					break;
				case '4':
					insert_buf.n4 = true;
					break;
				case '8':
					insert_buf.n8 = true;
					break;
				}
				strmods(buf, 1);
			}
		}
		else if (strands(buf, "TYPE:")) {
			FBDF_Play_motion_type_st &insert_buf = motion_buf.type;
			strmods(buf, 5);
			while (buf[0] != '\0') {
				switch (buf[0]) {
				case 'u':
					insert_buf.up = true;
					break;
				case 'd':
					insert_buf.down = true;
					break;
				case 'l':
					insert_buf.left = true;
					break;
				case 'r':
					insert_buf.right = true;
					break;
				case 'f':
					insert_buf.front = true;
					break;
				case 'b':
					insert_buf.back = true;
					break;
				case 'j':
					insert_buf.jump = true;
					break;
				case 'c':
					insert_buf.clap = true;
					break;
				case 't':
					insert_buf.turn = true;
					break;
				case '1':
					insert_buf.n1 = true;
					break;
				case '2':
					insert_buf.n2 = true;
					break;
				case '3':
					insert_buf.n3 = true;
					break;
				case '4':
					insert_buf.n4 = true;
					break;
				case 'v':
					insert_buf.vpose = true;
					break;
				}
				strmods(buf, 1);
			}
		}
		else if (strands(buf, "NEXT:")) {
			strmods(buf, 5);
			while (buf[0] != '\0') {
				motion_buf.next.push_back(strtol(buf, NULL, 10));
				strnex_EX2(buf, 256, ',');
			}
		}
		else if (strands(buf, "EXTRA")) {
			motion_buf.extra = true;
		}
		else if (strands(buf, "DATAIN")) {
#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
			/* 画像読み込み */
			int pic_num = 1;
			if (motion_buf.type_8) {
				pic_num = 9;
			}
			else if (motion_buf.type_4) {
				pic_num = 5;
			}
			else if (motion_buf.type_3) {
				pic_num = 4;
			}
			else if (motion_buf.type_2) {
				pic_num = 3;
			}
			else {
				pic_num = 2;
			}
			motion_buf.pic = dxcur_divpic_c(motion_buf.image_path.c_str(), pic_num, pic_num, 1);
#endif /* 画像 */
			/* 登録 */
			motion.push_back(motion_buf);
#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
			motion_buf.image_path.clear();
			motion_buf.pic.clear();
#endif /* 画像 */
			motion_buf.next.clear();
			motion_buf.len.n1 = false;
			motion_buf.len.n2 = false;
			motion_buf.len.n3 = false;
			motion_buf.len.n4 = false;
			motion_buf.len.n8 = false;
			motion_buf.type.up = false;
			motion_buf.type.down = false;
			motion_buf.type.left = false;
			motion_buf.type.right = false;
			motion_buf.type.front = false;
			motion_buf.type.back = false;
			motion_buf.type.jump = false;
			motion_buf.type.clap = false;
			motion_buf.type.turn = false;
			motion_buf.type.n1 = false;
			motion_buf.type.n2 = false;
			motion_buf.type.n3 = false;
			motion_buf.type.n4 = false;
			motion_buf.type.vpose = false;
		}
	}
	fclose(fp);

	return 0;
}
