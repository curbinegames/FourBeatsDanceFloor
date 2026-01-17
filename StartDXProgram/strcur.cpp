/**
 * @file strcur.cpp
 * @brief 文字列操作で便利なものを集めたコード
 * @author curbine
 * @date 2024/7/19
 * @note 個人が趣味で製作したものなので、品質は保証しません。このコードを使用したことによるトラブルの責任は負いかねます。
 */

#pragma once

#include <stdarg.h>
#include <tchar.h>
#include <string.h>
#include "strcur.h"
#include "sancur.h"

/**
 * p1の先頭にp2があるかどうかを調べる
 * @param[in] p1 探す場所
 * @param[in] p1size p1の長さ、配列数で指定
 * @param[in] p2 探す文字列
 * @param[in] p2size p2の長さ、配列数で指定
 * @return int あったらtrue、なかったらfalse
 */
int strands_2(const TCHAR *p1, size_t p1size, const TCHAR *p2, size_t p2size) {
	for (int i = 0; (i < p1size) && (i < p2size); i++) {
		if (p2[i] == _T('\0')) { break; }
		if (p1[i] != p2[i]) { return false; }
	}
	return true;
}

int strands_2_wc(const wchar_t *p1, size_t p1size, const wchar_t *p2, size_t p2size) {
	for (int i = 0; (i < p1size) && (i < p2size); i++) {
		if (p2[i] == _T('\0')) { break; }
		if (p1[i] != p2[i]) { return false; }
	}
	return true;
}

/**
 * p1から先頭a文字を消す
 * @param[out] p1 対象の文字列
 * @param[in] size p1のサイズ
 * @param[in] a 消す文字数
 */
void strmods_2(TCHAR *p1, size_t size, int a) {
	int i;
	TCHAR *p2 = p1;
	for (i = 0; (i < a) && (i < size); i++) {
		if (p1[i] == _T('\0')) {
			p1[0] = _T('\0');
			return;
		}
	}
	for (i = 0; p2[i + a] != _T('\0'); i++) {
		p1[i] = p2[i + a];
	}
	p1[i] = _T('\0');
	return;
}

void strmods_2_wc(wchar_t *p1, size_t size, int a) {
	int i;
	wchar_t *p2 = p1;
	for (i = 0; (i < a) && (i < size); i++) {
		if (p1[i] == _T('\0')) {
			p1[0] = _T('\0');
			return;
		}
	}
	for (i = 0; p2[i + a] != _T('\0'); i++) {
		p1[i] = p2[i + a];
	}
	p1[i] = _T('\0');
	return;
}

/**
 * p1の最後にp2を付ける
 * @param[out] p1 足される文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @param[in] p2 足す文字列
 * @note 1文字だけ追加したいときはstradds_2を使ってください
 * @sa stradds_2
 */
void strcats_2(TCHAR p1[], size_t size, const TCHAR *p2) {
	int offset = -1;

	for (int i = 0; i < size; i++) {
		if (p1[i] == _T('\0')) {
			offset = i;
			break;
		}
	}

	if (offset == -1) { return; }

	strcopy_2(p2, &p1[offset], size - offset);

	p1[size - 1] = _T('\0');
	return;
}

/**
 * 文字列sの長さを取得する
 * @param[in] s 対象となる文字列
 * @param[in] size sののサイズ、配列数で指定
 * @return int sの長さ
 */
int strlens_2(const TCHAR* s, size_t size) {
	int ret = 0;
	for (ret = 0; ret < size; ret++) {
		if (s[ret] == _T('\0')) {
			break;
		}
	}
	return ret;
}

/**
 * 文字列を数値に変換にする。
 * @param[in] p1 変換する文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @return int 変換結果
 * @su strsansD
 * @note 少数以下は無視されます。少数以下も変換したい場合はstrsansD()を使用してください
 * 途中に数字,マイナス以外の文字があったら終わり.
 */
