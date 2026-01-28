
#include <DxLib.h>

#include <sancur.h>

#include <system.h>

#define FBDF_PLAY_SCOREBAR_COLOR_RED60     0xFFEB3324 /* 70  - 60 */
#define FBDF_PLAY_SCOREBAR_COLOR_RED0      0xFF962117 /* 60  -  0 */
#define FBDF_PLAY_SCOREBAR_COLOR_YELLOW70  0xFFBDBB3F /* 70  - 80 */
#define FBDF_PLAY_SCOREBAR_COLOR_YELLOW80  0xFFEDEB4F /* 80  - 90 */
#define FBDF_PLAY_SCOREBAR_COLOR_GREEN90   0xFF5BC23C /* 90  - 95 */
#define FBDF_PLAY_SCOREBAR_COLOR_GREEN95   0xFF78FF4F /* 95  - 97 */
#define FBDF_PLAY_SCOREBAR_COLOR_BLUE97    0xFF3282F6 /* 97  - 98 */
#define FBDF_PLAY_SCOREBAR_COLOR_BLUE98    0xFF73FBFD /* 98  - 99 */
#define FBDF_PLAY_SCOREBAR_COLOR_PURPLE99  0xFFA349A4 /* 99  -100 */
#define FBDF_PLAY_SCOREBAR_COLOR_PURPLE100 0xFFEA3FF7 /* 99.5-100 */

#if 1 /* DrawScoreBar系 */

/**
 * @brief スコアバーを描く。右向き
 * @param[in] score_bar スコアバーのデータ
 * @param[in] pos_left  描画左位置
 * @param[in] pos_up    描画上位置
 * @param[in] pos_right 描画右位置
 * @param[in] pos_down  描画下位置
 * @return なし
 */
void FBDF_DrawScoreBarHori(const FBDF_score_bar_st &score_bar, int pos_left, int pos_up, int pos_right, int pos_down) {
    double drawRight = 167;
    /* 70-60 */
    drawRight = lins_scale(70.0, pos_left,  60.0, pos_right, score_bar.bar_70);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_RED60, TRUE);
    /* 60- 0 */
    drawRight = lins_scale(60.0, pos_left,   0.0, pos_right, score_bar.bar_70);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_RED0, TRUE);
    /* 70-80 */
    drawRight = lins_scale(70.0, pos_left,  80.0, pos_right, score_bar.bar_70);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_YELLOW70, TRUE);
    /* 80-90 */
    drawRight = lins_scale(80.0, pos_left,  90.0, pos_right, score_bar.bar_70);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_YELLOW80, TRUE);
    /* 90-95 */
    drawRight = lins_scale(90.0, pos_left,  95.0, pos_right, score_bar.bar_90);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_GREEN90, TRUE);
    /* 95-97 */
    drawRight = lins_scale(95.0, pos_left,  97.0, pos_right, score_bar.bar_90);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_GREEN95, TRUE);
    /* 97-98 */
    drawRight = lins_scale(97.0, pos_left,  98.0, pos_right, score_bar.bar_96);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_BLUE97, TRUE);
    /* 98-99 */
    drawRight = lins_scale(98.0, pos_left,  99.0, pos_right, score_bar.bar_96);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_BLUE98, TRUE);
    /* 99-100 */
    drawRight = lins_scale(99.0, pos_left, 100.0, pos_right, score_bar.bar_98);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_PURPLE99, TRUE);
    /* 99.5-100 */
    drawRight = lins_scale(99.5, pos_left, 100.0, pos_right, score_bar.bar_99);
    DrawBox(pos_left, pos_up, drawRight, pos_down, FBDF_PLAY_SCOREBAR_COLOR_PURPLE100, TRUE);
}

/**
 * @brief スコアバーを描く。上向き
 * @param[in] score_bar スコアバーのデータ
 * @param[in] pos_left  描画左位置
 * @param[in] pos_up    描画上位置
 * @param[in] pos_right 描画右位置
 * @param[in] pos_down  描画下位置
 * @return なし
 */
void FBDF_DrawScoreBarVert(const FBDF_score_bar_st &score_bar, int pos_left, int pos_up, int pos_right, int pos_down) {
    const int y_middle = (pos_down + pos_up) / 2;
    int drawUp = 167;
    /* 70-60 */
    drawUp = betweens(pos_up, lins(70.0, y_middle,  60.0, pos_up, score_bar.bar_70), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_RED60, TRUE);
    /* 60- 0 */
    drawUp = betweens(pos_up, lins(60.0, y_middle,   0.0, pos_up, score_bar.bar_70), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_RED0, TRUE);
    /* 70-80 */
    drawUp = betweens(pos_up, lins(70.0, y_middle,  80.0, pos_up, score_bar.bar_70), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_YELLOW70, TRUE);
    /* 80-90 */
    drawUp = betweens(pos_up, lins(80.0, y_middle,  90.0, pos_up, score_bar.bar_70), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_YELLOW80, TRUE);
    /* 90-95 */
    drawUp = betweens(pos_up, lins(90.0, y_middle,  95.0, pos_up, score_bar.bar_90), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_GREEN90, TRUE);
    /* 95-97 */
    drawUp = betweens(pos_up, lins(95.0, y_middle,  97.0, pos_up, score_bar.bar_90), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_GREEN95, TRUE);
    /* 97-98 */
    drawUp = betweens(pos_up, lins(97.0, y_middle,  98.0, pos_up, score_bar.bar_96), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_BLUE97, TRUE);
    /* 98-99 */
    drawUp = betweens(pos_up, lins(98.0, y_middle,  99.0, pos_up, score_bar.bar_96), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_BLUE98, TRUE);
    /* 99-100 */
    drawUp = betweens(pos_up, lins(99.0, y_middle, 100.0, pos_up, score_bar.bar_98), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_PURPLE99, TRUE);
    /* 99.5-100 */
    drawUp = betweens(pos_up, lins(99.5, y_middle, 100.0, pos_up, score_bar.bar_99), pos_down);
    DrawBox(pos_left, drawUp, pos_right, pos_down, FBDF_PLAY_SCOREBAR_COLOR_PURPLE100, TRUE);
}

