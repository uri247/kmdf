#include "Driver.h"
#include "Usb.tmh"

NTSTATUS SmplUsbIoTargetInitialize( _In_ WDFDEVICE Device )
{
	NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_CONTEXT pDeviceContext;
    WDF_USB_DEVICE_CREATE_CONFIG createParams;
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;
	WDFUSBINTERFACE intf;
	BOOLEAN hasIntr;

    pDeviceContext = DeviceGetContext(Device);

    if (pDeviceContext->UsbDevice == NULL)
	{
        //
        // Specifying a client contract version of 602 enables us to query for
        // and use the new capabilities of the USB driver stack for Windows 8.
        // It also implies that we conform to rules mentioned in MSDN
        // documentation for WdfUsbTargetDeviceCreateWithParameters.
        //
        WDF_USB_DEVICE_CREATE_CONFIG_INIT( &createParams, USBD_CLIENT_CONTRACT_VERSION_602 );

        status = WdfUsbTargetDeviceCreateWithParameters(
			Device,
			&createParams,
			WDF_NO_OBJECT_ATTRIBUTES,
			&pDeviceContext->UsbDevice
			);

        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfUsbTargetDeviceCreateWithParameters failed 0x%x", status);
            return status;
        }

        //
        // Select the first configuration of the device, using the first alternate
        // setting of each interface
        //
        WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_MULTIPLE_INTERFACES( &configParams, 0, NULL );
        status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->UsbDevice,
                                                WDF_NO_OBJECT_ATTRIBUTES,
                                                &configParams
                                                );

        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfUsbTargetDeviceSelectConfig failed 0x%x", status);
            return status;
        }

		intf = WdfUsbTargetDeviceGetInterface( pDeviceContext->UsbDevice, 0 );
		//SmplUsbIoTargetInterfaceTrace( intf );
		hasIntr = SmplUsbPipeTypeInterrupt( intf, pDeviceContext );

    }

    return status;
}

/*
void SmplUsbIoTargetInterfaceTrace( WDFUSBINTERFACE TargetInterface )
{
	BYTE numPipes;
	BYTE i;

	numPipes = WdfUsbInterfaceGetNumConfiguredPipes( TargetInterface );
	for( i=0; i<numPipes; ++i ) {
		WDF_USB_PIPE_INFORMATION pipeInfo;
		WdfUsbInterfaceGetConfiguredPipe( TargetInterface, i, &pipeInfo );
		DbgPrintEx( DPFLTR_IHVDRIVER_ID, 1234, "Pipe %d: max pack=%u, ep addr=%d, interval=%d", 
			i, pipeInfo.MaximumPacketSize, pipeInfo.EndpointAddress, pipeInfo.Interval );
	}
}
*/

BOOLEAN SmplUsbPipeTypeInterrupt( _In_ WDFUSBINTERFACE TargetInterface, _Out_ PDEVICE_CONTEXT pSmplDeviceContext )
{
	BYTE numPipes;
	BYTE i;

	numPipes = WdfUsbInterfaceGetNumConfiguredPipes( TargetInterface );
	for( i=0; i<numPipes; ++i ) {
		WDF_USB_PIPE_INFORMATION pipeInfo;
		WDFUSBPIPE pipe;
		
		pipe = WdfUsbInterfaceGetConfiguredPipe( TargetInterface, i, &pipeInfo );
		if( pipeInfo.PipeType == WdfUsbPipeTypeInterrupt ) {
			pSmplDeviceContext->UsbInterruptPipe = pipe;
			pSmplDeviceContext->UsbInterruptMaximumPacketSize = pipeInfo.MaximumPacketSize;
			pSmplDeviceContext->UsbSpecializedIoTarget = WdfUsbTargetPipeGetIoTarget( pipe );
			return TRUE;
		}
	}

	return FALSE;
}


