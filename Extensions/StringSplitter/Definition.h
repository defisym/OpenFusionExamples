#pragma once

#define Splitter rdPtr->S

#define ONIT_SSV	0
#define ONIT_KWPV	1
#define ONIT_MSS	2
#define ONIT_RPE    5

struct GlobalData {
	std::map<size_t, SplitResult> data;
};