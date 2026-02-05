
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* own include */
#include <fbdf_cutin.h>

/**
 * @brief カットイン描画
 * @param なし
 * @return なし
 */
void FBDF_cutin_c::DrawCut() const {
	if (this->sequence == 0) { return; }
	int Ntime = GetNowCount();
	int EffTime = Ntime - this->cutStime;
	int Xoffset = lins(0, 600, 1000, 0, this->sequence);
	DrawExtendGraph(-Xoffset, 0, -Xoffset + this->x_window_size, this->y_window_size, this->leftpic.handle() , TRUE);
	DrawExtendGraph( Xoffset, 0,  Xoffset + this->x_window_size, this->y_window_size, this->rightpic.handle(), TRUE);
	return;
}

/**
 * @brief イン/アウトのセット
 * @param[in] val 1でイン、0でアウト
 * @return なし
 */
void FBDF_cutin_c::SetIo(int val) {
	this->cutIoFg = val;
	this->cutStime = GetNowCount();
	if (this->cutIoFg == CUT_FRAG_IN) {
		this->acs_sound = true;
	}
	return;
}

/**
 * @brief ウィンドウサイズの情報をクラス内に保存する(コンストラクタでやるべき?)
 * @param[in] x ウィンドウの横サイズ
 * @param[in] y ウィンドウの縦サイズ
 * @return なし
 */
void FBDF_cutin_c::SetWindowSize(int x, int y) {
	this->x_window_size = x;
	this->y_window_size = y;
	return;
}

/**
 * @brief 情報の更新、少なくとも描画前に呼んで
 * @param なし
 * @return なし
 */
void FBDF_cutin_c::update() {
	if (this->cutIoFg == CUT_FRAG_IN) {
		this->sequence = lins_scale(0, 0, 500, 1000, GetNowCount() - this->cutStime);
	}
	else {
		this->sequence = lins_scale(0, 1000, 500, 0, GetNowCount() - this->cutStime);
	}
	if (this->acs_sound && (this->cutIoFg == CUT_FRAG_IN) && (this->sequence == 1000)) {
		PlaySoundMem(this->sound.handle(), DX_PLAYTYPE_BACK);
		this->acs_sound = false;
	}
	return;
}

/**
 * @brief カットインであるかどうかを返す
 * @param なし
 * @return int 1=カットインである, 0=カットアウトである
 */
int FBDF_cutin_c::IsClosing() const {
	return this->cutIoFg;
}

/**
 * @brief カットインのアニメが終わったかどうかを返す
 * @param なし
 * @return bool true=カットインであり、アニメも終わった, false=アニメが終わってないか、カットアウトである
 */
bool FBDF_cutin_c::IsEndAnim() const {
	return (this->cutIoFg == CUT_FRAG_IN && this->cutStime + 2000 <= GetNowCount());
}
