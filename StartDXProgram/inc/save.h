#pragma once

/**
 * 読み込んだデータ数 -> LOG_INFO
 * モーションが不足 -> LOG_WARN
 * 読み込んだファイルに変な設定値とかがある -> LOG_ERROR
 * ファイルが開けない -> LOG_ERROR
 * スコアのセーブに失敗 -> LOG_ALERT
 * DxLib_Init()に失敗 -> LOG_EMERG
 */

#include <tchar.h>
#include <system.h>

#if 1 /* define */

#define FBDF_LOG_LEVEL_DEF 4 /* 通常4(ERROR以上を出す) */

#if (FBDF_LOG_LEVEL_DEF <= 0)
#define FBDF_LOG_DEBUG(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_DEBUG, mes)
#else
#define FBDF_LOG_DEBUG(mes)
#endif

#if (FBDF_LOG_LEVEL_DEF <= 1)
#define FBDF_LOG_INFO(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_INFO, mes)
#else
#define FBDF_LOG_INFO(mes)
#endif

#if (FBDF_LOG_LEVEL_DEF <= 2)
#define FBDF_LOG_NOTICE(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_NOTICE, mes)
#else
#define FBDF_LOG_NOTICE(mes)
#endif

#if (FBDF_LOG_LEVEL_DEF <= 3)
#define FBDF_LOG_WARN(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_WARN, mes)
#else
#define FBDF_LOG_WARN(mes)
#endif

#if (FBDF_LOG_LEVEL_DEF <= 4)
#define FBDF_LOG_ERROR(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_ERROR, mes)
#else
#define FBDF_LOG_ERROR(mes)
#endif

#if (FBDF_LOG_LEVEL_DEF <= 5)
#define FBDF_LOG_CRIT(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_CRIT, mes)
#else
#define FBDF_LOG_CRIT(mes)
#endif

#if (FBDF_LOG_LEVEL_DEF <= 6)
#define FBDF_LOG_ALERT(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_ALERT, mes)
#else
#define FBDF_LOG_ALERT(mes)
#endif

#define FBDF_LOG_EMERG(mes)  FBDF_ErrorLogWrite(FBDF_LOG_LEVEL_EMERG, mes)

#endif /* define */

typedef enum FBDF_log_level_e {
	FBDF_LOG_LEVEL_DEBUG,  /* デバッグ用の詳細情報 */
	FBDF_LOG_LEVEL_INFO,   /* 情報メッセージ */
	FBDF_LOG_LEVEL_NOTICE, /* 正常だけど重要なイベント */
	FBDF_LOG_LEVEL_WARN,   /* エラーじゃないけど注意がいる */
	FBDF_LOG_LEVEL_ERROR,  /* エラーだけどすぐに直さなくて良い */
	FBDF_LOG_LEVEL_CRIT,   /* 致命的な問題 */
	FBDF_LOG_LEVEL_ALERT,  /* すぐに直す必要がある問題 */
	FBDF_LOG_LEVEL_EMERG   /* ゲームが継続不可になった問題 */
} FBDF_log_level_et;

typedef struct FBDF_file_music_score_s {
	int score = 0;
	double acc = 0.0;
	FBDF_clear_type_et clear_type = FBDF_CLEAR_TYPE_NOPLAY;
} FBDF_file_music_score_st;

extern bool FBDF_Save_ReadScoreAllDif(        FBDF_file_music_score_st  dest[], const TCHAR *music_folder_name);
extern bool FBDF_Save_WriteScoreAllDif( const FBDF_file_music_score_st  src[],  const TCHAR *music_folder_name);
extern bool FBDF_Save_ReadScoreOneDif(        FBDF_file_music_score_st &dest,   const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);
extern bool FBDF_Save_WriteScoreOneDif( const FBDF_file_music_score_st &src,    const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);
extern bool FBDF_Save_UpdateScoreOneDif(const FBDF_file_music_score_st &src,    const TCHAR *music_folder_name, FBDF_dif_type_ec dif_type);

extern bool FBDF_Save_ReadOption(       FBDF_game_option_st *dest);
extern bool FBDF_Save_WriteOption(const FBDF_game_option_st *src);

extern bool FBDF_ErrorLogWrite(FBDF_log_level_et level, const char *message);
