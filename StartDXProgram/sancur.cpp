/**
 * @file sancur.cpp
 * @brief 数値計算で便利なものを集めたコード
 * @author curbine
 * @date 2024/7/19
 * @note 個人が趣味で製作したものなので、品質は保証しません。このコードを使用したことによるトラブルの責任は負いかねます。
 */

#include "sancur.h"

/**
 * 点1(x1,y1)、点2(x2,y2)、を通る直線を考えて、xの時の値を返す。
 * @param[in] x1 点1のx座標
 * @param[in] y1 点1のy座標
 * @param[in] x2 点2のx座標
 * @param[in] y2 点2のy座標
 * @param[in] x 評価する値
 * @return double 点1(x1,y1)、点2(x2,y2)、を通る直線を考えた時のxの値
 */
double lins(double x1, double y1, double x2, double y2, double x) {
	if (x1 == x2) { return y1; }
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}

/**
 * lins関数のxの値をx1~x2の範囲内に収める版
 * @param[in] x1 点1のx座標
 * @param[in] y1 点1のy座標
 * @param[in] x2 点2のx座標
 * @param[in] y2 点2のy座標
 * @param[in] x 評価する値
 * @return double 点1(x1,y1)、点2(x2,y2)、を通る直線を考えた時のxの値
 */
double lins_scale(double x1, double y1, double x2, double y2, double x) {
	if (x1 == x2) { return y1; }
	if (x1 <= x2) {
		x = betweens_D(x1, x, x2);
	}
	else {
		x = betweens_D(x2, x, x1);
	}
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}

/**
 * 頂点(x1,y1)、通過点(x2,y2)、を通る二次関数を考えて、xの時の値を返す。
 * @param[in] x1 頂点のx座標
 * @param[in] y1 頂点のy座標
 * @param[in] x2 通過点のx座標
 * @param[in] y2 通過点のy座標
 * @param[in] x 評価する値
 * @return double 頂点(x1,y1)、通過点(x2,y2)を通る二次関数を考えた時のxの値
 */
double pals(double x1, double y1, double x2, double y2, double x) {
	double s1 = 0;
	double s2 = 0;
	if (x1 == x2) { return y1; }
	s1 = x - x1;
	s2 = x2 - x1;
	return (y2 - y1) * s1 * s1 / (s2 * s2) + y1;
}

/**
 * pals関数のxの値をx1~x2の範囲内に収める版
 * @param[in] x1 頂点のx座標
 * @param[in] y1 頂点のy座標
 * @param[in] x2 通過点のx座標
 * @param[in] y2 通過点のy座標
 * @param[in] x 評価する値
 * @return double 頂点(x1,y1)、通過点(x2,y2)を通る二次関数を考えた時のxの値
 */
double pals_scale(double x1, double y1, double x2, double y2, double x) {
	double s1 = 0;
	double s2 = 0;
	if (x1 == x2) { return y1; }
	if (x1 <= x2) {
		x = betweens_D(x1, x, x2);
	}
	else {
		x = betweens_D(x2, x, x1);
	}
	s1 = x - x1;
	s2 = x2 - x1;
	return (y2 - y1) * s1 * s1 / (s2 * s2) + y1;
}

/**
 * mode(1:lin,2:acc,3:dec)に合わせた計算をする。
 * @param[in] mode 線の種類
 * @param[in] x1 始点のx座標
 * @param[in] y1 始点のy座標
 * @param[in] x2 終点のx座標
 * @param[in] y2 終点のy座標
 * @param[in] x 評価する値
 * @return double 始点(x1,y1)、終点(x2,y2)をmode法で通る線を考えた時のxの値
 */
double movecal(int mode, double startx, double starty, double endx, double endy, double nowx) {
	switch (mode) {
	case 1:
		return lins(startx, starty, endx, endy, nowx);
	case 2:
		return pals(startx, starty, endx, endy, nowx);
	case 3:
		return pals(endx, endy, startx, starty, nowx);
	default:
		return 0;
	}
}

/**
 * ルートcを返す
 * @param[in] c 評価する値
 * @return double ルート(c)
 */
double sanrute(double c) {
	double a = 0;
	double b = 1;

	while (b * b < c) { b *= 10; }
	if (b * b == c) { return b; }
	b /= 10;
	for (uint i = 0; i < 10; i++) {
		while (a * a < c) { a += b; }
		if (a * a == c) { return a; }
		a -= b;
		b /= 10;
	}
	return a;
}

/**
 * aとbの差の絶対値を求める
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return int |a-b|
 */
int abss(int a, int b) {
	a -= b;
	if (a < 0) { a *= -1; }
	return a;
}

/**
 * aを0からbでループさせる
 * @param[in] a 評価する値
 * @param[in] b 最大値
 * @return int a % (b + 1)
 * @note この関数は使う必要なし。演算子で頑張ってください。
 */
int NumLoop(int a, int b) {
	while (a < 0) { a += b; }
	while (a > b) { a -= b; }
	return a;
}

/**
 * sin"a"を出力する
 * @param[in] a 入力
 * @return double sin(a)
 */
