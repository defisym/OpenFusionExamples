// Common Include
#include	"common.h"

//RECT运算符重载 +
RECT operator +(RECT A, RECT B) {
	A.left += B.left;
	A.right += B.right;
	A.top += B.top;
	A.bottom += B.bottom;
	return A;
}

//RECT运算符重载 +=
RECT operator +=(RECT A, RECT B) {
	A.left += B.left;
	A.right += B.right;
	A.top += B.top;
	A.bottom += B.bottom;
	return A;
}

//RECT运算符重载 -
RECT operator -(RECT A, RECT B) {
	A.left -= B.left;
	A.right -= B.right;
	A.top -= B.top;
	A.bottom -= B.bottom;
	return A;
}

//RECT运算符重载 -
RECT operator -=(RECT A, RECT B) {
	A.left -= B.left;
	A.right -= B.right;
	A.top -= B.top;
	A.bottom -= B.bottom;
	return A;
}

//POINT运算符重载 +
POINT operator +(POINT A, POINT B) {
	A.x += B.x;
	A.y += B.y;
	return A;
}

//POINT运算符重载 +=
POINT operator +=(POINT A, POINT B) {
	A.x += B.x;
	A.y += B.y;
	return A;
}

//POINT运算符重载 -
POINT operator -(POINT A, POINT B) {
	A.x -= B.x;
	A.y -= B.y;
	return A;
}

//POINT运算符重载 -=
POINT operator -=(POINT A, POINT B) {
	A.x -= B.x;
	A.y -= B.y;
	return A;
}

//POINT运算符重载 *
POINT operator *(POINT A, LONG B) {
	A.x *= B;
	A.y *= B;
	return A;
}

//POINT运算符重载 /
POINT operator /(POINT A, LONG B) {
	A.x /= B;
	A.y /= B;
	return A;
}