int strsans_3(const TCHAR *p1, size_t size) {
	int a = 0, b = 1;

	for (int i = 0; i < size; i++) {
		if (IS_NUMBER_CHAR(*p1)) { a = a * 10 + (*p1 - _T('0')); }
		else if (*p1 == _T('-')) { b *= -1; }
		else { break; }
		p1++;
	}

	return a * b;
}

/**
 * 文字列を数値に変換にする。
 * @param[in] p1 変換する文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @return double 変換結果
 * @sa strsans_3
 * @note この関数は少数以下も変換します。整数のみで良い場合はstrsans_3を使用してください。
 */
double strsansD(const TCHAR *p1, size_t size) {
	short b = 1, c = 0, d = 99, i;
	double a = 0;

	for (int ip = 0; ip < size; ip++) {
		if (IS_NUMBER_CHAR(*p1)) {
			c++;
			a = a * 10 + (*p1 - _T('0'));
		}
		else if (*p1 == _T('-')) { b *= -1; }
		else if (*p1 == _T('.')) { d = c; }
		else {
			for (i = c; i > d; i--) { a /= 10.0; }
			break;
		}
		p1++;
	}

	return b * a;
}

double strsansD_ws(const wchar_t *p1, size_t size) {
	short b = 1, c = 0, d = 99, i;
	double a = 0;

	for (int ip = 0; ip < size; ip++) {
		if (IS_NUMBER_CHAR(*p1)) {
			c++;
			a = a * 10 + (*p1 - _T('0'));
		}
		else if (*p1 == _T('-')) { b *= -1; }
		else if (*p1 == _T('.')) { d = c; }
		else {
			for (i = c; i > d; i--) { a /= 10.0; }
			break;
		}
		p1++;
	}

	return b * a;
}

/**
 * 次の'/'か':'があるところまで消す
 * @param[out] p1 対象となる文字列
 * @param[in] size p1のサイズ、配列数で指定
 */
void strnex_2(TCHAR *p1, size_t size) {
	int i = 1;
	for (i = 0; (i < size) && IS_NORMAL_CHAR(p1[i]) && p1[i] != _T('/') && p1[i] != _T(':') && p1[i] != _T('\0'); i++) { ; }
	if (p1[i] == _T('/') || p1[i] == _T(':')) { i++; }
	strmods(p1, i);
	return;
}

/**
 * 指定の文字p3があるところまで消す
 * @param[out] p1 対象となる文字列
 * @param[in] p3 指定の文字
 */
void strnex_EX2(TCHAR *p1, size_t size, TCHAR p3) {
	TCHAR *p2 = p1;
	short a = 1;
	for (uint inum = 0; (inum < size) && IS_NORMAL_CHAR(*p1) && *p1 != p3 && *p1 != _T('\0'); inum++) {
		a++;
		p1++;
	}
	strmods(p2, a);
	return;
}

/**
 * p1の最後にaの文字を追加する
 * @param[out] p1 足される文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @param[in] a 足す文字
 * @note 文字列を追加したいときはstrcats_2を使ってください
 * @sa strcats_2
 */
void stradds_2(TCHAR p1[], size_t size, TCHAR a) {
	int offset = -1;

	for (int i = 0; i < size; i++) {
		if (p1[i] == _T('\0')) {
			offset = i;
			break;
		}
	}

	if (offset == -1) { return; }

	p1[offset] = a;
	p1[offset + 1] = _T('\0');
	p1[size - 1] = _T('\0');
	return;
}

/**
 * 数値を文字列にする。小数点以下は無視される
 * @param[out] ret 変換された文字列の格納場所
 * @param[in] val 変換する数値
 * @param[in] size retの長さ。配列数で指定
 * @note 小数点以下も文字列にしたい場合はstrnumsDを使ってください
 * @sa strnumsD
 */
