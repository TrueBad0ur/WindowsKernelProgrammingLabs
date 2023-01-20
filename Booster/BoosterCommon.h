#pragma once
#include <ntddk.h>

#define BOOSTER_DEVICE 0x8000
#define IOCTL_BOOSTER_SET_PRIORITY CTL_CODE(BOOSTER_DEVICE, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

struct ThreadData {
	ULONG ThreadId;
	int Priority;
};