#include <ntddk.h>

// Simple driver to show direct I/O in Irp->AssociatedIrp.SystemBuffer

#pragma warning( disable : 4996 ) 

#define DRIVER_PREFIX "buffer Driver: "
#define IOCTL_BUFFER_GET_INPUT CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

void bufferUnload(PDRIVER_OBJECT DriverObject);
DRIVER_DISPATCH bufferCreateClose, bufferDeviceControl;

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = bufferUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = bufferCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = bufferCreateClose;
	DriverObject->MajorFunction[IRP_MJ_READ] = bufferCreateClose;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = bufferCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = bufferDeviceControl;

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\buffer");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\buffer");

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

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status = STATUS_SUCCESS, ULONG_PTR info = 0) {
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, 0);
	return status;
}

void bufferUnload(PDRIVER_OBJECT DriverObject) {
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\buffer");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS bufferCreateClose(PDEVICE_OBJECT, PIRP Irp) {
	return CompleteIrp(Irp);
}

NTSTATUS bufferDeviceControl(PDEVICE_OBJECT, PIRP Irp) {
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto& dic = stack->Parameters.DeviceIoControl;

	if (dic.IoControlCode != IOCTL_BUFFER_GET_INPUT)
		return CompleteIrp(Irp, STATUS_INVALID_DEVICE_REQUEST);

	auto data = (ULONG*)Irp->AssociatedIrp.SystemBuffer;
	DbgPrint("Data: %lu", *data);

	return CompleteIrp(Irp, STATUS_SUCCESS, sizeof(data));
}