double sinC(int a) {
	double sin[91] = { 0.0000,0.0175,0.0349,0.0523,0.0698,
		0.0872,0.1045,0.1219,0.1392,0.1564,
		0.1736,0.1908,0.2079,0.2250,0.2419,
		0.2588,0.2756,0.2924,0.3090,0.3256,
		0.3420,0.3584,0.3746,0.3907,0.4067,
		0.4226,0.4384,0.4540,0.4695,0.4848,
		0.5000,0.5150,0.5299,0.5446,0.5592,
		0.5736,0.5878,0.6018,0.6157,0.6293,
		0.6428,0.6561,0.6691,0.6820,0.6947,
		0.7071,0.7193,0.7314,0.7431,0.7547,
		0.7660,0.7771,0.7880,0.7986,0.8090,
		0.8192,0.8290,0.8387,0.8480,0.8572,
		0.8660,0.8746,0.8829,0.8910,0.8988,
		0.9063,0.9135,0.9205,0.9272,0.9336,
		0.9397,0.9455,0.9511,0.9563,0.9613,
		0.9659,0.9703,0.9744,0.9781,0.9816,
		0.9848,0.9877,0.9903,0.9925,0.9945,
		0.9962,0.9976,0.9986,0.9994,0.9998,1.0000 };
	while (a < 0) { a += 360; }
	while (a >= 360) { a -= 360; }
	if (91 <= a && a <= 179) { return sin[-(a - 180)]; }
	if (180 <= a && a <= 270) { return -sin[a - 180]; }
	if (271 <= a && a <= 359) { return -sin[-(a - 360)]; }
	return sin[a];
}

/**
 * 点(x, y)を　原点(0, 0)を中心にrot度右回転させる
 * @param[out] x x座標
 * @param[out] y y座標
 * @param[in] rot 角度
 */
void rot_xy_pos(int rot, int *x, int *y) {
	int G = *x;
	*x = *x * cosC(rot) - *y * sinC(rot);
	*y = G * sinC(rot) + *y * cosC(rot);
	return;
}

/**
* HSVから色を取得する
* @param[in] hue 色相 0~360
* @param[in] saturation 彩度 0~100
* @param[in] value 明度 0~100
* @return DxColor_t RGB情報
*/
uint GetColorFromHSV(int hue, int saturation, int value) {
	int r = 0;
	int g = 0;
	int b = 0;

	saturation = betweens(0, saturation, 100);
	value = betweens(0, value, 100);
	hue = hue % 360;

	const int scaleHigh = lins(  0, 0, 100,       255, value);
	const int scaleLow  = lins(100, 0,   0, scaleHigh, saturation);

	if (IS_BETWEEN(0, hue, 60)) {
		r = 0xff;
		g = lins(0, 0x00, 60, 0xff, hue);
		b = 0x00;
	}
	else if (IS_BETWEEN(60, hue, 120)) {
		r = lins(60, 0xff, 120, 0x00, hue);
		g = 0xff;
		b = 0x00;
	}
	else if (IS_BETWEEN(120, hue, 180)) {
		r = 0x00;
		g = 0xff;
		b = lins(120, 0x00, 180, 0xff, hue);
	}
	else if (IS_BETWEEN(180, hue, 240)) {
		r = 0x00;
		g = lins(180, 0xff, 240, 0x00, hue);
		b = 0xff;
	}
	else if (IS_BETWEEN(240, hue, 300)) {
		r = lins(240, 0x00, 300, 0xff, hue);
		g = 0x00;
		b = 0xff;
	}
	else if (IS_BETWEEN(300, hue, 360)) {
		r = 0xff;
		g = 0x00;
		b = lins(300, 0xff, 360, 0x00, hue);
	}

	r = lins(0, scaleLow, 255, scaleHigh, r);
	g = lins(0, scaleLow, 255, scaleHigh, g);
	b = lins(0, scaleLow, 255, scaleHigh, b);

	return ( (r << 16) | (g << 8) | (b) );
}

/**
* curbineが個人的に綺麗だと思う虹色を出力する
* @param[in] hueParam 色相パラメータ 0~255 デジタルでよく言う色相とは違う。
*                0で赤、16でオレンジ、64で黄色、96で緑、112で水色、192で青、240で紫
* @param[in] saturation 彩度 0~100
* @param[in] value 明度 0~100
* @return DxColor_t RGB情報
*/
uint GetColorCurRainbow(int hueParam, int saturation, int value) {
	/*                             0      32      64       96       128       160       192       224       256 */
	static const int table[17] = { 0, 30, 40, 50, 60, 90, 120, 150, 180, 190, 200, 215, 240, 278, 290, 300, 360 };
	int h = 0;
	int s = 100;
	int v = 100;

	hueParam = hueParam % 256;

	for (uint iSect = 0; iSect < 16; iSect++) {
		if (IS_BETWEEN(16 * iSect, hueParam, 16 * iSect + 16)) {
			h = lins(16 * iSect, table[iSect], 16 * iSect + 16, table[iSect + 1], hueParam);
			break;
		}
	}

	if (IS_BETWEEN(60, h, 120)) {
		int max_value = lins(60, 100, 120,  75, h);
		v = lins(0, 0, 100, max_value, value);
	}
	else if (IS_BETWEEN(120, h, 180)) {
		int max_value = lins(120, 75, 180, 100, h);
		v = lins(0, 0, 100, max_value, value);
	}

	s = saturation;

	return GetColorFromHSV(h, s, v);
}

#if 1 /* cur_deviation_c */

void cur_deviation_c::add(int val) {
	this->view[this->head] = val;
	if (0x7fffffff < (double)(this->ssum) + val * val) {
		this->sum   /= 2;
		this->ssum  /= 2;
		this->count /= 2;
	}
	this->sum += val;
	this->ssum += val * val;
	this->count++;
	this->head = LOOP_ADD(this->head, 30);
	return;
}

int cur_deviation_c::GetHead(void) const {
	return this->head;
}

int cur_deviation_c::GetList(uint num) const {
	num = betweens(0, num, 29);
	return this->view[num];
}

int cur_deviation_c::GetAverage(void) const {
	if (this->count == 0) { return 0; }
	return this->sum / this->count;
}

int cur_deviation_c::GetDeviation(void) const {
	if (this->count == 0) { return 0; }
	return sanrute((this->ssum * this->count - this->sum * this->sum) / (this->count * this->count));
}

#endif /* cur_deviation_c */
