
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* own include */
#include <fbdf_cutin.h>

/**
 * @brief コンストラクタ、画像と音声を用意している
 * @param なし
 */
fbdf_cutin_c::fbdf_cutin_c() {
	this->leftpic.reload(_T("pic/cutinLeft.png"));
	this->rightpic.reload(_T("pic/cutinRight.png"));
	this->sound = LoadSoundMem(_T("SE/cutin.wav"));
}

/**
 * @brief デストラクタ、音声を破棄している(画像はdxcur_pic_c内で破棄するから大丈夫)
 */
fbdf_cutin_c::~fbdf_cutin_c() {
	DeleteSoundMem(this->sound);
}

/**
 * @brief カットイン描画
 * @param なし
 * @return なし
 */
void fbdf_cutin_c::DrawCut() const {
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
void fbdf_cutin_c::SetIo(int val) {
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
void fbdf_cutin_c::SetWindowSize(int x, int y) {
	this->x_window_size = x;
	this->y_window_size = y;
	return;
}

/**
 * @brief 情報の更新、少なくとも描画前に呼んで
 * @param なし
 * @return なし
 */
void fbdf_cutin_c::update() {
	if (this->cutIoFg == CUT_FRAG_IN) {
		this->sequence = lins_scale(0, 0, 500, 1000, GetNowCount() - this->cutStime);
	}
	else {
		this->sequence = lins_scale(0, 1000, 500, 0, GetNowCount() - this->cutStime);
	}
	if (this->acs_sound && (this->cutIoFg == CUT_FRAG_IN) && (this->sequence == 1000)) {
		PlaySoundMem(this->sound, DX_PLAYTYPE_BACK);
		this->acs_sound = false;
	}
	return;
}

/**
 * @brief カットインであるかどうかを返す
 * @param なし
 * @return int 1=カットインである, 0=カットアウトである
 */
int fbdf_cutin_c::IsClosing() const {
	return this->cutIoFg;
}

/**
 * @brief カットインのアニメが終わったかどうかを返す
 * @param なし
 * @return bool true=カットインであり、アニメも終わった, false=アニメが終わってないか、カットアウトである
 */
bool fbdf_cutin_c::IsEndAnim() const {
	return (this->cutIoFg == CUT_FRAG_IN && this->cutStime + 2000 <= GetNowCount());
}
