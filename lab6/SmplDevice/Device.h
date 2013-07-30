/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include "public.h"

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    ULONG PortAddressLength;
    PUCHAR pPortAddress;

    // Handle of registration with Power Framework (PoFx)
    POHANDLE PoHandle;

    // Power-managed queue used for the device.
    // Stoped/started on active/idle transitions.
    WDFQUEUE Queue;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
SmplDeviceCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DEVICE_PREPARE_HARDWARE SmplDeviceEvtPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE SmplDeviceEvtReleaseHardware;

EVT_WDF_DEVICE_D0_ENTRY SmplDeviceEvtD0Entry;
EVT_WDF_DEVICE_D0_EXIT SmplDeviceEvtD0Exit;

EVT_WDFDEVICE_WDM_POST_PO_FX_REGISTER_DEVICE SmplDeviceEvtWdmPostPoFxRegisterDevice;



