/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#define INITGUID

#include <ntddk.h>
#include <wdf.h>
#include "Hw8250.h"

#include "device.h"
#include "queue.h"
#include "trace.h"

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD SmplDeviceEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP  SmplDeviceEvtDriverContextCleanup;
EVT_WDF_DEVICE_PREPARE_HARDWARE SmplDeviceEvtPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE SmplDeviceEvtReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY         SmplDeviceEvtD0Entry;
EVT_WDF_DEVICE_D0_EXIT          SmplDeviceEvtD0Exit;

//
// Interrupt.c
//
EVT_WDF_INTERRUPT_ENABLE SmplInterruptEvtEnable;
EVT_WDF_INTERRUPT_DISABLE SmplInterruptEvtDisable;
EVT_WDF_INTERRUPT_ISR SmplInterruptEvtIsr;