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
DEFINE_GUID(GUID_DEVINTERFACE_SMPLDEVICE, 
0xd0c941c5, 0x51b0, 0x4d90, 0x82, 0x78, 0xfb, 0x17, 0xdf, 0x5, 0xfd, 0x65);
// {D0C941C5-51B0-4d90-8278-FB17DF05FD65}
