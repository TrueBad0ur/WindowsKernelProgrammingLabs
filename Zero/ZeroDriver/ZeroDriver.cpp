#include "pch.h"
#include "ZeroDriverCommon.h"

#define DRIVER_PREFIX "Zero: "

// prototypes

void ZeroDriverUnload(PDRIVER_OBJECT DriverObject);
DRIVER_DISPATCH ZeroDriverCreateClose, ZeroDriverRead, ZeroDriverWrite, ZeroDeviceControl;

// globals

long long g_TotalRead, g_TotalWritten;

// DriverEntry

extern "C" NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = ZeroDriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = ZeroDriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = ZeroDriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_READ] = ZeroDriverRead;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = ZeroDriverWrite;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ZeroDeviceControl;

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\ZeroDriver");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ZeroDriver");
	PDEVICE_OBJECT DeviceObject = nullptr;
	auto status = STATUS_SUCCESS;
	auto symLinkCreated = false;

	do {
		status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
		if (!NT_SUCCESS(status)) {
			KdPrint((DRIVER_PREFIX "failed to create device (0x%08X)\n", status));
			break;
		}
		DeviceObject->Flags |= DO_DIRECT_IO;

		status = IoCreateSymbolicLink(&symLink, &devName);
		if (!NT_SUCCESS(status)) {
			KdPrint((DRIVER_PREFIX "failed to create symbolic link (0x%08X)\n", status));
			break;
		}
		symLinkCreated = true;

	} while (false);

	if (!NT_SUCCESS(status)) {
		if (DeviceObject)
			IoDeleteDevice(DeviceObject);
	}

	return status;
}

// implementation

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status = STATUS_SUCCESS, ULONG_PTR info = 0) {
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, 0);
	return status;
}

void ZeroDriverUnload(PDRIVER_OBJECT DriverObject) {
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ZeroDriver");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS ZeroDriverCreateClose(PDEVICE_OBJECT, PIRP Irp) {
	return CompleteIrp(Irp);
}

NTSTATUS ZeroDriverRead(PDEVICE_OBJECT, PIRP Irp) {
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto len = stack->Parameters.Read.Length;
	if (len == 0)
		return CompleteIrp(Irp, STATUS_INVALID_BUFFER_SIZE);

	auto buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (!buffer)
		return CompleteIrp(Irp, STATUS_INSUFFICIENT_RESOURCES);

	memset(buffer, 0, len);
	InterlockedAdd64(&g_TotalRead, len);

	return CompleteIrp(Irp, STATUS_SUCCESS, len);
}

NTSTATUS ZeroDriverWrite(PDEVICE_OBJECT, PIRP Irp) {
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto len = stack->Parameters.Write.Length;

	InterlockedAdd64(&g_TotalWritten, len);

	return CompleteIrp(Irp, STATUS_SUCCESS, len);
}

NTSTATUS ZeroDeviceControl(PDEVICE_OBJECT, PIRP Irp) {
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto& dic = stack->Parameters.DeviceIoControl;

	if (dic.IoControlCode != IOCTL_ZERO_GET_STATS)
		return CompleteIrp(Irp, STATUS_INVALID_DEVICE_REQUEST);

	//if (dic.InputBufferLength < sizeof(ZeroStats))
	//	return CompleteIrp(Irp, STATUS_BUFFER_TOO_SMALL);

	auto stats = (ZeroStats*)Irp->AssociatedIrp.SystemBuffer;
	stats->TotalRead = g_TotalRead;
	stats->TotalWritten = g_TotalWritten;

	return CompleteIrp(Irp, STATUS_SUCCESS, sizeof(ZeroStats));
}