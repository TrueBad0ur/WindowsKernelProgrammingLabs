#pragma once

#include "FastMutex.h"

#define DRIVER_PREFIX "ProcessProtector: "
#define PROCESS_TERMINATE 1

const int MaxPids = 256;

DRIVER_UNLOAD ProcessProtectorUnload;
DRIVER_DISPATCH ProcessProtectorCreateClose, ProcessProtectorDeviceControl;
OB_PREOP_CALLBACK_STATUS OnPreOpenProcess(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION Info);

bool FindProcess(ULONG pid);
bool AddProcess(ULONG pid);
bool RemoveProcess(ULONG pid);

typedef struct _Globals {
	int			PidsCount;		// Currently protected process count
	ULONG		Pids[MaxPids];	// protected PIDs
	FastMutex	Lock;
	PVOID		RegHandle;		// object registration cookie

	void Init()
	{
		Lock.Init();
	}
} Globals, * PGlobals;