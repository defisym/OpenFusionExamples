#pragma once

constexpr auto ONMONITORCHANGE = 13;
constexpr auto ONRESIZINGCOMPLETE = 16;

constexpr auto ONCLICKBACKWARD = 18;
constexpr auto ONCLICKFORWARD = 19;

typedef struct DOUBLEPOINT
{
	DOUBLE  x;
	DOUBLE  y;
} DPOINT;

typedef struct Gauss3_Coefs {
	int N;
	double sigma;
	double B;
	double b[4];
} GCoef;

//窗口锁定类别
typedef enum _LOCKTYPE {
	LOCK_CURRENTWINDOW,
	LOCK_CLIENTAREA,
	LOCK_FRAMEAREA,
	LOCK_BYRECT,
}LT;

//矩形锁定时的相对位置
typedef enum _RELATIVETYPE {
	RELATIVE_SCREEN,
	RELATIVE_CURRENTWINDOW,
	RELATIVE_CLIENTAREA,
	RELATIVE_FRAMEAREA,
}RT;

//指定鼠标坐标类别
typedef enum _SETMOUSETYPE {
	SET_SCREEN,
	SET_CURRENTWINDOW,
	SET_CLIENTAREA,
	SET_FRAMEAREA,
}ST;

//获取偏差值类别
typedef enum _GETOFFSETTYPE {
	GET_CLIENTTOCURRENTWINDOW,
	GET_FRAMETOCLIENTAREA,
	GET_FRAMEAREATOCURRENTWINDOW,
}GT;

//获取矩形区域类别
typedef enum _GETRECT {
	GET_CURRENTWINDOW,
	GET_CLIENTAREA,
	GET_FRAMEAREA,
}GR;

//RECT运算符重载 +
inline RECT operator+(RECT A, RECT B) {
	A.left += B.left;
	A.right += B.right;
	A.top += B.top;
	A.bottom += B.bottom;
	return A;
}

//RECT运算符重载 +=
inline RECT operator +=(RECT A, RECT B) {
	A.left += B.left;
	A.right += B.right;
	A.top += B.top;
	A.bottom += B.bottom;
	return A;
}

//RECT运算符重载 -
inline RECT operator-(RECT A, RECT B) {
	A.left -= B.left;
	A.right -= B.right;
	A.top -= B.top;
	A.bottom -= B.bottom;
	return A;
}

//RECT运算符重载 -=
inline RECT operator -=(RECT A, RECT B) {
	A.left -= B.left;
	A.right -= B.right;
	A.top -= B.top;
	A.bottom -= B.bottom;
	return A;
}

//POINT运算符重载 +
inline POINT operator +(POINT A, POINT B) {
	A.x += B.x;
	A.y += B.y;
	return A;
}

//POINT运算符重载 +=
inline POINT operator +=(POINT A, POINT B) {
	A.x += B.x;
	A.y += B.y;
	return A;
}

//POINT运算符重载 -
inline POINT operator -(POINT A, POINT B) {
	A.x -= B.x;
	A.y -= B.y;
	return A;
}

//POINT运算符重载 -=
inline POINT operator -=(POINT A, POINT B) {
	A.x -= B.x;
	A.y -= B.y;
	return A;
}

//POINT运算符重载 *
inline POINT operator *(POINT A, LONG B) {
	A.x *= B;
	A.y *= B;
	return A;
}

//POINT运算符重载 /
inline POINT operator /(POINT A, LONG B) {
	A.x /= B;
	A.y /= B;
	return A;
}


//POINT运算符重载 ==
inline bool operator==(POINT A, POINT B) {
	return A.x == B.x
		&& A.y == B.y;
}

