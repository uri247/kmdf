///////////////////////////////////////////////////////////////////////////////
// File:          PoFx.c
//
// Description:   PoFx single component power management functionality 
//                for the simple sample SMPL device driver.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright © datronicsoft.  2012
///////////////////////////////////////////////////////////////////////////////
#include "driver.h"
#include "PoFx.tmh"

///////////////////////////////////////////////////////////////////////////////
// Initialization of component level power management.
///////////////////////////////////////////////////////////////////////////////
VOID
SmplPoFxSingleComponentInitialize(
    _In_ WDFDEVICE Device
    )
{
    WDF_POWER_FRAMEWORK_SETTINGS Settings;
    PO_FX_COMPONENT Component;
    PO_FX_COMPONENT_IDLE_STATE IdleStates[FSTATE_COUNT];
    NTSTATUS Status;

    //
    // Initialization
    //
    RtlZeroMemory(&Component, sizeof(Component));
    RtlZeroMemory(&IdleStates, sizeof(IdleStates));

    //
    // The transition latency and residency requirement values used here are for
    // illustration purposes only. The driver should use values that are 
    // appropriate for its device.
    //

    //
    // F0
    //
    IdleStates[0].TransitionLatency = 0;    
    IdleStates[0].ResidencyRequirement = 0;    
    IdleStates[0].NominalPower = 0;    

    //
    // F1
    //
    IdleStates[1].TransitionLatency = WDF_ABS_TIMEOUT_IN_MS(200);
    IdleStates[1].ResidencyRequirement = WDF_ABS_TIMEOUT_IN_SEC(3);   
    IdleStates[1].NominalPower = 0;    

    //
    // F2
    //
    IdleStates[2].TransitionLatency = WDF_ABS_TIMEOUT_IN_MS(400);    
    IdleStates[2].ResidencyRequirement = WDF_ABS_TIMEOUT_IN_SEC(6);    
    IdleStates[2].NominalPower = 0;    

    //
    // F3
    //
    IdleStates[3].TransitionLatency = WDF_ABS_TIMEOUT_IN_MS(DEEPEST_FSTATE_LATENCY_IN_MS); 
    IdleStates[3].ResidencyRequirement = WDF_ABS_TIMEOUT_IN_SEC(DEEPEST_FSTATE_RESIDENCY_IN_SEC);
    IdleStates[3].NominalPower = 0;

    //
    // Component 0 (the only component)
    //
    Component.IdleStateCount = FSTATE_COUNT;
    Component.IdleStates = IdleStates;

    WDF_POWER_FRAMEWORK_SETTINGS_INIT(&Settings);

    Settings.EvtDeviceWdmPostPoFxRegisterDevice = SmplDeviceEvtWdmPostPoFxRegisterDevice;

    Settings.Component = &Component;
    Settings.ComponentActiveConditionCallback = SmplPoFxComponentActiveConditionCallback;
    Settings.ComponentIdleConditionCallback =   SmplPoFxComponentIdleConditionCallback;
    Settings.ComponentIdleStateCallback = SmplPoFxComponentIdleStateCallback;
    Settings.PoFxDeviceContext = (PVOID) Device;



    Status = WdfDeviceWdmAssignPowerFrameworkSettings(Device, &Settings);
    if (FALSE == NT_SUCCESS(Status)) 
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"WdfDeviceWdmAssignPowerFrameworkSettings failed! 0x%x\n", Status);
    }

} // end SmplPoFxSingleComponentInitialize

///////////////////////////////////////////////////////////////////////////////
// This callback is invoked by Power Framework to notify driver about any
//  F-state transition.
///////////////////////////////////////////////////////////////////////////////
VOID
SmplPoFxComponentIdleStateCallback(
    _In_ PVOID Context,
    _In_ ULONG Component,
    _In_ ULONG State
    )
{
    DEVICE_CONTEXT *pDeviceContext = DeviceGetContext((WDFDEVICE)Context);

    //
    // Note the new F-state. PEP may direct to any of the F-states directly.
    // Transition to any Fx state happens from F0 (and not another Fx state).
    //
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"SmplPoFxComponentIdleStateCallback Component:%d F%d\n", Component, State);
    PoFxCompleteIdleState(pDeviceContext->PoHandle, Component); 

} // end SmplPoFxComponentIdleStateCallback

///////////////////////////////////////////////////////////////////////////////
// SmplPoFxComponentActiveConditionCallback
// This callback is invoked by Power Framework to notify driver that one of its
// components has become active.
///////////////////////////////////////////////////////////////////////////////
VOID
SmplPoFxComponentActiveConditionCallback(
    _In_ PVOID Context,
    _In_ ULONG Component
    )
{
    DEVICE_CONTEXT *pDeviceContext = DeviceGetContext((WDFDEVICE) Context);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"SmplPoFxComponentActiveConditionCallback Component:%d\n", Component);
    WdfIoQueueStart(pDeviceContext->Queue);

} // end SmplPoFxComponentActiveConditionCallback

///////////////////////////////////////////////////////////////////////////////
// SmplPoFxComponentIdleConditionCallback
//
// IMPORTANT NOTE
// ==============
// Given that the idle transition does not complete until all the power-
// managed queues have been stopped, it is extremely important for the 
// driver to ensure that stopping of power-managed queues is reasonably 
// quick. If the driver fails to ensure this, the power framework can remain
// stuck in the idle transition for a long time, which could hamper its 
// ability to put the component in a low-power F-state. This could 
// negatively impact any power savings that can be gained via component 
// power management.
//
// In order to ensure that idle transitions can complete quickly, the driver
// should quickly process any requests that are dispatched to it via a 
// power-managed queue. If the driver forwards a request (that was received 
// via a power-managed queue) to an I/O target that might keep the request 
// pending for a long time, then the driver should cancel the request when
// the component idle condition callback is invoked. This is because the 
// power-managed queue cannot be stopped while the request is pending in the
// I/O target.
//
///////////////////////////////////////////////////////////////////////////////
VOID
SmplPoFxComponentIdleConditionCallback(
   _In_ PVOID Context,
   _In_ ULONG Component
   )
{
    DEVICE_CONTEXT *pDeviceContext = DeviceGetContext((WDFDEVICE) Context);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"SmplPoFxComponentIdleConditionCallback Component:%d\n", Component);
    WdfIoQueueStop(pDeviceContext->Queue, SmplQueueEvtStatePoFxStopComplete, NULL);
    
    //
    // The idle transition will complete asynchronously. We'll call 
    // PoFxCompleteIdleCondition to complete it when all the queues have been
    // stopped.

} // end SmplPoFxComponentIdleConditionCallback

