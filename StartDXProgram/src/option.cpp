
#include <string>
#include <DxLib.h>
#include <dxcur.h>
#include <main.h>
#include <system.h>
#include <option.h>

typedef enum FBDF_param_type_e {
    FBDF_PARAM_TYPE_BOOL,
    FBDF_PARAM_TYPE_UINT,
    FBDF_PARAM_TYPE_INT
} FBDF_param_type_et;

#if 1 /* class */

#if 1 /* 項目系 */

/* 継承前提、テンプレートは実装できなかった */
class FBDF_option_item_base_c {
protected:
    FBDF_param_type_et param_type = FBDF_PARAM_TYPE_BOOL;
    bool is_loop_param = false; /* is_bool_paramがtrueだったら無効 */
    int  lower_limit   = 0;     /* is_bool_paramがtrueだったら無効、is_loop_paramがtrueだったら0固定 */
    int  upper_limit   = 0;     /* is_bool_paramがtrueだったら無効 */
    void *option_p     = &game_option.empty_val;
    dxcur_pic_c base_pic = dxcur_pic_c();

public:
    std::string item_name = "";
    std::string item_detail = "";

    void CmdUp(void) {
        switch (this->param_type) {
        case FBDF_PARAM_TYPE_BOOL:
            *(bool *)(this->option_p) = !(*(bool *)(this->option_p));
            break;
        case FBDF_PARAM_TYPE_UINT:
            if (this->is_loop_param) {
                *(uint *)(this->option_p) = LOOP_ADD(*(uint *)(this->option_p), this->upper_limit + 1);
            }
            else {
                *(uint *)(this->option_p) = min(*(uint *)(this->option_p) + 1, this->upper_limit);
            }
            break;
        case FBDF_PARAM_TYPE_INT:
            if (this->is_loop_param) {
                *(int *)(this->option_p) = LOOP_ADD(*(int *)(this->option_p), this->upper_limit + 1);
            }
            else {
                *(int *)(this->option_p) = min(*(int *)(this->option_p) + 1, this->upper_limit);
            }
            break;
        }
    }

    void CmdDown(void) {
        switch (this->param_type) {
        case FBDF_PARAM_TYPE_BOOL:
            *(bool *)(this->option_p) = !(*(bool *)(this->option_p));
            break;
        case FBDF_PARAM_TYPE_UINT:
            if (this->is_loop_param) {
                *(uint *)(this->option_p) = LOOP_SUB(*(uint *)(this->option_p), this->upper_limit + 1);
            }
            else {
                *(uint *)(this->option_p) = max(this->lower_limit, *(uint *)(this->option_p) - 1);
            }
            break;
        case FBDF_PARAM_TYPE_INT:
            if (this->is_loop_param) {
                *(int *)(this->option_p) = LOOP_SUB(*(int *)(this->option_p), this->upper_limit + 1);
            }
            else {
                *(int *)(this->option_p) = max(this->lower_limit, *(int *)(this->option_p) - 1);
            }
            break;
        }
    }

    void ChangeBool(void) {
        if (this->param_type == FBDF_PARAM_TYPE_BOOL) {
            this->CmdUp();
        }
    }

    virtual std::string GetParamName(void) const {
        std::string s = "";
        return s;
    }

    virtual std::string GetParamDetail(void) const {
        std::string s = "";
        return s;
    }

    virtual DxPic_t GetPicHandle(void) const {
        return this->base_pic.handle();
    }

    virtual void ReloadPic(void) {}
};

class FBDF_option_item_chara_c : public FBDF_option_item_base_c {
private:
    dxcur_pic_c pic[5];

public:
    FBDF_option_item_chara_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_UINT;
        this->is_loop_param = true;
        this->upper_limit   = 3;
        this->option_p      = &game_option.chara;
        this->item_name     = "ダンサー";
        this->item_detail   = "使用するダンサーを選びます。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(uint *)(this->option_p)) {
        case FBDF_DANCER_UNIOW:
            s = LANGUAGE_CHOOSE("ユニオ", "uniow");
            break;
        case FBDF_DANCER_NEIDA:
            s = LANGUAGE_CHOOSE("ニーダ", "neida");
            break;
        case FBDF_DANCER_TRIMBA:
            s = LANGUAGE_CHOOSE("トリンバ", "trimba");
            break;
        case FBDF_DANCER_QUATTRO:
            s = LANGUAGE_CHOOSE("クアトロ", "quattro");
            break;
        }
        return s;
    }

    DxPic_t GetPicHandle(void) const override {
        return this->pic[*(uint *)(this->option_p)].handle();
    }

    void ReloadPic(void) override {
        this->pic[0].reload(_T(""));
        this->pic[1].reload(_T(""));
        this->pic[2].reload(_T(""));
        this->pic[3].reload(_T(""));
        this->pic[4].reload(_T(""));
    }
};

