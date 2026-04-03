
#include <string>
#include <DxLib.h>
#include <sancur.h>
#include <strcur.h>
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

FBDF_game_option_st game_option; /* ゲームオプション、option.cpp以外で変更されない、他の場所では読み込み専用 */

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

FBDF_usage_c::FBDF_usage_c(void) {}
FBDF_usage_c::FBDF_usage_c(const char *str) : usage(str) {}

#if 1 /* FBDF_cascadia_pic_c */

/* x座標は勝手に進むので注意 */
void FBDF_cascadia_pic_c::DrawNumOnce(int &x, int y, char num, double size) const {
    num = betweens('0', num, '9');
    DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
        this->pic.handle(num - '0'), TRUE);
    x += this->picsizeX * size;
}

/* x座標は勝手に進むので注意 */
void FBDF_cascadia_pic_c::DrawPoint(int &x, int y, double size) const {
    DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
        this->pic.handle(10), TRUE);
    x += this->pointsizeX * size;
}

/* x座標は勝手に進むので注意 */
void FBDF_cascadia_pic_c::DrawPlus(int &x, int y, double size) const {
    DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
        this->pic.handle(11), TRUE);
    x += this->picsizeX * size;
}

/* x座標は勝手に進むので注意 */
void FBDF_cascadia_pic_c::DrawMinus(int &x, int y, double size) const {
    DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
        this->pic.handle(12), TRUE);
    x += this->picsizeX * size;
}

uint FBDF_cascadia_pic_c::GetPicSize(uint num, double size) const {
    uint ret = 0;
    if (num == 0) { return this->picsizeX * size; }
    while (num != 0) {
        ret += this->picsizeX;
        num /= 10;
    }
    return (uint)(ret * size);
}

void FBDF_cascadia_pic_c::DrawNum(int x, int y, double size, int num, bool sign) const {
    char buf[8];
    int DrawX = x;
    int DrawY = y;
    if (num < 0) {
        this->DrawMinus(DrawX, DrawY, size);
    }
    else if (sign){
        this->DrawPlus(DrawX, DrawY, size);
    }
    strnums(buf, num, 8);
    for (size_t i = 0; buf[i] != '\0'; i++) {
        this->DrawNumOnce(DrawX, DrawY, buf[i], size);
    }
}

void FBDF_cascadia_pic_c::DrawNumRight(int right, int up, double size, uint num, bool sign) const {
    char buf[8];
    int DrawX = right - this->GetPicSize(num, size);
    this->DrawNum(DrawX, up, size, num, sign);
}

void FBDF_cascadia_pic_c::DrawFloat(int x, int y, double size, double num, uint under, bool sign) const {
    char buf[12];
    int DrawX = x;
    int DrawY = y;
    if (num < 0) {
        this->DrawMinus(DrawX, DrawY, size);
    }
    else if (sign){
        this->DrawPlus(DrawX, DrawY, size);
    }
    strnumsD(buf, num, 12, under);
    for (size_t i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == '.') {
            this->DrawPoint(DrawX, DrawY, size);
        }
        else {
            this->DrawNumOnce(DrawX, DrawY, buf[i], size);
        }
    }
}

#endif /* FBDF_cascadia_pic_c */

void FBDF_usage_c::draw(int left, int down) const {
    DrawGraph(left, down - 48, this->pic.handle(), TRUE);
    DrawFormatString(left + 5, down - 40, COLOR_WHITE, _T("%s"), this->usage.c_str());
}

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

std::string FBDF_ClearTypeToString(FBDF_clear_type_et type) {
    std::string ret = "";
    switch (type) {
    case FBDF_CLEAR_TYPE_NOPLAY:
        ret = "NO PLAY";
        break;
    case FBDF_CLEAR_TYPE_FAILED:
        ret = "FAILED";
        break;
    case FBDF_CLEAR_TYPE_ASSIST:
        ret = "ASSIST";
        break;
    case FBDF_CLEAR_TYPE_CLEARED:
        ret = "CLEARED";
        break;
    case FBDF_CLEAR_TYPE_CAKEWALK:
        ret = "CAKEWALK";
        break;
    case FBDF_CLEAR_TYPE_MISSLESS:
        ret = "MISS LESS";
        break;
    case FBDF_CLEAR_TYPE_FULLCOMBO:
        ret = "FULL COMBO";
        break;
    case FBDF_CLEAR_TYPE_PERFECT:
        ret = "PERFECT";
        break;
    }
    return ret;
}
