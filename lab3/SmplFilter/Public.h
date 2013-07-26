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

DEFINE_GUID (GUID_DEVINTERFACE_SmplFilter,
    0x9ebfbcc9,0x96ed,0x42c3,0xb7,0x55,0xbd,0x24,0xbe,0xf5,0x4f,0xcc);
// {9ebfbcc9-96ed-42c3-b755-bd24bef54fcc}
