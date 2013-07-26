/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#define INITGUID

#pragma warning (disable:4100) // unreferenced formal parameter

#include <ntddk.h>
#include <wdf.h>

#include "device.h"
#include "queue.h"
#include "trace.h"

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD SmplFilterEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP SmplFilterEvtDriverContextCleanup;

