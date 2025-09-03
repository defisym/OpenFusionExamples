#ifndef _GLOBALFUNC_
#define	_GLOBALFUNC_

#include <functional>

inline int GetRandomNumber(LPRDATA rdPtr) {
	int result = rdPtr->pRandomTable->front();

	rdPtr->pRandomTable->pop_front();
	rdPtr->pRandomTable->emplace_back(rdPtr->pRand->Generate());

	return result;
}

template<typename T, typename V >
inline void GeneralLoop(T& assign, size_t size, std::function<V(size_t)> f) {
	for (size_t i = 0; i < size; i++) {
		assign[i] = f(i);
	}
}

#endif // !_GLOBALFUNC_

