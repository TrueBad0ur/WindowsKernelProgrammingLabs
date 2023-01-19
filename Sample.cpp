#include <ntddk.h>

void UnloadRoutine(_In_ PDRIVER_OBJECT DriverObject) {
	KdPrint(("Sample driver Unload called\n"));

	UNREFERENCED_PARAMETER(DriverObject);
}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = UnloadRoutine;

	RTL_OSVERSIONINFOEXW infoW;
	if (RtlGetVersion((PRTL_OSVERSIONINFOW) & infoW) != STATUS_SUCCESS) return FALSE;

	KdPrint(("dwMajorVersion: %d\t dwMinorVersion: %d\t dwBuildNumber: %d\t dwPlatformId: %d", infoW.dwMajorVersion, 
		infoW.dwMinorVersion, infoW.dwBuildNumber, infoW.dwPlatformId));

	return STATUS_SUCCESS;
}