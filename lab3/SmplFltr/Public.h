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

DEFINE_GUID (GUID_DEVINTERFACE_SmplFltr,
    0xeb26be53,0xad6f,0x4e84,0x91,0xab,0xad,0x56,0xa5,0xb9,0x1c,0x7d);
// {eb26be53-ad6f-4e84-91ab-ad56a5b91c7d}
