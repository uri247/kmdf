/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"

///////////////////////////////////////////////////////////////////////////////
// Private local helper function that parses the translated resource list 
// and extracts the data needed for the port address.
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
SmplDeviceHwResourcesGet(
    IN WDFDEVICE Device,
    IN WDFCMRESLIST ResourceListTranslated
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    ULONG index = 0;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR  pCMResourceDescPartial;
    PDEVICE_CONTEXT pSmplDeviceContext = NULL;

    pSmplDeviceContext = DeviceGetContext(Device);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"SmplDeviceHwResourcesGet >>\n");

    if (NULL != pSmplDeviceContext)
    {
        for (index = 0; index < WdfCmResourceListGetCount(ResourceListTranslated); index++)
        {
            pCMResourceDescPartial = WdfCmResourceListGetDescriptor(ResourceListTranslated, index);
            
            if (NULL == pCMResourceDescPartial)
            {
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"pCMResourceDescPartial is NULL!!!\n");
                return STATUS_DEVICE_CONFIGURATION_ERROR;
            }

            switch (pCMResourceDescPartial->Type) 
            {
                case CmResourceTypePort:
                    //
                    // Handle port resources here.
                    //
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"CmResourceTypePort 0x%x %d\n", pCMResourceDescPartial->u.Port.Start.LowPart, pCMResourceDescPartial->u.Port.Length);
                    pSmplDeviceContext->pPortAddress = (PUCHAR) pCMResourceDescPartial->u.Port.Start.LowPart;
                    pSmplDeviceContext->PortAddressLength = pCMResourceDescPartial->u.Port.Length;
                    break;

                case CmResourceTypeMemory:
                    //
                    // Handle memory resources here.
                    //
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"CmResourceTypeMemory Desc\n");
                    break;

                case CmResourceTypeInterrupt:
                    //
                    // Handle interrupt resources here.
                    //
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"CmResourceTypeInterrupt\n");
                    break;
                default:
                    //
                    // Ignore all other descriptors.
                    //
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"CmResourceType unknown\n");
                    break;
            } // end switch
        } // end for loop
    } // end if

    return Status;

} // end SmplDeviceHwResourcesGet


