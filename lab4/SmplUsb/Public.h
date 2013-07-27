/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_SmplUsb,
    0xfdff02c0,0x4b8b,0x4ff7,0xa1,0xfc,0x0d,0x81,0x42,0xc9,0x8d,0x18);
// {fdff02c0-4b8b-4ff7-a1fc-0d8142c98d18}
