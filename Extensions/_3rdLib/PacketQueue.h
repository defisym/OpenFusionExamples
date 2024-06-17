#pragma once

#include <queue>

extern "C" {
#include <libavformat/avformat.h>
}

//#define STL
#define SDL

// https://blog.csdn.net/fengchu3415/article/details/82453808

#ifdef STL
#include <condition_variable>
#include <functional>
#include <atomic>
#include <memory>
#include <mutex>

class PacketQueue {
private:
	std::queue<AVPacket> queue;
	int dataSize = 0;

	// Thread gubbins
	std::mutex _mutex;
	std::condition_variable _fullQue;
	std::condition_variable _empty;

	// Exit
	// 原子操作
	std::atomic_bool _quit; //{ false };
	std::atomic_bool _finished; // { false };

public:
	PacketQueue() {
		_quit = ATOMIC_VAR_INIT(false);
		_finished = ATOMIC_VAR_INIT(false);
	}

	inline size_t size() {
		return queue.size();
	}

	inline int getDataSize() {
		return dataSize;
	}

	inline bool put(const AVPacket* pPacket) {
		std::unique_lock<std::mutex> lock(_mutex);

		while (!_quit && !_finished) {
			AVPacket pkt;

			if (av_packet_ref(&pkt, pPacket) < 0) {
				return false;
			}

			queue.push(pkt);
			dataSize += pkt.size;

			_empty.notify_all();

			return true;
		}

		return false;
	}

	inline bool get(AVPacket* pPacket) {
		std::unique_lock<std::mutex> lock(_mutex);

		while (!_quit) {
			if (!queue.empty()) {
				if (this->queue.empty()) {
					return false;
				}

				if (av_packet_ref(pPacket, &queue.front()) < 0) {
					return false;
				}

				queue.pop();
				dataSize -= pPacket->size;

				_fullQue.notify_all();

				return true;
			}
			else if (queue.empty() && _finished) {
				return false;
			}
			else {
				_empty.wait(lock);
			}
		}
		return false;

	}

	// The queue has finished accepting input
	void finished() {
		_finished = true;

		_empty.notify_all();
	}

	void quit() {
		_quit = true;

		_empty.notify_all();
		_fullQue.notify_all();
	}
};
#endif

#ifdef SDL

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
	SDL_SpinLock audioLock = 0;
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

	inline size_t size() const {
		return queue.size();
	}

	inline int getDataSize() const {
		return dataSize;
	}

	inline void exit() {
		if (bExit) {
			return;
		}

		bExit = true;

#ifdef QUEUE_SPINLOCK
#else
		SDL_CondSignal(cond);
#endif
	}

#ifdef QUEUE_SPINLOCK
	inline bool readFinish() const {
		return bReadFinish;
	}
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
		SDL_AtomicLock(&audioLock);
#else
		SDL_LockMutex(mutex);
#endif
		
		this->bExit = false;

#ifdef QUEUE_SPINLOCK
		SDL_AtomicUnlock(&audioLock);
#else
		SDL_UnlockMutex(mutex);
#endif
	}
		
	inline void flush() {
		pause();

#ifdef QUEUE_SPINLOCK
		SDL_AtomicLock(&audioLock);
#else
		SDL_LockMutex(mutex);
#endif

		erase();
		bReadFinish = false;

#ifdef QUEUE_SPINLOCK
		SDL_AtomicUnlock(&audioLock);
#else
		SDL_UnlockMutex(mutex);
#endif

		restore();
	}

	inline bool put(AVPacket* pPacket) {
#ifdef QUEUE_SPINLOCK
		SDL_AtomicLock(&audioLock);
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
		SDL_AtomicUnlock(&audioLock);
#else
		SDL_UnlockMutex(mutex);
#endif

		return true;
	}

	//block: 阻塞线程并等待
	inline int get(AVPacket* pPacket, bool block = true) {
		int ret = false;

#ifdef QUEUE_SPINLOCK
		SDL_AtomicLock(&audioLock);
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
		SDL_AtomicUnlock(&audioLock);
#else
		SDL_UnlockMutex(mutex);
#endif

		return ret;
	}
};
#endif