/**
 * @brief スコアバーを描く。上向き。傾斜付き
 * @param[in] score_bar1 スコアバーのデータ1個目、左側
 * @param[in] score_bar2 スコアバーのデータ2個目、右側
 * @param[in] pos_left  描画左位置
 * @param[in] pos_up    描画上位置
 * @param[in] pos_right 描画右位置
 * @param[in] pos_down  描画下位置
 * @return なし
 */
void FBDF_DrawScoreBarVertQuad(const FBDF_score_bar_st &score_bar1, const FBDF_score_bar_st &score_bar2,
    int pos_left, int pos_up, int pos_right, int pos_down
) {
    const int y_middle = (pos_down + pos_up) / 2;
    int drawUp1 = 167;
    int drawUp2 = 167;
    /* 70-60 */
    drawUp1 = betweens(pos_up, lins(70.0, y_middle, 60.0, pos_up, score_bar1.bar_70), pos_down);
    drawUp2 = betweens(pos_up, lins(70.0, y_middle, 60.0, pos_up, score_bar2.bar_70), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_RED60, TRUE
    );
    /* 60- 0 */
    drawUp1 = betweens(pos_up, lins(60.0, y_middle, 0.0, pos_up, score_bar1.bar_70), pos_down);
    drawUp2 = betweens(pos_up, lins(60.0, y_middle, 0.0, pos_up, score_bar2.bar_70), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_RED0, TRUE
    );
    /* 70-80 */
    drawUp1 = betweens(pos_up, lins(70.0, y_middle, 80.0, pos_up, score_bar1.bar_70), pos_down);
    drawUp2 = betweens(pos_up, lins(70.0, y_middle, 80.0, pos_up, score_bar2.bar_70), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_YELLOW70, TRUE
    );
    /* 80-90 */
    drawUp1 = betweens(pos_up, lins(80.0, y_middle, 90.0, pos_up, score_bar1.bar_70), pos_down);
    drawUp2 = betweens(pos_up, lins(80.0, y_middle, 90.0, pos_up, score_bar2.bar_70), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_YELLOW80, TRUE
    );
    /* 90-95 */
    drawUp1 = betweens(pos_up, lins(90.0, y_middle, 95.0, pos_up, score_bar1.bar_90), pos_down);
    drawUp2 = betweens(pos_up, lins(90.0, y_middle, 95.0, pos_up, score_bar2.bar_90), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_GREEN90, TRUE
    );
    /* 95-97 */
    drawUp1 = betweens(pos_up, lins(95.0, y_middle, 97.0, pos_up, score_bar1.bar_90), pos_down);
    drawUp2 = betweens(pos_up, lins(95.0, y_middle, 97.0, pos_up, score_bar2.bar_90), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_GREEN95, TRUE
    );
    /* 97-98 */
    drawUp1 = betweens(pos_up, lins(97.0, y_middle, 98.0, pos_up, score_bar1.bar_96), pos_down);
    drawUp2 = betweens(pos_up, lins(97.0, y_middle, 98.0, pos_up, score_bar2.bar_96), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_BLUE97, TRUE
    );
    /* 98-99 */
    drawUp1 = betweens(pos_up, lins(98.0, y_middle, 99.0, pos_up, score_bar1.bar_96), pos_down);
    drawUp2 = betweens(pos_up, lins(98.0, y_middle, 99.0, pos_up, score_bar2.bar_96), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_BLUE98, TRUE
    );
    /* 99-100 */
    drawUp1 = betweens(pos_up, lins(99.0, y_middle, 100.0, pos_up, score_bar1.bar_98), pos_down);
    drawUp2 = betweens(pos_up, lins(99.0, y_middle, 100.0, pos_up, score_bar2.bar_98), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_PURPLE99, TRUE
    );
    /* 99.5-100 */
    drawUp1 = betweens(pos_up, lins(99.5, y_middle, 100.0, pos_up, score_bar1.bar_99), pos_down);
    drawUp2 = betweens(pos_up, lins(99.5, y_middle, 100.0, pos_up, score_bar2.bar_99), pos_down);
    DrawQuadrangle(
        pos_left, drawUp1,
        pos_right, drawUp2,
        pos_right, pos_down,
        pos_left, pos_down,
        FBDF_PLAY_SCOREBAR_COLOR_PURPLE100, TRUE
    );
}

#endif
