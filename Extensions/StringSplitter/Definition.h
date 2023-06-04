#pragma once

#define Spliter rdPtr->S

#define ONIT_SSV	0
#define ONIT_KWPV	1
#define ONIT_MSS	2
#define ONIT_RPE    5

//Need not to release currents cause they are pointers pointed to Split class's variable value
#define release_str() release_arr(rdPtr->SplitStrVecLoopName);release_arr(rdPtr->KeyWordPairVecLoopName);release_arr(rdPtr->SubStringVecLoopName);release_arr(rdPtr->CurrentReplaceString);release_arr(rdPtr->ReplacEachResult);release_arr(rdPtr->ReplaceEachLoopName);

struct GlobalData {
	std::map<size_t, SplitResult> data;
};