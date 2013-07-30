/*++

Module Name:

    Interrupt.c - Interrupt handling events for example driver.

Abstract:

   This file contains the interrupt callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"

///////////////////////////////////////////////////////////////////////////////
// SmplInterruptEvtEnable 
///////////////////////////////////////////////////////////////////////////////
NTSTATUS SmplInterruptEvtEnable(
  __in  WDFINTERRUPT Interrupt,
  __in  WDFDEVICE AssociatedDevice
)
{
    PDEVICE_CONTEXT pDeviceContext = DeviceGetContext(WdfInterruptGetDevice(Interrupt));
    WRITE_INTERRUPT_ENABLE( pDeviceContext->pPortAddress, IER_RECEIVER_FULL );
    return STATUS_SUCCESS;

} // end SmplInterruptEvtEnable

///////////////////////////////////////////////////////////////////////////////
// SmplInterruptEvtDisable 
///////////////////////////////////////////////////////////////////////////////
NTSTATUS SmplInterruptEvtDisable(
  __in  WDFINTERRUPT Interrupt,
  __in  WDFDEVICE AssociatedDevice
)
{
    PDEVICE_CONTEXT pDeviceContext = DeviceGetContext(WdfInterruptGetDevice(Interrupt));
    WRITE_INTERRUPT_ENABLE( pDeviceContext->pPortAddress, 0);
    return STATUS_SUCCESS;

} // end SmplInterruptEvtDisable

///////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routine (ISR) event callback function 
// processing on IRQL_DIRQL
///////////////////////////////////////////////////////////////////////////////
BOOLEAN SmplInterruptEvtIsr(
  __in  WDFINTERRUPT Interrupt,
  __in  ULONG MessageID
)
{
    WDFDEVICE Device = WdfInterruptGetDevice(Interrupt);
    PDEVICE_CONTEXT pSmplDeviceContext = DeviceGetContext(Device);
    BOOLEAN bReturnValue = FALSE;

#pragma warning(push)
#pragma warning(disable:4127) // warning C4127: conditional expression is constant
    while(TRUE)
#pragma warning(pop)
    {   // First check: Is it ours???
        UCHAR rcvd;
        UCHAR IntId = READ_INTERRUPT_IDENTIFICATION(pSmplDeviceContext->pPortAddress);
        if (IntId & IIR_NOT_FIRED)
        {
            break;
        }

        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"Interrupt\n");
        bReturnValue = TRUE;

        switch (IntId) 
        {
            case IIR_RECEIVER_FULL: 
                rcvd = READ_RECEIVER_BUFFER (pSmplDeviceContext->pPortAddress);
                break;
            case IIR_RECEIVER_ERROR:
            case IIR_TRANSMITTER_EMPTY:
            case IIR_STATUS_CHANGE:
            default:
                ;
        } // end switch

    } // end while

    if(TRUE == bReturnValue)
        WdfInterruptQueueDpcForIsr(Interrupt);

    return bReturnValue;

} // end SmplInterruptEvtIsr

///////////////////////////////////////////////////////////////////////////////
// Deferred procedure call (DPC) event callback function 
// for postprocessing after interrupt on IRQL_DISPATCH_LEVEL
///////////////////////////////////////////////////////////////////////////////
VOID
SmplInterruptEvtDpc(
    IN WDFINTERRUPT Interrupt,
    IN WDFOBJECT AssociatedObject
    )
{
    WDFDEVICE Device = WdfInterruptGetDevice(Interrupt);
    PDEVICE_CONTEXT pSmplDeviceContext = DeviceGetContext(Device);
    WDFREQUEST Request;
    NTSTATUS Status;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"SmplInterruptEvtDpc >>\n");

    Status = WdfIoQueueRetrieveNextRequest(pSmplDeviceContext->Queue, &Request);
    if(STATUS_SUCCESS == Status)
    {
        WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, 0);
    }

} // end SmplInterruptEvtDpc


