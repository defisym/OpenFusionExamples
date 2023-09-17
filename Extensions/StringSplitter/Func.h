#pragma once

#include "Split.h"

inline void SplitData(LPRDATA rdPtr) {
	if(!rdPtr->bCache) {
		Splitter->SplitData();

		return;
	}

	const auto hash = Splitter->GetHash();
	const auto it = rdPtr->pData->data.find(hash);

	if (it != rdPtr->pData->data.end()) {
		Splitter->SetResult(&it->second);
	}
	else {
		Splitter->SplitData();
		Splitter->GetResult(&rdPtr->pData->data[hash]);
	}
}
