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

#include <ntddkbd.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, SmplFilterQueueInitialize)
#endif

NTSTATUS
SmplFilterQueueInitialize(
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
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE( &queueConfig,
                                            WdfIoQueueDispatchParallel);
    queueConfig.EvtIoDefault = SmplFilterEvtIoDefault;
    status = WdfIoQueueCreate(Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue);

    if( !NT_SUCCESS(status) ) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}

///////////////////////////////////////////////////////////////////////////////
// SmplFilterEvtIoDefault
///////////////////////////////////////////////////////////////////////////////
VOID SmplFilterEvtIoDefault(
  __in  WDFQUEUE Queue,
  __in  WDFREQUEST Request
)
{
    WDF_REQUEST_PARAMETERS RequestParameters;
    BOOLEAN bResult;

    WDF_REQUEST_PARAMETERS_INIT(&RequestParameters);
    WdfRequestGetParameters(Request, &RequestParameters);
    if(WdfRequestTypeRead == RequestParameters.Type)
    {
        DbgPrintEx( DPFLTR_IHVDRIVER_ID, 1234, "SmplFilterEvtIoDefault WdfRequestTypeRead\n");
        WdfRequestFormatRequestUsingCurrentType(Request);
        WdfRequestSetCompletionRoutine(Request, SmplFilterCompletionRoutineRead, NULL);
        bResult = WdfRequestSend(Request,
                                WdfDeviceGetIoTarget(WdfIoQueueGetDevice(Queue)),
                                NULL);
    }
    else
    {
        WDF_REQUEST_SEND_OPTIONS Options;
        WDF_REQUEST_SEND_OPTIONS_INIT(&Options, WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

        DbgPrintEx( DPFLTR_IHVDRIVER_ID, 1234, "SmplFilterEvtIoDefault other Request type\n");

        bResult = WdfRequestSend(Request,
                                WdfDeviceGetIoTarget(WdfIoQueueGetDevice(Queue)),
                                &Options);
    }

    if(FALSE == bResult)
    {
        DbgPrintEx( DPFLTR_IHVDRIVER_ID, 1234, "WdfRequestSend failed!\n");
        WdfRequestComplete(Request, WdfRequestGetStatus(Request));
    }
} // end SmplFilterEvtIoDefault

///////////////////////////////////////////////////////////////////////////////
// SmplFilterCompletionRoutineRead
///////////////////////////////////////////////////////////////////////////////
void SmplFilterCompletionRoutineRead(
  __in  WDFREQUEST Request,
  __in  WDFIOTARGET Target,
  __in  PWDF_REQUEST_COMPLETION_PARAMS Params,
  __in  WDFCONTEXT Context
)
{
    size_t CountByte;
    PKEYBOARD_INPUT_DATA pKeyboardInputData;
    NTSTATUS Status;

    DbgPrintEx( DPFLTR_IHVDRIVER_ID, 1234, "SmplFilterCompletionRoutineRead\n");

    Status = WdfRequestRetrieveOutputBuffer(Request,
                                            sizeof(KEYBOARD_INPUT_DATA),
                                            &pKeyboardInputData,
                                            &CountByte);
    if (NT_SUCCESS (Status)) 
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "Size of data: %d.\n", CountByte);
        if (CountByte < sizeof(KEYBOARD_INPUT_DATA))
        {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "Buffer too small.\n");
        }
        else
        {
            DbgPrintEx( DPFLTR_IHVDRIVER_ID, 
                        1234, 
                        "Unit: %x  MakCode: %x, Flags: %x  ExtraInformation %x.\n", 
                        pKeyboardInputData->UnitId, 
                        pKeyboardInputData->MakeCode, 
                        pKeyboardInputData->Flags, 
                        pKeyboardInputData->ExtraInformation);
        }
    }
    else
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "WdfRequestRetrieveOutputBuffer() failed.\n");
    }

    WdfRequestComplete(Request, WdfRequestGetStatus(Request));

} // end SmplFilterCompletionRoutineRead


