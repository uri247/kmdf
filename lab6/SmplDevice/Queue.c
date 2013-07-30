/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "queue.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, SmplDeviceQueueInitialize)
#endif

NTSTATUS
SmplDeviceQueueInitialize(
    _In_ WDFDEVICE Device
    )
/*++

Routine Description:


     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

     A single default I/O Queue is configured for parallel request
     processing, and a driver context memory allocation is created
     to hold our structure QUEUE_CONTEXT.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    VOID

--*/
{
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;
    PDEVICE_CONTEXT pDeviceContext = DeviceGetContext(Device);

    PAGED_CODE();
    
    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchManual);

    status = WdfIoQueueCreate(  Device,
                                &queueConfig,
                                WDF_NO_OBJECT_ATTRIBUTES,
                                &(pDeviceContext->Queue));
    if( !NT_SUCCESS(status) ) 
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}

///////////////////////////////////////////////////////////////////////////////
// SmplQueueEvtStatePoFxStopComplete
// Callback invoked by KMDF when stop transiton of the supplied queue has
// completed.
///////////////////////////////////////////////////////////////////////////////
VOID
SmplQueueEvtStatePoFxStopComplete(
    _In_ WDFQUEUE Queue,
    _In_ WDFCONTEXT Context
    )
{
    DEVICE_CONTEXT *pDeviceContext = WdfObjectGetTypedContext(WdfIoQueueGetDevice(Queue), DEVICE_CONTEXT);

    UNREFERENCED_PARAMETER(Context);
    
    //
    // Queue has been stopped. Complete the idle transition.
    //
    PoFxCompleteIdleCondition(pDeviceContext->PoHandle, 0 /* Component */);

} // end SmplQueueEvtStatePoFxStopComplete


