#pragma once

template<typename TLock> struct AutoLock {
	AutoLock(TLock& lock) : _lock(lock) {
		// lock.Lock();
		_lock.Lock();
	}

	~AutoLock() {
		_lock.Unlock();
	}

private:
	TLock& _lock;
};