#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

template <typename T>
class thdq
{
public:
	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty()) { cond_.wait(mlock); }
		auto item = std::move(queue_.front());
		queue_.pop();
		return item;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty()) { cond_.wait(mlock); }
		item = queue_.front();
		queue_.pop();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

	void push(T&& item)
	{
		try {
			std::unique_lock<std::mutex> mlock(mutex_);
			queue_.push(std::move(item));
			mlock.unlock();
			cond_.notify_one();
		}
		catch (...) {
			std::cout << "Error pushing an miner event" << std::endl;
		}
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};
