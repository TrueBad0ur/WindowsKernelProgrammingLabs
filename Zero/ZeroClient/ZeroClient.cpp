#include "pch.h"
#include "ZeroClientCommon.h"

int Error(const char* msg) {
	printf("%s: error=%d\n", msg, ::GetLastError());
	return 1;
}

int main() {
	HANDLE hDevice = ::CreateFile(L"\\\\.\\ZeroDriver", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return Error("failed to open device");
	}

	BYTE buffer[64];

	for (int i = 0; i < sizeof(buffer); ++i)
		buffer[i] = i + 1;

	DWORD bytes;
	BOOL ok = ::ReadFile(hDevice, buffer, sizeof(buffer), &bytes, nullptr);
	if (!ok)
		return Error("failed to read");

	if (bytes != sizeof(buffer))
		printf("Wrong number of bytes\n");

	long total = 0;
	for (auto n : buffer)
		total += n;

	if (total != 0)
		printf("Wrong data\n");

	BYTE buffer2[1024];

	ok = ::WriteFile(hDevice, buffer2, sizeof(buffer2), &bytes, nullptr);

	if (!ok)
		return Error("failed to write");

	if (bytes != sizeof(buffer2))
		printf("Wrong byte count\n");

	ZeroStats stats = {0};
	ZeroStats statsOut = {0};

	ok = ::DeviceIoControl(hDevice, IOCTL_ZERO_GET_STATS, &stats, sizeof(stats), &statsOut, sizeof(statsOut), &bytes, nullptr);
	if (!ok)
		return Error("failed to write");

	if (bytes != sizeof(statsOut))
		printf("Wrong byte count\n");

	printf("stats.TotalRead: %lld\nstats.TotalWritten: %lld\n", statsOut.TotalRead, statsOut.TotalWritten);

	::CloseHandle(hDevice);
}