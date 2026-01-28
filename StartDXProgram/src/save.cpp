
#include <string>

#include <DxLib.h>

#include <save.h>

/* 前置インクリメントの定義 */
FBDF_dif_type_ec &operator++(FBDF_dif_type_ec &val) {
    switch (val) {
    case FBDF_dif_type_ec::NONE: /* 例外 */
        val = FBDF_dif_type_ec::LIGHT;
        break;
    case FBDF_dif_type_ec::HYPER: /* 上限 */
        val = FBDF_dif_type_ec::HYPER;
        break;
    default:
        val = static_cast<FBDF_dif_type_ec>(static_cast<int>(val) + 1);
    }
    return val;
}

/* 前置デクリメントの定義 */
FBDF_dif_type_ec &operator--(FBDF_dif_type_ec &val) {
    switch (val) {
    case FBDF_dif_type_ec::NONE: /* 例外 */
        val = FBDF_dif_type_ec::HYPER;
        break;
    case FBDF_dif_type_ec::LIGHT: /* 下限 */
        val = FBDF_dif_type_ec::LIGHT;
        break;
    default:
        val = static_cast<FBDF_dif_type_ec>(static_cast<int>(val) - 1);
    }
    return val;
}

/**
 * @brief ユーザースコアデータを全部の難易度分読み込む
 * @param[out] dest 格納先、配列数は3
 * @param[in] music_folder_name 曲のフォルダ名
 * @return bool true=成功, false=失敗
 */
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

/**
 * @brief ユーザースコアデータを全部の難易度分書き込む。すべてを上書きするので注意
 * @param[in] src 保存するデータ、配列数は3
 * @param[in] music_folder_name 曲のフォルダ名
 * @return bool true=成功, false=失敗
 */
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

/**
 * @brief ユーザースコアデータを指定した難易度だけ読み込む
 * @param[out] dest 格納先
 * @param[in] music_folder_name 曲のフォルダ名
 * @param[in] dif_type 難易度タイプ
 * @return bool true=成功, false=失敗
 */
bool FBDF_Save_ReadScoreOneDif(FBDF_file_music_score_st *dest, const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type) {
    FBDF_file_music_score_st buf[3];
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

/**
 * @brief ユーザースコアデータを指定した難易度だけ書き込む。上書きするので注意
 * @param[in] src 保存するデータ
 * @param[in] music_folder_name 曲のフォルダ名
 * @param[in] dif_type 難易度タイプ
 * @return bool true=成功, false=失敗
 */
bool FBDF_Save_WriteScoreOneDif(const FBDF_file_music_score_st *src, const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type) {
    FBDF_file_music_score_st buf[3];
    FBDF_Save_ReadScoreAllDif(buf, music_folder_name); /* readできてなくても良い */
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

/**
 * @brief ユーザースコアデータを指定した難易度だけ書き込む。良い成績だけを書き込む
 * @param[in] src 保存するデータ
 * @param[in] music_folder_name 曲のフォルダ名
 * @param[in] dif_type 難易度タイプ
 * @return bool true=成功, false=失敗
 */
bool FBDF_Save_UpdateScoreOneDif(const FBDF_file_music_score_st *src, const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type) {
    FBDF_file_music_score_st buf;
    FBDF_Save_ReadScoreOneDif(&buf, music_folder_name, dif_type); /* readできてなくても良い */

    if (buf.acc        < src->acc       ) { buf.acc        = src->acc;        }
    if (buf.clear_type < src->clear_type) { buf.clear_type = src->clear_type; }
    if (buf.score      < src->score     ) { buf.score      = src->score;      }

    return FBDF_Save_WriteScoreOneDif(&buf, music_folder_name, dif_type);
}
