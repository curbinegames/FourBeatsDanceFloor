#pragma once

#include "dxcur.h"
#include "sancur.h"

typedef unsigned int uint;
typedef int intx100_t;

/** dxdrawで使う基準点の列挙型 */
typedef enum dxdraw_anchor_e {
	DXDRAW_ANCHOR_TOP_LEFT = 0,  /** 上の左 */
	DXDRAW_ANCHOR_TOP_CENTRE,    /** 上の真ん中 */
	DXDRAW_ANCHOR_TOP_RIGHT,     /** 上の右 */
	DXDRAW_ANCHOR_CENTRE_LEFT,   /** 真ん中の左 */
	DXDRAW_ANCHOR_CENTRE,        /** 真ん中 */
	DXDRAW_ANCHOR_CENTRE_RIGHT,  /** 真ん中の右 */
	DXDRAW_ANCHOR_BOTTOM_LEFT,   /** 下の左 */
	DXDRAW_ANCHOR_BOTTOM_CENTRE, /** 下の真ん中 */
	DXDRAW_ANCHOR_BOTTOM_RIGHT,  /** 下の右 */
} dxdraw_anchor_t;

extern int  GetScreenXSize(void);
extern int  GetScreenYSize(void);
extern void GetScreenSize(int *x, int *y);
extern void DrawLineAnchor(int left, int up, int right, int down, uint color, dxdraw_anchor_t anchor);
extern void DrawBoxAnchor(int left, int up, int right, int down, uint color, dxdraw_anchor_t anchor, int FillFg);
extern void DrawGraphAnchor(int x, int y, int pic, dxdraw_anchor_t anchor);
extern void DrawZoomGraphAnchor(int x, int y, intx100_t size, int pic, dxdraw_anchor_t anchor);
extern void DrawRotaGraphAnchor(int x, int y, double size, double rot, DxPic_t pic, dxdraw_anchor_t anchor, int TransFlag, int ReverseXFlag = 0, int ReverseYFlag = 0);
extern void DrawStringAnchor(int x, int y, const TCHAR *s, DxColor_t cr, dxdraw_anchor_t anchor);
extern void DrawStringToHandleAnchor(int x, int y, const TCHAR *s, DxColor_t cr, int handle, dxdraw_anchor_t anchor);
extern void DrawFormatStringAnchor(int x, int y, DxColor_t cr, dxdraw_anchor_t anchor, const TCHAR *s, ...);
extern void DrawFormatStringToHandleAnchor(int x, int y, DxColor_t cr, int handle, dxdraw_anchor_t anchor, const TCHAR *s, ...);
extern void DrawDeformationPic(int x, int y, double sizeX, double sizeY, int rot, int handle);
