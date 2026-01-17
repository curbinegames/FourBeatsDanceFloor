#pragma once

/* a/bを返す、ただし、bが0の時はcを返す。0除算回避の為に作った */
#define DIV_AVOID_ZERO(a, b, c) ( ((b) == 0) ? (c) : ((a) / (double)(b)) )

/* a%bを返す、ただし、bが0の時はcを返す。0除算回避の為に作った */
#define MOD_AVOID_ZERO(a, b, c) ( ((b) == 0) ? (c) : ((a) % (int)(b)) )

#define LOOP_ADD(mat, loop) ( ((mat) + 1) % (loop) )
#define LOOP_SUB(mat, loop) ( ((mat) + (loop) - 1) % (loop) )

/* 小さいほうを返す */
#define mins_2(a, b) ( ((a) < (b)) ? (a) : (b) )

/* 大きいほうを返す */
#define maxs_2(a, b) ( ((a) > (b)) ? (a) : (b) )

/* bをaまで引き上げ、cまで引き下げたものを返す */
#define betweens(a, b, c) mins_2(maxs_2((a), (b)), (c))

/* cos"a"を出力する */
#define cosC(a) sinC((a) + 90)

/* bがa以上c以下ならtrueを返す */
#define IS_BETWEEN(a, b, c)            ( (a) <= (b) && (b) <= (c) )

/* bがa超えc未満ならtrueを返す */
#define IS_BETWEEN_LESS(a, b, c)       ( (a) <  (b) && (b) <  (c) )

/* bがa超えc以下ならtrueを返す */
#define IS_BETWEEN_LEFT_LESS(a, b, c)  ( (a) <  (b) && (b) <= (c) )

/* bがa以上c未満ならtrueを返す */
#define IS_BETWEEN_RIGHT_LESS(a, b, c) ( (a) <= (b) && (b) <  (c) )

/* ↑命名の意図、"="がない(LESSしている)からIS_BETWEEN_"LESS" */

/* 配列数を返す */
#define ARRAY_COUNT(a) ( sizeof(a) / sizeof(a[0]) )

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef int intx100_t; /** 100倍していることを示すint型 */

typedef enum cur_move_e {
	CUR_MOVE_LINE = 1,
	CUR_MOVE_ACC,
	CUR_MOVE_DEC
} cur_move_type_t;

extern double lins(double x1, double y1, double x2, double y2, double x);
extern double lins_scale(double x1, double y1, double x2, double y2, double x);
extern double pals(double x1, double y1, double x2, double y2, double x);
extern double pals_scale(double x1, double y1, double x2, double y2, double x);
extern double movecal(int mode, double x1, double y1, double x2, double y2, double x);
extern double sanrute(double c);
extern int    abss(int a, int b);
extern double sinC(int a);
extern void   rot_xy_pos(int rot, int *x, int *y);
extern uint   GetColorFromHSV(int hue, int saturation, int value);
extern uint   GetColorCurRainbow(int hueParam, int saturation, int value);

/* 与えられたデータの平均と標準偏差を求めるクラス */
/* 少しレコラン寄りのコードになってる */
class cur_deviation_c {
private:
	int  view[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int  sum      = 0; /* 合計 */
	uint ssum     = 0; /* 2乗の合計 */
	uint count    = 0; /* データ数 */
	uint head     = 0; /* 次に書く場所 */

public:
	void add(int val);
	int GetHead(void) const;
	int GetList(uint num) const;
	int GetAverage(void) const;
	int GetDeviation(void) const;
};

/* 使用してはいけない関数。過去に使っていたことがあるので、互換のために残す */
#define maxs(a, b)          mins_2((a), (b))
#define mins(a, b)          maxs_2((a), (b))
#define mins_D(a, b)        maxs_2(a, b)
#define maxs_D(a, b)        mins_2(a, b)
#define betweens_D(a, b, c) betweens(a, b, c)
#define notzero(a)          ( ((a) == 0) ? 1 : (a) )
extern int NumLoop(int a, int b);
