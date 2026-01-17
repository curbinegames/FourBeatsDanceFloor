
#include "DxLib.h"
#include "dxdraw.h"
#include "dxcur.h"
#include "sancur.h"
#include "strcur.h"

typedef unsigned int uint;
typedef int intx100_t;

#define DEFAULT_WINDOW_SIZE_X 640
#define DEFAULT_WINDOW_SIZE_Y 480

int GetScreenXSize(void) {
	int x = DEFAULT_WINDOW_SIZE_X;
	int y = DEFAULT_WINDOW_SIZE_Y;
	int cr = 0;
	GetScreenState(&x, &y, &cr);
	return x;
}

int GetScreenYSize(void) {
	int x = DEFAULT_WINDOW_SIZE_X;
	int y = DEFAULT_WINDOW_SIZE_Y;
	int cr = 0;
	GetScreenState(&x, &y, &cr);
	return y;
}

void GetScreenSize(int *x, int *y) {
	int cr = 0;
	GetScreenState(x, y, &cr);
	return;
}

void DrawLineAnchor(int left, int up, int right, int down, uint color, dxdraw_anchor_t anchor) {
	int drawX = left;
	int drawY = up;
	int sizeX = right - left;
	int sizeY = down - up;
	int screenX = DEFAULT_WINDOW_SIZE_X;
	int screenY = DEFAULT_WINDOW_SIZE_Y;
	GetScreenSize(&screenX, &screenY);

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawLine(left, up, right, down, color);
		return;
	}

	/* TODO: 座標変更を別関数にする */
	switch (anchor) {
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
		drawX = left;
		break;
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
		drawX = (screenX - sizeX + 2 * left) / 2;
		break;
	case DXDRAW_ANCHOR_TOP_RIGHT:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawX = screenX + left;
		break;
	}

	switch (anchor) {
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_TOP_RIGHT:
		drawY = up;
		break;
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
		drawY = (screenY - sizeY + 2 * up) / 2;
		break;
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawY = screenY + up;
		break;
	}

	DrawLine(drawX, drawY, drawX + sizeX, drawY + sizeY, color);

	return;
}

/* TODO: centre系の仕様を変更する */
void DrawBoxAnchor(int left, int up, int right, int down, uint color, dxdraw_anchor_t anchor, int FillFg) {
	int drawX = left;
	int drawY = up;
	int sizeX = right - left;
	int sizeY = down - up;
	int screenX = DEFAULT_WINDOW_SIZE_X;
	int screenY = DEFAULT_WINDOW_SIZE_Y;
	GetScreenSize(&screenX, &screenY);

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawBox(left, up, right, down, color, FillFg);
		return;
	}

	switch (anchor) {
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
		drawX = left;
		break;
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
		drawX = (screenX - sizeX + 2 * left) / 2;
		break;
	case DXDRAW_ANCHOR_TOP_RIGHT:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawX = screenX + left;
		break;
	}

	switch (anchor) {
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_TOP_RIGHT:
		drawY = up;
		break;
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
		drawY = (screenY - sizeY + 2 * up) / 2;
		break;
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawY = screenY + up;
		break;
	}

	DrawBox(drawX, drawY, drawX + sizeX, drawY + sizeY, color, FillFg);

	return;
}


static int TransDrawX(int baseX, int sizeX, dxdraw_anchor_t anchor) {
	int drawX = 0;
	int screenX = GetScreenXSize();

	switch (anchor) {
	case DXDRAW_ANCHOR_TOP_LEFT:
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
		drawX = baseX;
		break;
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
		drawX = (screenX - sizeX + 2 * baseX) / 2;
		break;
	case DXDRAW_ANCHOR_TOP_RIGHT:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawX = screenX - sizeX + baseX;
		break;
	}

	return drawX;
}

static int TransDrawY(int baseY, int sizeY, dxdraw_anchor_t anchor) {
	int drawY = 0;
	int screenY = GetScreenYSize();

	switch (anchor) {
	case DXDRAW_ANCHOR_TOP_LEFT:
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_TOP_RIGHT:
		drawY = baseY;
		break;
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
		drawY = (screenY - sizeY + 2 * baseY) / 2;
		break;
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawY = screenY - sizeY + baseY;
		break;
	}

	return drawY;
}