class FBDF_option_item_playstyle_c : public FBDF_option_item_base_c {
private:
    dxcur_pic_c pic[3];

public:
    FBDF_option_item_playstyle_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_UINT;
        this->is_loop_param = true;
        this->upper_limit   = 4;
        this->option_p      = &game_option.play_style;
        this->item_name     = "プレイスタイル";
        this->item_detail   = "プレイスタイルを選びます。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(uint *)(this->option_p)) {
        case FBDF_PLAYSTYLE_ASSIST_PLUS:
            s = "assist+";
            break;
        case FBDF_PLAYSTYLE_ASSIST:
            s = "assist";
            break;
        case FBDF_PLAYSTYLE_NORMAL:
            s = "normal";
            break;
        case FBDF_PLAYSTYLE_BLANC:
            s = "blanc";
            break;
        case FBDF_PLAYSTYLE_BLANC_PLUS:
            s = "blanc+";
            break;
        }
        return s;
    }

    std::string GetParamDetail(void) const override {
        std::string s = "";
        switch (*(uint *)(this->option_p)) {
        case FBDF_PLAYSTYLE_ASSIST_PLUS:
            s = "常にボタン位置のアシストが付きます。\nその代わり、得られるスコアがかなり減り、accスコアが保存されません。";
            break;
        case FBDF_PLAYSTYLE_ASSIST:
            s = "ミスをすると一定時間ボタン位置のアシストが付きます。\nその代わり、得られるスコアが減り、accスコアが保存されません。";
            break;
        case FBDF_PLAYSTYLE_NORMAL:
            s = "通常のモードです。";
            break;
        case FBDF_PLAYSTYLE_BLANC:
            s = "色が分からなくなりますが、その代わり得られるスコアがほんの僅かだけ増えます。\nミスをすると一定時間色が復活します。";
            break;
        case FBDF_PLAYSTYLE_BLANC_PLUS:
            s = "色が分からなくなりますが、その代わり得られるスコアがちょっとだけ増えます。\nミスをしても色は復活しません。スコアは別で保存されます。";
            break;
        }
        return s;
    }

    DxPic_t GetPicHandle(void) const override {
        return this->pic[*(uint *)(this->option_p)].handle();
    }

    void ReloadPic(void) override {
        this->pic[0].reload(_T(""));
        this->pic[1].reload(_T(""));
        this->pic[2].reload(_T(""));
    }
};

