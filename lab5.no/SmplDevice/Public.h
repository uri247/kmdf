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

DEFINE_GUID (GUID_DEVINTERFACE_SmplDevice,
    0xb4a17322,0x2f5c,0x41d3,0x89,0xb5,0xe8,0x1b,0x5d,0x56,0xa3,0x4b);
// {b4a17322-2f5c-41d3-89b5-e81b5d56a34b}
