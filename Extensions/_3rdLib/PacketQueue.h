#pragma once

// https://blog.csdn.net/fengchu3415/article/details/82453808

#include <condition_variable>
#include <functional>
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>

extern "C" {
#include <libavformat/avformat.h>
}

class packetQueue {
private:
	std::queue<AVPacket> queue;
	int dataSize = 0;

	// Thread gubbins
	std::mutex _mutex;
	std::condition_variable _fullQue;
	std::condition_variable _empty;

	// Exit
	// Ô­×Ó²Ù×÷
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