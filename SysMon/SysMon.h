#pragma once

#include "FastMutex.h"

#define DRIVER_PREFIX "SysMon: "
#define DRIVER_TAG 'msys'

struct Globals {
	LIST_ENTRY ItemsHead;
	int ItemCount;
	FastMutex Mutex;
};

template<typename T>
struct FullItem {
	LIST_ENTRY Entry;
	T Data;
};
