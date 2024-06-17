#pragma once

class AudioInterface {
public:
	AudioInterface() = default;
	virtual ~AudioInterface() = default;
	virtual inline void AddInstance(FFMpeg** ppFFMpeg, void* pData) = 0;
	virtual inline void RemoveInstance(FFMpeg** ppFFMpeg, void* pData) = 0;
};