#pragma once

#include "Split.h"

inline void SplitData(LPRDATA rdPtr) {
	if(!rdPtr->bCache) {
        rdPtr->pSplitter->SplitData();

		return;
	}

	const auto hash = rdPtr->pSplitter->GetHash();
	const auto it = rdPtr->pData->data.find(hash);

	if (it != rdPtr->pData->data.end()) {
        rdPtr->pSplitter->SetResult(&it->second);
	}
	else {
        rdPtr->pSplitter->SplitData();
        rdPtr->pSplitter->GetResult(&rdPtr->pData->data[hash]);
	}
}
