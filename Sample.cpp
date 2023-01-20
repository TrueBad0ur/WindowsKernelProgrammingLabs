#include <ntddk.h>

#pragma warning( disable : 4996 ) 

#define DRIVER_TAG 'Tvrd' // drvT
UNICODE_STRING g_RegistryPath;

void UnloadRoutine(_In_ PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);

	ExFreePool(g_RegistryPath.Buffer);
	KdPrint(("Driver unloaded!\n"));
}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

	// Setup part
	UNREFERENCED_PARAMETER(DriverObject);

	DriverObject->DriverUnload = UnloadRoutine;

	// Print version about OS
	RTL_OSVERSIONINFOEXW infoW;
	//if (RtlGetVersion((PRTL_OSVERSIONINFOW)&infoW) != STATUS_SUCCESS) return FALSE;
	RtlGetVersion((PRTL_OSVERSIONINFOW)&infoW);

	KdPrint(("dwMajorVersion: %d\t dwMinorVersion: %d\t dwBuildNumber: %d\t dwPlatformId: %d", infoW.dwMajorVersion,
		infoW.dwMinorVersion, infoW.dwBuildNumber, infoW.dwPlatformId));

	// Copy regsitry Path
	g_RegistryPath.Buffer = (WCHAR*)ExAllocatePoolWithTag(PagedPool, RegistryPath->Length, DRIVER_TAG);
	if (g_RegistryPath.Buffer == nullptr) {
		KdPrint(("Failed to allocate memory\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	g_RegistryPath.MaximumLength = RegistryPath->Length;
	RtlCopyUnicodeString(&g_RegistryPath, (PCUNICODE_STRING)RegistryPath);

	KdPrint(("Copied registry path: %wZ\n", &g_RegistryPath));

	return STATUS_SUCCESS;
}
