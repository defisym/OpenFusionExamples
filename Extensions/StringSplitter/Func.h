#pragma once

#include "Split.h"

inline void SplitData(LPRDATA rdPtr) {
	if(!rdPtr->bCache) {
		Spliter->SplitData();

		return;
	}

	const auto hash = Spliter->GetHash();
	const auto it = rdPtr->pData->data.find(hash);

	if (it != rdPtr->pData->data.end()) {
		Spliter->SetResult(&it->second);
	}
	else {
		SplitResult result;

		Spliter->SplitData();
		Spliter->GetResult(&result);

		rdPtr->pData->data[hash] = result;
	}
}
