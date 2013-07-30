#include "driver.h"
#include "interrupt.tmh"


EVT_WDF_INTERRUPT_ENABLE SmplInterruptEvtEnable;
EVT_WDF_INTERRUPT_DISABLE SmplInterruptEvtDisable;
EVT_WDF_INTERRUPT_ISR SmplInterruptEvtIsr;

NTSTATUS SmplInterruptEvtEnable(
  _In_  WDFINTERRUPT Interrupt,
  _In_  WDFDEVICE AssociatedDevice
)
{
    PDEVICE_CONTEXT pSmplDeviceContext = DeviceGetContext(AssociatedDevice);
    WRITE_INTERRUPT_ENABLE(pSmplDeviceContext->pPortAddress, IER_RECEIVER_FULL );
    return STATUS_SUCCESS;
}

NTSTATUS SmplInterruptEvtDisable(
  _In_  WDFINTERRUPT Interrupt,
  _In_  WDFDEVICE AssociatedDevice
)
{
    PDEVICE_CONTEXT pSmplDeviceContext = DeviceGetContext(AssociatedDevice);
    WRITE_INTERRUPT_ENABLE(pSmplDeviceContext->pPortAddress, 0 );
    return STATUS_SUCCESS;
}

BOOLEAN SmplInterruptEvtIsr(
    _In_  WDFINTERRUPT Interrupt,
    _In_  ULONG MessageID
    )
{
    WDFDEVICE Device = WdfInterruptGetDevice(Interrupt);
    PDEVICE_CONTEXT pSmplDeviceContext = DeviceGetContext(Device);
    //UCHAR IntId;
    BOOLEAN bReturnValue = FALSE;
    
#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant
    while(TRUE)
#pragma warning(pop)
    { // First check: Is it ours???
        UCHAR rcvd;
        UCHAR IntId = READ_INTERRUPT_IDENTIFICATION(pSmplDeviceContext->pPortAddress);
        if (IntId & IIR_NOT_FIRED)
        {
            break;
        }

        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"Interrupt\n");
        bReturnValue = TRUE;

        if (IntId == IIR_RECEIVER_FULL)
        {
            //UCHAR rcvd;
            rcvd = READ_RECEIVER_BUFFER(pSmplDeviceContext->pPortAddress);
        }
    }

    return bReturnValue;
}