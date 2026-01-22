
#include <string>

#include <DxLib.h>

#include <save.h>

FBDF_dif_type_ec &operator++(FBDF_dif_type_ec &val) {
    switch (val) {
    case FBDF_dif_type_ec::NONE: /* ó·äO */
        val = FBDF_dif_type_ec::LIGHT;
        break;
    case FBDF_dif_type_ec::HYPER: /* è„å¿ */
        val = FBDF_dif_type_ec::HYPER;
        break;
    default:
        val = static_cast<FBDF_dif_type_ec>(static_cast<int>(val) + 1);
    }
    return val;
}

FBDF_dif_type_ec &operator--(FBDF_dif_type_ec &val) {
    switch (val) {
    case FBDF_dif_type_ec::NONE: /* ó·äO */
        val = FBDF_dif_type_ec::HYPER;
        break;
    case FBDF_dif_type_ec::LIGHT: /* â∫å¿ */
        val = FBDF_dif_type_ec::LIGHT;
        break;
    default:
        val = static_cast<FBDF_dif_type_ec>(static_cast<int>(val) - 1);
    }
    return val;
}

bool FBDF_Save_ReadScoreAllDif(FBDF_file_music_score_st dest[], const TCHAR *music_folder_name) {
    std::string save_path;
    FILE *fp;

    save_path  = "save/";
    save_path += music_folder_name;
    save_path += ".dat";

    fopen_s(&fp, save_path.c_str(), "rb");
    if (fp == NULL) { return false; }
    fread(dest, sizeof(FBDF_file_music_score_st), 3, fp);
    fclose(fp);

    return true;
}

bool FBDF_Save_WriteScoreAllDif(const FBDF_file_music_score_st src[], const TCHAR *music_folder_name) {
    std::string save_path;
    FILE *fp;

    save_path  = "save/";
    save_path += music_folder_name;
    save_path += ".dat";

    fopen_s(&fp, save_path.c_str(), "wb");
    if (fp == NULL) { return false; }
    fwrite(src, sizeof(FBDF_file_music_score_st), 3, fp);
    fclose(fp);

    return true;
}

bool FBDF_Save_ReadScoreOneDif(FBDF_file_music_score_st *dest, const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type) {
    FBDF_file_music_score_st buf[3];
    buf[0].acc = 0.0;
    buf[0].clear_type = FBDF_CLEAR_TYPE_NOPLAY;
    buf[0].score = 0;
    buf[1].acc = 0.0;
    buf[1].clear_type = FBDF_CLEAR_TYPE_NOPLAY;
    buf[1].score = 0;
    buf[2].acc = 0.0;
    buf[2].clear_type = FBDF_CLEAR_TYPE_NOPLAY;
    buf[2].score = 0;
    if (FBDF_Save_ReadScoreAllDif(buf, music_folder_name) != true) { return false; }
    switch (dif_type) {
    case FBDF_dif_type_ec::LIGHT:
        *dest = buf[0];
        break;
    case FBDF_dif_type_ec::NORMAL:
        *dest = buf[1];
        break;
    case FBDF_dif_type_ec::HYPER:
        *dest = buf[2];
        break;
    }
    return true;
}

bool FBDF_Save_WriteScoreOneDif(const FBDF_file_music_score_st *src, const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type) {
    FBDF_file_music_score_st buf[3];
    buf[0].acc = 0.0;
    buf[0].clear_type = FBDF_CLEAR_TYPE_NOPLAY;
    buf[0].score = 0;
    buf[1].acc = 0.0;
    buf[1].clear_type = FBDF_CLEAR_TYPE_NOPLAY;
    buf[1].score = 0;
    buf[2].acc = 0.0;
    buf[2].clear_type = FBDF_CLEAR_TYPE_NOPLAY;
    buf[2].score = 0;
    FBDF_Save_ReadScoreAllDif(buf, music_folder_name); /* readÇ≈Ç´ÇƒÇ»Ç≠ÇƒÇ‡ó«Ç¢ */
    switch (dif_type) {
    case FBDF_dif_type_ec::LIGHT:
        buf[0] = *src;
        break;
    case FBDF_dif_type_ec::NORMAL:
        buf[1] = *src;
        break;
    case FBDF_dif_type_ec::HYPER:
        buf[2] = *src;
        break;
    }
    return FBDF_Save_WriteScoreAllDif(buf, music_folder_name);
}

bool FBDF_Save_UpdateScoreOneDif(const FBDF_file_music_score_st *src, const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type) {
    FBDF_file_music_score_st buf;
    buf.acc = 0.0;
    buf.clear_type = FBDF_CLEAR_TYPE_NOPLAY;
    buf.score = 0;
    FBDF_Save_ReadScoreOneDif(&buf, music_folder_name, dif_type); /* readÇ≈Ç´ÇƒÇ»Ç≠ÇƒÇ‡ó«Ç¢ */

    if (buf.acc        < src->acc       ) { buf.acc        = src->acc;        }
    if (buf.clear_type < src->clear_type) { buf.clear_type = src->clear_type; }
    if (buf.score      < src->score     ) { buf.score      = src->score;      }

    return FBDF_Save_WriteScoreOneDif(&buf, music_folder_name, dif_type);
}
