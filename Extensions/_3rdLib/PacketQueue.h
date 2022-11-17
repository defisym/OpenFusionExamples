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

class packetQueue {
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
	packetQueue() {
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

class packetQueue {
private:
	std::queue<AVPacket> queue;
	int dataSize = 0;

	SDL_mutex* mutex;
	SDL_cond* cond;

	SDL_mutex* mutexExit;
	SDL_cond* condExit;

	//AVPacket pkt = { 0 };

	bool bExit = false;

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
	packetQueue() {
		mutex = SDL_CreateMutex();
		cond = SDL_CreateCond();

		mutexExit = SDL_CreateMutex();
		condExit = SDL_CreateCond();
	}

	~packetQueue() {
		//exit();
		pause();

		SDL_CondWait(condExit, mutexExit);

		flush();
		pause();

		SDL_DestroyMutex(mutex);
		SDL_DestroyCond(cond);

		SDL_DestroyMutex(mutexExit);
		SDL_DestroyCond(condExit);
	}

	inline size_t size() {
		return queue.size();
	}

	inline int getDataSize() {
		return dataSize;
	}

	inline void exit() {
		if (bExit) {
			return;
		}

		bExit = true;
		SDL_CondSignal(cond);
	}

	inline void stopBlock() {
		// don't use exit() as parent will get -1 as wait and sent nullptr
		SDL_CondSignal(cond);
	}

	inline void pause() {
		exit();

		SDL_LockMutex(mutex);
		SDL_UnlockMutex(mutex);
	}

	inline void restore() {
		SDL_LockMutex(mutex);
		
		this->bExit = false;
		
		SDL_UnlockMutex(mutex);
	}
		
	inline void flush() {
		pause();

		SDL_LockMutex(mutex);

		erase();

		SDL_UnlockMutex(mutex);

		restore();
	}

	inline bool put(const AVPacket* pPacket) {
		//AVPacket pkt;

		//if (av_packet_ref(&pkt, pPacket) < 0) {
		//	return false;
		//}

		AVPacket pkt = *pPacket;

		SDL_LockMutex(mutex);
		queue.push(pkt);

		dataSize += pkt.size;
		//nb_packets++;

		SDL_CondSignal(cond);
		SDL_UnlockMutex(mutex);

		return true;
	}

	//block: 阻塞线程并等待
	inline BOOL get(AVPacket* pPacket, bool block = true) {
		BOOL ret = false;

		SDL_LockMutex(mutex);

		while (true) {
			if (bExit) {
				ret = QUEUE_WAITING;

				break;
			}

			if (!queue.empty()) {
				//if (av_packet_ref(pPacket, &queue.front()) < 0) {
				//	ret = false;

				//	break;
				//}

				*pPacket = queue.front();

				queue.pop();
				//nb_packets--;
				dataSize -= pPacket->size;

				ret = true;

				break;
			}
			else if (!block) {
				ret = false;

				break;
			}
			else {
				SDL_CondWait(cond, mutex);
			}
		}

		SDL_CondSignal(condExit);
		SDL_UnlockMutex(mutex);

		return ret;
	}
};
#endif