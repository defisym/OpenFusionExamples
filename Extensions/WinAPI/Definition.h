#ifndef _DEFINITION_
#define _DEFINITION_

typedef struct DOUBLEPOINT
{
	DOUBLE  x;
	DOUBLE  y;
} DPOINT;

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
RECT operator+(RECT A, RECT B);

//RECT运算符重载 +=
RECT operator +=(RECT A, RECT B);

//RECT运算符重载 -
RECT operator-(RECT A, RECT B);

//RECT运算符重载 -=
RECT operator -=(RECT A, RECT B);

//POINT运算符重载 +
POINT operator +(POINT A, POINT B);

//POINT运算符重载 +=
POINT operator +=(POINT A, POINT B);

//POINT运算符重载 -
POINT operator -(POINT A, POINT B);

//POINT运算符重载 -=
POINT operator -=(POINT A, POINT B);

//POINT运算符重载 *
POINT operator *(POINT A, LONG B);

//POINT运算符重载 /
POINT operator /(POINT A, LONG B);

#endif // !_DEFINITION_

