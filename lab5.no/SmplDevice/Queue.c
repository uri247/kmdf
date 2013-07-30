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
    WDFQUEUE queue;
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG    queueConfig;

    PAGED_CODE();
    
    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchParallel
        );

	queueConfig.EvtIoRead = SmplDeviceEvtIoRead;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue
                 );

    if( !NT_SUCCESS(status) ) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}


VOID
SmplDeviceEvtIoRead(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t Length
)
{
    WDFMEMORY OutputMemory;
    NTSTATUS Status;
	UNREFERENCED_PARAMETER(Queue);
	UNREFERENCED_PARAMETER(Length);

    Status = WdfRequestRetrieveOutputMemory(Request, &OutputMemory);
    if(!NT_SUCCESS(Status))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "WdfRequestRetrieveOutputMemory failed! 0x%x\n", Status);
    }
    else
    {
        Status = WdfMemoryCopyFromBuffer(OutputMemory, 0, "Hello World", 12);
        if(!NT_SUCCESS(Status))
        {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "WdfMemoryCopyFromBuffer failed! 0x%x\n", Status);
        }
    }

    WdfRequestCompleteWithInformation(Request, Status, 12);
}