/**
 * DrawGraphに基準点を追加したもの
 * @param[in] x x位置
 * @param[in] y y位置
 * @param[in] pic 画像ハンドル
 * @param[in] anchor 基準点
 * @details DrawGraphAnchor(xxx, xxx, xxx, DXDRAW_ANCHOR_TOP_LEFT)はDrawGraph(xxx,xxx,xxx,TRUE)と同じ。
 */
void DrawGraphAnchor(int x, int y, int pic, dxdraw_anchor_t anchor) {
	int drawX = 0;
	int drawY = 0;
	int sizeX = 0;
	int sizeY = 0;

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawGraph(x, y, pic, TRUE);
		return;
	}

	GetGraphSize(pic, &sizeX, &sizeY);

	drawX = TransDrawX(x, sizeX, anchor);
	drawY = TransDrawY(y, sizeY, anchor);

	DrawGraph(drawX, drawY, pic, TRUE);

	return;
}

/**
 * DrawGraphAnchorに拡大量を追加したもの
 * @param[in] x x位置
 * @param[in] y y位置
 * @param[in] size 拡大量、100で等倍
 * @param[in] pic 画像ハンドル
 * @param[in] anchor 基準点
 * @details sizeに100が入った場合、DrawGraphAnchorを実行する
 */
void DrawZoomGraphAnchor(int x, int y, intx100_t size, int pic, dxdraw_anchor_t anchor) {
	int drawLeft = 0;
	int drawUp = 0;
	int drawRight = 0;
	int drawDown = 0;
	int sizeX = 0;
	int sizeY = 0;

	if (size == 100) {
		DrawGraphAnchor(x, y, pic, anchor);
		return;
	}

	GetGraphSize(pic, &sizeX, &sizeY);
	sizeX = sizeX * size / 100;
	sizeY = sizeY * size / 100;

	drawLeft = TransDrawX(x, sizeX, anchor);
	drawUp   = TransDrawY(y, sizeY, anchor);

	drawRight = x + sizeX;
	drawDown  = y + sizeY;

	DrawExtendGraph(drawLeft, drawUp, drawRight, drawDown, pic, TRUE);

	return;
}

void DrawRotaGraphAnchor(int x, int y, double size, double rot, DxPic_t pic, dxdraw_anchor_t anchor, int TransFlag, int ReverseXFlag, int ReverseYFlag) {
	int drawX = 0;
	int drawY = 0;

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawRotaGraph(x, y, size, rot, pic, TRUE, ReverseXFlag, ReverseYFlag);
		return;
	}

	drawX = TransDrawX(x, 0, anchor);
	drawY = TransDrawY(y, 0, anchor);

	DrawRotaGraph(drawX, drawY, size, rot, pic, TransFlag, ReverseXFlag, ReverseYFlag);
	return;
}

void DrawStringAnchor(int x, int y, const TCHAR *s, DxColor_t cr, dxdraw_anchor_t anchor) {
	int drawX = 0;
	int drawY = 0;

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawString(x, y, s, cr);
		return;
	}

	drawX = TransDrawX(x, 0, anchor);
	drawY = TransDrawY(y, 0, anchor);

	DrawString(drawX, drawY, s, cr);
	return;
}

void DrawStringToHandleAnchor(int x, int y, const TCHAR *s, DxColor_t cr, int handle, dxdraw_anchor_t anchor) {
	int drawX = 0;
	int drawY = 0;

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawStringToHandle(x, y, s, cr, handle);
		return;
	}

	drawX = TransDrawX(x, 0, anchor);
	drawY = TransDrawY(y, 0, anchor);

	DrawStringToHandle(drawX, drawY, s, cr, handle);
	return;
}

