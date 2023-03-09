#include <Windows.h>
#include <stdio.h>

#define IOCTL_BUFFER_GET_INPUT CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

int Error(const char* msg) {
	printf("%s: error=%d\n", msg, ::GetLastError());
	return 1;
}

int main() {
	HANDLE hDevice = ::CreateFile(L"\\\\.\\buffer", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return Error("failed to open device");
	}

	DWORD data = 0x1337;

	BOOL ok;
	ok = ::DeviceIoControl(hDevice, IOCTL_BUFFER_GET_INPUT, &data, sizeof(data), nullptr, NULL, nullptr, nullptr);
	if (!ok)
		return Error("failed to write");

	::CloseHandle(hDevice);
	return 0;
}