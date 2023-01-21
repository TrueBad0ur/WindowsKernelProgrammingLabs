#include <windows.h>
#include <stdio.h>
#include "..\BoosterDriver\BoosterCommon.h"

int Error(const char* message) {
	printf("%s (error=%d)\n", message, GetLastError());
	return 1;
}

int main(int argc, const char* argv[]) {
	if (argc < 4) {
		printf("Usage: Booster <threadid> <basepriority> <priority>\n");
		return 0;
	}

	HANDLE hDevice = CreateFile(L"\\\\.\\Booster", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE)
		return Error("Failed to open device");

	ThreadData data;
	data.ThreadId = atoi(argv[1]);
	data.BasePriority = atoi(argv[2]);
	data.Priority = atoi(argv[3]);

	DWORD returned;
	BOOL success = DeviceIoControl(hDevice, IOCTL_BOOSTER_SET_PRIORITY, &data, sizeof(data), nullptr, 0, &returned, nullptr);
	if (success)
		printf("Priority change succeeded!\n");
	else
		Error("Priority change failed!");
	CloseHandle(hDevice);

	return 0;
}