void DrawFormatStringAnchor(int x, int y, DxColor_t cr, dxdraw_anchor_t anchor, const TCHAR *s, ...) {
	va_list as;
	va_start(as, s);

	int drawX = 0;
	int drawY = 0;
	TCHAR buf[256];

	_vstprintf_s(buf, s, as);

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawString(x, y, buf, cr);
		return;
	}

	drawX = TransDrawX(x, 0, anchor);
	drawY = TransDrawY(y, 0, anchor);

	DrawString(drawX, drawY, buf, cr);

	va_end(as);
	return;
}

void DrawFormatStringToHandleAnchor(int x, int y, DxColor_t cr, int handle, dxdraw_anchor_t anchor, const TCHAR *s, ...) {
	va_list as;
	va_start(as, s);

	int drawX = 0;
	int drawY = 0;
	TCHAR buf[256];

	_vstprintf_s(buf, s, as);

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawStringToHandle(x, y, buf, cr, handle);
		return;
	}

	drawX = TransDrawX(x, 0, anchor);
	drawY = TransDrawY(y, 0, anchor);

	DrawStringToHandle(drawX, drawY, buf, cr, handle);

	va_end(as);
	return;
}

#if 1 /* DrawDeformationPic */

static void DrawDeformationPic_NoDef(int x, int y, int handle) {
	int PSizeX = 1;
	int PSizeY = 1;
	GetGraphSize(handle, &PSizeX, &PSizeY);
	x -= PSizeX / 2;
	y -= PSizeY / 2;
	DrawGraph(x, y, handle, TRUE);
	return;
}

static void DrawDeformationPic_Turn(int x, int y, int handle) {
	int PSizeX = 1;
	int PSizeY = 1;
	GetGraphSize(handle, &PSizeX, &PSizeY);
	x -= PSizeX / 2;
	y -= PSizeY / 2;
	DrawTurnGraph(x, y, handle, TRUE);
	return;
}

static void DrawDeformationPic_Extend(int x, int y, double sizeX, double sizeY, int handle) {
	int TSizeX = 1;
	int TSizeY = 1;
	GetGraphSize(handle, &TSizeX, &TSizeY);
	TSizeX *= sizeX;
	TSizeY *= sizeY;
	int pos[4] = {
		x - TSizeX / 2, y - TSizeY / 2, 0, 0
	};
	pos[2] = pos[0] + TSizeX + 1;
	pos[3] = pos[1] + TSizeY + 1;
	DrawExtendGraph(pos[0], pos[1], pos[2], pos[3], handle, TRUE);
	return;
}

static void DrawDeformationPic_Rot(int x, int y, double size, int rot, int handle) {
	int PSizeX = 1;
	int PSizeY = 1;
	GetGraphSize(handle, &PSizeX, &PSizeY);
	PSizeX /= 2;
	PSizeY /= 2;
	DrawRotaGraph2(x, y, PSizeX, PSizeY, size, 3.14 * rot / 180, handle, TRUE);
	return;
}

static void DrawDeformationPic_FullDef(int x, int y, double sizeX, double sizeY, int rot, int handle) {
	int PSizeX = 1;
	int PSizeY = 1;
	GetGraphSize(handle, &PSizeX, &PSizeY);
	PSizeX /= 2;
	PSizeY /= 2;
	DrawRotaGraph3(x, y, PSizeX, PSizeY, sizeX, sizeY, 3.14 * rot / 180, handle, TRUE);
	return;
}

void DrawDeformationPic(int x, int y, double sizeX, double sizeY, int rot, int handle) {
	if (rot % 360 == 0) { /* 回転が0 */
		if (sizeX == 1 && sizeY == 1) { /* 拡大率が1 */
			DrawDeformationPic_NoDef(x, y, handle);
		}
		else if (sizeX == -1 && sizeY == 1) { /* 左右反転 */
			DrawDeformationPic_Turn(x, y, handle);
		}
		else {
			DrawDeformationPic_Extend(x, y, sizeX, sizeY, handle);
		}
	}
	else {
		if (sizeX == sizeY) { /* XY拡大率が一緒 */
			DrawDeformationPic_Rot(x, y, sizeX, rot, handle);
		}
		else {
			DrawDeformationPic_FullDef(x, y, sizeX, sizeY, rot, handle);
		}
	}
	return;
}

#endif /* DrawDeformationPic */
