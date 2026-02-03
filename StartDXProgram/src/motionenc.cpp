
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

    size_t motionNo = 0;

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
		else if (strands(buf, "TYPE:")) {
			strmods(buf, 5);
			while (buf[0] != '\0') {
				switch (buf[0]) {
				case '1':
					motion_buf.type_1 = true;
					break;
				case '2':
					motion_buf.type_2 = true;
					break;
				case '3':
					motion_buf.type_3 = true;
					break;
				case '4':
					motion_buf.type_4 = true;
					break;
				case '8':
					motion_buf.type_8 = true;
					break;
				case 'u':
					motion_buf.type_u = true;
					break;
				case 'd':
					motion_buf.type_d = true;
					break;
				case 'l':
					motion_buf.type_l = true;
					break;
				case 'r':
					motion_buf.type_r = true;
					break;
				case 'f':
					motion_buf.type_f = true;
					break;
				case 'b':
					motion_buf.type_b = true;
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
			motion_buf.type_1 = false;
			motion_buf.type_2 = false;
			motion_buf.type_3 = false;
			motion_buf.type_4 = false;
			motion_buf.type_8 = false;
			motion_buf.type_u = false;
			motion_buf.type_d = false;
			motion_buf.type_l = false;
			motion_buf.type_r = false;
			motion_buf.type_f = false;
			motion_buf.type_b = false;
		}
	}
	fclose(fp);

	return 0;
}
