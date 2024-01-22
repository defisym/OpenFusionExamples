#pragma once

struct GDIObjectCounter {
	DWORD objectCount = 0;
	DWORD objectCountPeak = 0;
	DWORD objectCountPrevious = 0;

	inline void UpdateObjectCount() {
		objectCount = ::GetGuiResources(::GetCurrentProcess(), GR_GDIOBJECTS);

		if(objectCount > objectCountPeak) {
			objectCountPeak = objectCount;
		}

		if(objectCount > objectCountPrevious) {
			const auto objCount = std::format(L"Add GDI Object Count: {}\n", objectCount);
			OutputDebugStringW(objCount.c_str());
		}

		if (objectCount < objectCountPrevious) {
			const auto objCount = std::format(L"Sub GDI Object Count: {}\n", objectCount);
			OutputDebugStringW(objCount.c_str());
		}

		objectCountPrevious = objectCount;
	}
};