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

/*
DEFINE_GUID (GUID_DEVINTERFACE_SmplDevice,
    0x19e8432f,0xec78,0x4df0,0xbe,0xe6,0xfe,0x3d,0xfd,0xdb,0x20,0xb6);
	*/
// {19e8432f-ec78-4df0-bee6-fe3dfddb20b6}
DEFINE_GUID(GUID_DEVINTERFACE_SMPLDEVICE, 
    0xd0c941c5, 0x51b0, 0x4d90, 0x82, 0x78, 0xfb, 0x17, 0xdf, 0x5, 0xfd, 0x65);