class FBDF_option_item_autoen_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_autoen_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_BOOL;
        this->option_p      = &game_option.auto_en;
        this->item_name     = "オート";
        this->item_detail   = "自動演奏モードの切り替えをします。\n有効にすると記録は保存されません。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(bool *)(this->option_p)) {
        case false:
            s = "OFF";
            break;
        case true:
            s = "ON";
            break;
        }
        return s;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_lanespeed_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_lanespeed_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_UINT;
        this->is_loop_param = false;
        this->lower_limit   = 1;
        this->upper_limit   = 100;
        this->option_p      = &game_option.lane_speed;
        this->item_name     = "ノーツ速度";
        this->item_detail   = "ノーツの速度を変更します。増やすと速くなります。";
    }

    std::string GetParamName(void) const override {
        std::string s;
        return std::to_string((int)(*(uint *)(this->option_p) / 10)) + '.' + std::to_string(*(uint *)(this->option_p) % 10);
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_noteoffset_time_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_noteoffset_time_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_INT;
        this->is_loop_param = false;
        this->lower_limit   = -10000;
        this->upper_limit   =  10000;
        this->option_p      = &game_option.note_offset_timing;
        this->item_name     = "ノーツタイミング";
        this->item_detail   = "ノーツが来るタイミングをずらします。\nfast多いなら減らして、slow多いなら増やしてください。";
    }

    std::string GetParamName(void) const override {
        std::string ret = "";
        ret += (0 < *(int *)(this->option_p)) ? ("+") : ("");
        ret += std::to_string(*(int *)(this->option_p));
        ret += "ms";
        return ret;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_noteoffset_draw_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_noteoffset_draw_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_INT;
        this->is_loop_param = false;
        this->lower_limit   = -10000;
        this->upper_limit   =  10000;
        this->option_p      = &game_option.note_offset_draw;
        this->item_name     = "ノーツ描画位置";
        this->item_detail   = "ノーツを描く位置をずらします。\n増やすと上に、減らすと下にずれます。";
    }

    std::string GetParamName(void) const override {
        std::string ret = "";
        ret += (0 < *(int *)(this->option_p)) ? ("+") : ("");
        ret += std::to_string(*(int *)(this->option_p));
        ret += "ms";
        return ret;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_hiteffecten_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_hiteffecten_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_BOOL;
        this->option_p      = &game_option.hit_effect_en;
        this->item_name     = "ノーツエフェクト";
        this->item_detail   = "ノーツを叩いた時のエフェクトの表示切替をします。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(bool *)(this->option_p)) {
        case false:
            s = "OFF";
            break;
        case true:
            s = "ON";
            break;
        }
        return s;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_judgedrawen_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_judgedrawen_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_BOOL;
        this->option_p      = &game_option.judge_draw_en;
        this->item_name     = "判定表示";
        this->item_detail   = "ノーツの判定表示を切り替えます。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(bool *)(this->option_p)) {
        case false:
            s = "OFF";
            break;
        case true:
            s = "ON";
            break;
        }
        return s;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_fastslowen_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_fastslowen_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_BOOL;
        this->option_p      = &game_option.fast_slow_en;
        this->item_name     = "判定詳細表示";
        this->item_detail   = "ノーツを叩いたタイミングの詳細判定の表示を切り替えます。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(bool *)(this->option_p)) {
        case false:
            s = "OFF";
            break;
        case true:
            s = "ON";
            break;
        }
        return s;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_chaindrawen_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_chaindrawen_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_BOOL;
        this->option_p      = &game_option.chain_draw_en;
        this->item_name     = "チェイン数表示";
        this->item_detail   = "チェイン数の表示を切り替えます。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(bool *)(this->option_p)) {
        case false:
            s = "OFF";
            break;
        case true:
            s = "ON";
            break;
        }
        return s;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

class FBDF_option_item_language_c : public FBDF_option_item_base_c {
public:
    FBDF_option_item_language_c (void) {
        this->param_type    = FBDF_PARAM_TYPE_UINT;
        this->is_loop_param = true;
        this->upper_limit   = 1;
        this->option_p      = &game_option.language;
        this->item_name     = "言語";
        this->item_detail   = "ゲーム内で使用する言語を選択します。";
    }

    std::string GetParamName(void) const override {
        std::string s = "";
        switch (*(uint *)(this->option_p)) {
        case 0:
            s = "Japanese";
            break;
        case 1:
            s = "English";
            break;
        }
        return s;
    }

    std::string GetParamDetail(void) const override {
        std::string s = "";
        switch (*(uint *)(this->option_p)) {
        case 0:
            s = "";
            break;
        case 1:
            s = "※すべてに対応していない可能性があります。";
            break;
        }
        return s;
    }

    void ReloadPic(void) override {
        this->base_pic.reload(_T(""));
    }
};

#endif /* 項目系 */

#endif /* class */

static std::vector<FBDF_option_item_base_c *> s_op_list = {
    new FBDF_option_item_chara_c(),
    new FBDF_option_item_playstyle_c(),
    new FBDF_option_item_autoen_c(),
    new FBDF_option_item_lanespeed_c(),
    new FBDF_option_item_noteoffset_time_c(),
    new FBDF_option_item_noteoffset_draw_c(),
    new FBDF_option_item_hiteffecten_c(),
    new FBDF_option_item_judgedrawen_c(),
    new FBDF_option_item_fastslowen_c(),
    new FBDF_option_item_chaindrawen_c(),
    new FBDF_option_item_language_c()
};

