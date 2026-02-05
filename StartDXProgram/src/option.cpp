
#include <DxLib.h>

#include <dxcur.h>

#include <system.h>

void FBDF_Option_KeyAction(int &cmd) {
	InputAllKeyHold();
    switch (GetKeyPushOnce()) {
    case KEY_INPUT_RETURN: /* bool項目の切り替え */
        switch (cmd) {
        case 2:
            game_option.auto_en = !game_option.auto_en;
            break;
        case 6:
            game_option.hit_effect_en = !game_option.hit_effect_en;
            break;
        case 7:
            game_option.judge_draw_en = !game_option.judge_draw_en;
            break;
        case 8:
            game_option.fast_slow_en = !game_option.fast_slow_en;
            break;
        case 9:
            game_option.chain_draw_en = !game_option.chain_draw_en;
            break;
        default:
            break;
        }
        break;
    case KEY_INPUT_BACK: /* セレクトに戻る */
        break;
    case KEY_INPUT_UP: /* 項目選択 */
		cmd = MOD_AVOID_ZERO((cmd + 10 - 1), 10, 0);
        break;
    case KEY_INPUT_DOWN: /* 項目選択 */
		cmd = MOD_AVOID_ZERO((cmd + 1), 10, 0);
        break;
    case KEY_INPUT_LEFT: /* 項目操作 */
        switch (cmd) {
        case 0:
            game_option.chara = MOD_AVOID_ZERO((game_option.chara + 4 - 1), 4, 0);
            break;
        case 1:
            game_option.play_style = MOD_AVOID_ZERO((game_option.play_style + 3 - 1), 3, 0);
            break;
        case 2:
            game_option.auto_en = !game_option.auto_en;
            break;
        case 3:
            game_option.lane_speed = max(1, game_option.lane_speed);
            break;
        case 4:
            game_option.note_offset_timing--;
            break;
        case 5:
            game_option.note_offset_draw--;
            break;
        case 6:
            game_option.hit_effect_en = !game_option.hit_effect_en;
            break;
        case 7:
            game_option.judge_draw_en = !game_option.judge_draw_en;
            break;
        case 8:
            game_option.fast_slow_en = !game_option.fast_slow_en;
            break;
        case 9:
            game_option.chain_draw_en = !game_option.chain_draw_en;
            break;
        default:
            break;
        }
        break;
    case KEY_INPUT_RIGHT: /* 項目操作 */
        switch (cmd) {
        case 0:
            game_option.chara = MOD_AVOID_ZERO((game_option.chara + 1), 4, 0);
            break;
        case 1:
            game_option.play_style = MOD_AVOID_ZERO((game_option.play_style + 1), 3, 0);
            break;
        case 2:
            game_option.auto_en = !game_option.auto_en;
            break;
        case 3:
            game_option.lane_speed = min(game_option.lane_speed, 100);
            break;
        case 4:
            game_option.note_offset_timing++;
            break;
        case 5:
            game_option.note_offset_draw++;
            break;
        case 6:
            game_option.hit_effect_en = !game_option.hit_effect_en;
            break;
        case 7:
            game_option.judge_draw_en = !game_option.judge_draw_en;
            break;
        case 8:
            game_option.fast_slow_en = !game_option.fast_slow_en;
            break;
        case 9:
            game_option.chain_draw_en = !game_option.chain_draw_en;
            break;
        default:
            break;
        }
        break;
    }
}

void FBDF_Option_Draw(void) {
    DrawString(5, 5, _T("OPTION NOW"), COLOR_WHITE);
}