void strnums(TCHAR ret[], int val, size_t size) {
	int keta = 1;
	int buf = val;
	int minFG = 0;

	if (size < 2) { return; }

	while (buf != 0) {
		buf /= 10;
		if (buf != 0) { keta++; }
	}

	if (val < 0) {
		ret[0] = _T('-');
		minFG = 1;
	}

	keta = mins_2(size - minFG - 1, keta);

	buf = val;
	if (val < 0) { buf *= -1; }

	for (int i = 0; i < keta; i++) {
		ret[minFG + keta - i - 1] = (TCHAR)(_T('0') + buf % 10);
		buf /= 10;
	}

	ret[minFG + keta] = _T('\0');
	ret[size - 1] = _T('\0');

	return;
}

/**
 * 数値を文字列にする。小数点以下も変換する
 * @param[out] ret 変換された文字列の格納場所
 * @param[in] val 変換する数値
 * @param[in] size retの長さ。配列数で指定
 * @param[in] under 小数点以下の桁数
 * @note 小数点以下を無視する場合はstrnumsを使ってください
 * @sa strnums
 */
void strnumsD(TCHAR ret[], double val, size_t size, int under) {
	int keta = 1;
	int buf = (int)val;
	double bufD = val;
	int minFG = 0;

	strnums(ret, (int)val, size);

	if (under <= 0) { return; }

	stradds_2(ret, size, _T('.'));

	for (int i = 0; i < under; i++) {
		bufD *= 10;
		buf = ((int)bufD) % 10;
		stradds_2(ret, size, (TCHAR)(_T('0') + (int)buf));
	}

	ret[size - 1] = _T('\0');

	return;
}

/**
 * 与えられた文字列の()内を抽出する。
 * @param[out] dest 抽出した文字列の格納場所
 * @param[in] size destの配列数
 * @param[in] src 元となる文字列
 * @param[in] c 抽出する括弧の種類。対応している括弧は、(), [], {}, <>。無効な文字が指定された場合、()として処理
 * @details 例: 入力 = "qw[cr{tg(iasd]e(knw)asd)v[vur]ne>", 指定文字 = '(', 出力 = "iasd]e(knw)asd"
 *          例: 入力 = "qw[cr{tg(iasd]e(knw)asd)v[vur]ne>", 指定文字 = '[', 出力 = "cr{tg(iasd"
 *          例: 入力 = "qw[cr{tg(iasd]e(knw)asd)v[vur]ne>", 指定文字 = '<', 出力 = ""
 *          例: 入力 = "qw[cr{tg(iasd]e(knw)asd)v[vur]ne>", 指定文字 = '{', 出力 = "tg(iasd]e(knw)asd)v[vur]ne>"
 *          例: 入力 = "qw[cr{tg(iasd]e(knw)asd)v[vur]ne>", 指定文字 = '3', 出力 = "iasd]e(knw)asd" (指定文字が無効の為、'('として処理)
 */
void strbox(TCHAR dest[], size_t size, const TCHAR src[], TCHAR c) {
	TCHAR sc = _T('(');
	TCHAR ec = _T(')');
	uint count = 1;
	uint srcp = 0;

	switch (c) {
	case _T('['):
	case _T(']'):
		sc = _T('[');
		ec = _T(']');
		break;
	case _T('{'):
	case _T('}'):
		sc = _T('{');
		ec = _T('}');
		break;
	case _T('<'):
	case _T('>'):
		sc = _T('<');
		ec = _T('>');
		break;
	}

	while (src[srcp] != sc) {
		if (src[srcp] == _T('\0')) { return; }
		srcp++;
	}
	srcp++;
	dest[0] = _T('\0');
	while (1) {
		if (src[srcp] == _T('\0')) { return; }
		if (src[srcp] == sc) { count++; }
		if (src[srcp] == ec) { count--; }
		if (count <= 0) { break; }
		stradds_2(dest, size, src[srcp]);
		srcp++;
	}
	return;
}