static void FBDF_Option_DrawItemOne(int no, int x, int y, const FBDF_option_pic_st &pic) {
    DrawGraph(       x,     y,      pic.box.handle(), TRUE);
    DrawFormatString(x + 9, y + 10, COLOR_WHITE, _T("%s"), s_op_list.at(no)->item_name.c_str());
    DrawFormatString(x + 9, y + 37, COLOR_WHITE, _T("%s"), s_op_list.at(no)->GetParamName().c_str());
}

/**
 * @brief オプションで使う画像のリロード。必ず1回は行うこと。
 * @param なし
 * @return なし
 */
void FBDF_Option_ReloadPic(void) {
    for (size_t i = 0; i < s_op_list.size(); i++) {
        s_op_list.at(i)->ReloadPic();
    }
}

void FBDF_Option_KeyAction(dxcur_key_c &key, int &cmd, bool &option_fg, DxSnd_t se_handle) {
    key.update();
    switch (key.GetKeyPulseOnce()) {
    case KEY_INPUT_Z:
    case KEY_INPUT_BACK: /* セレクトに戻る */
        option_fg = false;
        break;
    case KEY_INPUT_UP:   /* 項目選択 */
		cmd = LOOP_SUB(cmd, s_op_list.size());
        PlaySoundMem(se_handle, DX_PLAYTYPE_BACK);
        break;
    case KEY_INPUT_DOWN: /* 項目選択 */
		cmd = LOOP_ADD(cmd, s_op_list.size());
        PlaySoundMem(se_handle, DX_PLAYTYPE_BACK);
        break;
    case KEY_INPUT_LEFT: /* 項目操作 */
        s_op_list.at(cmd)->CmdDown();
        PlaySoundMem(se_handle, DX_PLAYTYPE_BACK);
        break;
    case KEY_INPUT_RIGHT: /* 項目操作 */
        s_op_list.at(cmd)->CmdUp();
        PlaySoundMem(se_handle, DX_PLAYTYPE_BACK);
        break;
    }
}

/**
 * @brief オプション画面を描く。セレクト画面の上に直書きするイメージ。
 * @param[in] cmd コマンド
 */
void FBDF_Option_Draw(int cmd, const FBDF_option_pic_st &pic, const FBDF_usage_c &usage) {
    int DrawY = (WINDOW_SIZE_Y - 120) / 2 - 32;
    int DrawC = cmd;
    /* 後ろの選曲画面を暗くする */
    DrawGraph(0, 0, pic.back.handle(), TRUE);
    /* 項目を描く */
    DrawC = cmd;
    DrawY = (WINDOW_SIZE_Y - 120) / 2 - 32;
    for (size_t i = 0; i < 5; i++) {
        DrawC = LOOP_SUB(DrawC, s_op_list.size());
        DrawY -= 80;
        FBDF_Option_DrawItemOne(DrawC, 15, DrawY, pic);
    }
    FBDF_Option_DrawItemOne(cmd, 15 + 50, (WINDOW_SIZE_Y - 120) / 2 - 32, pic);
    DrawC = cmd;
    DrawY = (WINDOW_SIZE_Y - 120) / 2 - 32;
    for (size_t i = 0; i < 5; i++) {
        DrawC = LOOP_ADD(DrawC, s_op_list.size());
        DrawY += 80;
        FBDF_Option_DrawItemOne(DrawC, 15, DrawY, pic);
    }
    /* 説明の画像を描く */
    DrawGraph(0, 0, s_op_list.at(cmd)->GetPicHandle(), TRUE);
    /* 操作説明を描く */
    usage.draw(0, WINDOW_SIZE_Y - 70);
    /* 詳細を描く */
    DrawExtendGraph( 0, WINDOW_SIZE_Y - 75, WINDOW_SIZE_X, WINDOW_SIZE_Y, pic.detail.handle(), TRUE);
    DrawFormatString(5, WINDOW_SIZE_Y - 65, COLOR_WHITE, _T("%s"), s_op_list.at(cmd)->item_detail.c_str());
    DrawFormatString(5, WINDOW_SIZE_Y - 40, COLOR_WHITE, _T("%s"), s_op_list.at(cmd)->GetParamDetail().c_str());
}
