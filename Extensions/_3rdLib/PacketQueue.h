#pragma once

#include <queue>

extern "C" {
#include <libavformat/avformat.h>
}

#include <SDL.h>
#include <SDL_thread.h>

constexpr auto QUEUE_WAITING = -1;

#define QUEUE_SPINLOCK

class PacketQueue {
private:
	AVPacket* pEmptyPacket = nullptr;
	std::queue<AVPacket> queue;
	int dataSize = 0;

	bool bExit = false;

#ifdef QUEUE_SPINLOCK
	bool bReadFinish = false;
	SDL_SpinLock queueLock = 0;
#else
	SDL_mutex* mutex;
	SDL_cond* cond;
#endif
	
	inline void erase() {
		while (!queue.empty()) {
			auto& packet = queue.front();

			if (packet.data) {
				av_packet_unref(&packet);
				//should not free it here as it's POD
				//av_packet_free(&pPacket);
			}

			queue.pop();
		}

		dataSize = 0;
	}

public:
	PacketQueue() {
		pEmptyPacket = av_packet_alloc();

#ifdef QUEUE_SPINLOCK
#else
		mutex = SDL_CreateMutex();
		cond = SDL_CreateCond();
#endif
	}

	~PacketQueue() {
		av_packet_free(&pEmptyPacket);

		flush();
		pause();

#ifdef QUEUE_SPINLOCK
#else
		SDL_DestroyMutex(mutex);
		SDL_DestroyCond(cond);
#endif
	}

	// ------------------------------------
	// Info
	// ------------------------------------

	inline size_t getQueueSize() const { return queue.size(); }

	inline int getDataSize() const { return dataSize; }

	// ------------------------------------
	// Queue control
	// ------------------------------------

	inline void exit() {
		if (bExit) { return; }
		bExit = true;

#ifdef QUEUE_SPINLOCK
#else
		SDL_CondSignal(cond);
#endif
	}

#ifdef QUEUE_SPINLOCK
	inline bool readFinish() const { return bReadFinish; }
#endif

	inline void stopBlock() {
#ifdef QUEUE_SPINLOCK
		if (bReadFinish) { return; }
		bReadFinish = true;
#endif
		pause();
		restore();
	}

	inline void pause() {
		exit();

#ifdef QUEUE_SPINLOCK
#else
		SDL_LockMutex(mutex);
		SDL_UnlockMutex(mutex);
#endif
	}

	inline void restore() {
#ifdef QUEUE_SPINLOCK
		SDL_AtomicLock(&queueLock);
#else
		SDL_LockMutex(mutex);
#endif
		
		this->bExit = false;

#ifdef QUEUE_SPINLOCK
		SDL_AtomicUnlock(&queueLock);
#else
		SDL_UnlockMutex(mutex);
#endif
	}

	// ------------------------------------
	// Update queue
	// ------------------------------------

	inline void flush() {
		pause();

#ifdef QUEUE_SPINLOCK
		SDL_AtomicLock(&queueLock);
#else
		SDL_LockMutex(mutex);
#endif

		erase();

#ifdef QUEUE_SPINLOCK
		bReadFinish = false;
		SDL_AtomicUnlock(&queueLock);
#else
		SDL_UnlockMutex(mutex);
#endif

		restore();
	}

	inline bool put(AVPacket* pPacket) {
#ifdef QUEUE_SPINLOCK
		SDL_AtomicLock(&queueLock);
#else
		SDL_LockMutex(mutex);
#endif
		AVPacket pkt = *pEmptyPacket;
		av_packet_move_ref(&pkt, pPacket);

		queue.emplace(pkt);
		dataSize += pkt.size;
		
#ifdef QUEUE_SPINLOCK
#else
		SDL_CondSignal(cond);
#endif

#ifdef QUEUE_SPINLOCK
		SDL_AtomicUnlock(&queueLock);
#else
		SDL_UnlockMutex(mutex);
#endif

		return true;
	}

	//block: block thread then wait
	inline int get(AVPacket* pPacket, bool block = true) {
		int ret = false;

#ifdef QUEUE_SPINLOCK
		SDL_AtomicLock(&queueLock);
#else
		SDL_LockMutex(mutex);
#endif

		while (true) {
			if (bExit) {
				ret = QUEUE_WAITING;

				break;
			}

			if (!queue.empty()) {
				av_packet_unref(pPacket);
				av_packet_move_ref(pPacket, &queue.front());

				queue.pop();
				dataSize -= pPacket->size;

				ret = true;

				break;
			}

#ifdef QUEUE_SPINLOCK
			ret = bReadFinish
				? false
				: QUEUE_WAITING;

			break;
#else
			else if (!block) {
				ret = false;

				break;
			}
			else {
				SDL_CondWait(cond, mutex);
			}
#endif

		}

#ifdef QUEUE_SPINLOCK
		SDL_AtomicUnlock(&queueLock);
#else
		SDL_UnlockMutex(mutex);
#endif

		return ret;
	}
};
