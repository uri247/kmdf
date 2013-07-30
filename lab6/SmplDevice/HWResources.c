/*++

Module Name:

    HWResources.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"

///////////////////////////////////////////////////////////////////////////////
// Helper function that parses the HAL translated CM_RESOURCE_LIST 
// and extracts the data needed for the port address.
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
SmplDeviceHwResourcesGet(
    IN WDFDEVICE Device,
    IN WDFCMRESLIST ResourcesTranslated
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
        for (index = 0; index < WdfCmResourceListGetCount(ResourcesTranslated); index++)
        {
            pCMResourceDescPartial = WdfCmResourceListGetDescriptor(ResourcesTranslated, index);
            
            if (NULL == pCMResourceDescPartial)
            {
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"pCMResourceDescPartial is NULL!!!\n");
                return STATUS_DEVICE_CONFIGURATION_ERROR;
            }

            switch (pCMResourceDescPartial->Type) 
            {
                case CmResourceTypeMemory:
                    //
                    // Handle memory resources here.
                    //
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"CmResourceTypeMemory Desc\n");
                    break;

                case CmResourceTypePort:
                    //
                    // Handle port resources here.
                    //
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"CmResourceTypePort 0x%x %d\n", pCMResourceDescPartial->u.Port.Start.LowPart, pCMResourceDescPartial->u.Port.Length);
                    pSmplDeviceContext->pPortAddress = (PUCHAR) pCMResourceDescPartial->u.Port.Start.LowPart;
                    pSmplDeviceContext->PortAddressLength = pCMResourceDescPartial->u.Port.Length;
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


