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
    0xe3fdaff3,0x2db5,0x49f7,0xb5,0xb8,0xe4,0x16,0x3c,0xfc,0xb0,0x6c);
// {e3fdaff3-2db5-49f7-b5b8-e4163cfcb06c}
