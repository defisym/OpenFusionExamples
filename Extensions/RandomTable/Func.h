#ifndef _GLOBALFUNC_
#define	_GLOBALFUNC_

inline int GetRandomNumber(LPRDATA rdPtr) {
	auto result = (*rdPtr->pRandomTable)[0];

	rdPtr->pRandomTable->pop_front();
	rdPtr->pRandomTable->push_back(rdPtr->pRand->GenerateRandNumber());

	return result;
}

#endif // !_GLOBALFUNC_

