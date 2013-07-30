/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#define INITGUID

#pragma warning(disable:4100) // unreferenced formal parameter

#include <ntddk.h>
#include <wdf.h>

#include "Ringbuffer.h"
#include "HW8250.h"
#include "device.h"
#include "queue.h"
#include "trace.h"

//
// PoFx defines
//
#define FSTATE_COUNT                    4
#define DEEPEST_FSTATE_LATENCY_IN_MS    800
#define DEEPEST_FSTATE_RESIDENCY_IN_SEC 12

//
// PoFx Callbacks
//
PO_FX_COMPONENT_IDLE_STATE_CALLBACK SmplPoFxComponentIdleStateCallback;
PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK SmplPoFxComponentActiveConditionCallback;
PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK SmplPoFxComponentIdleConditionCallback;

VOID SmplPoFxSingleComponentInitialize(_In_ WDFDEVICE Device);

//
// WDFDRIVER Events
//
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD SmplDeviceEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP SmplDeviceEvtDriverContextCleanup;

//
// WDFINTERRUPT Events
//
EVT_WDF_INTERRUPT_ENABLE SmplInterruptEvtEnable;
EVT_WDF_INTERRUPT_DISABLE SmplInterruptEvtDisable;
EVT_WDF_INTERRUPT_ISR SmplInterruptEvtIsr;
EVT_WDF_INTERRUPT_DPC SmplInterruptEvtDpc;

NTSTATUS
SmplDeviceHwResourcesGet(
    IN WDFDEVICE Device,
    IN WDFCMRESLIST ResourcesTranslated
    );

