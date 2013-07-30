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
    WDF_OBJECT_ATTRIBUTES Attributes;
    PDEVICE_CONTEXT pDeviceContext = DeviceGetContext(Device);

    PAGED_CODE();

    WDF_OBJECT_ATTRIBUTES_INIT(&Attributes);
    Attributes.SynchronizationScope = WdfSynchronizationScopeInheritFromParent;

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchManual);

    status = WdfIoQueueCreate(  Device,
                                &queueConfig,
                                &Attributes,
                                &(pDeviceContext->Queue));
    if( !NT_SUCCESS(status) ) 
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}

///////////////////////////////////////////////////////////////////////////////
// SmplIoQueueRequestTryComplete
// This function needs the IoQueue lock held before calling
///////////////////////////////////////////////////////////////////////////////
BOOLEAN
SmplIoQueueRequestTryComplete(
    IN PDEVICE_CONTEXT pSmplDeviceContext
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    WDFREQUEST ReadRequest = NULL;
    PUCHAR pReadRequestBuffer;
    size_t CountByteRequestBuffer = 0;
    size_t CountByteCircularBuffer = 0;
    ULONG i;

    if (NULL == pSmplDeviceContext)
        return FALSE;

    WdfInterruptAcquireLock(pSmplDeviceContext->Interrupt);
    CountByteCircularBuffer = RingbufferGetCountCharacters(pSmplDeviceContext->pRingBuffer);
    WdfInterruptReleaseLock(pSmplDeviceContext->Interrupt);

    if (0 == CountByteCircularBuffer)
    {   // no data in circular buffer
        return FALSE;
    }

    Status = WdfIoQueueRetrieveNextRequest(pSmplDeviceContext->Queue, &ReadRequest);
    if (!NT_SUCCESS(Status) || (NULL == ReadRequest))
    {   // no request in the queue
        return FALSE;
    }

    Status = WdfRequestRetrieveOutputBuffer(ReadRequest,
                                            0,
                                            &pReadRequestBuffer,
                                            &CountByteRequestBuffer);
    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"WdfRequestRetrieveOutputMemory failed! THIS SHOULD NEVER HAPPEN\n");
        return FALSE;
    }

    WdfInterruptAcquireLock(pSmplDeviceContext->Interrupt);
    for(i = 0; i < CountByteRequestBuffer; i++)
    {
        BOOLEAN bStatus = RingbufferRemoveCharacter(pSmplDeviceContext->pRingBuffer, pReadRequestBuffer + i);
        if(FALSE == bStatus)
            break;
    }
    WdfInterruptReleaseLock(pSmplDeviceContext->Interrupt);

    WdfRequestCompleteWithInformation(ReadRequest, Status, i); 
    return TRUE;

} // end SmplIoQueueRequestTryComplete

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

