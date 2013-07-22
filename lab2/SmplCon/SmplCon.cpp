
#include "stdafx.h"

#include <initguid.h>
#include "..\SmplDevice\Public.h"

using namespace std;

bool SetupApiDeviceInterfaceInstanceEnumerate(GUID* pGuid, DWORD instance, wstring& rSymbolicLinkName);
void DeviceRead(wstring SymbolicLinkName);


int main( int argc, wchar_t* argv[] )
{
    DWORD instance = 0;
    wstring SymbolicLinkName;
    
    // Retrieve the symbolic link via Device Interface
    if (!SetupApiDeviceInterfaceInstanceEnumerate((GUID*)&GUID_DEVINTERFACE_SmplDevice, instance, SymbolicLinkName))
    {
        cout << "Failed to access the Device Interface\n";
        system("pause");
        return 0;
    }	

    wcout << "\nSymbolicLinkName: " << SymbolicLinkName << endl;
        
    DeviceRead(SymbolicLinkName);

    system("pause");
    return 0;
} // end _tmain



bool SetupApiDeviceInterfaceInstanceEnumerate(GUID* pGuid, DWORD instance, wstring& rSymbolicLinkName)
{
    HDEVINFO hDevInfo;
    SP_DEVICE_INTERFACE_DATA DevInterfaceInfo;
    DWORD RequiredSize;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pBuffer;
    BOOL bResult;

    // Retrieve a handle for the relevant class information
    hDevInfo = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        cout << "\nSetupDiGetClassDevs failed! ErrorCode=" << GetLastError() << endl;
        return false;
    }

    // Enumerate the interfaces belonging to this class
    DevInterfaceInfo.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    bResult = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, pGuid, instance, &DevInterfaceInfo);
    if (bResult == FALSE)
    {
        cout << "\nSetupDiEnumDeviceInterfaces failed! ErrorCode=" << GetLastError() << endl;
        return false;
    }
        
    // Get the length of the symbolic links by passing a NULL to the third parameter and zero to the fourth.
    // If a call to SetupDiGetDeviceInterfaceDetail failed, GetLastError returns ERROR_INSUFFICIENT_BUFFER,
    // and the RequiredSize parameter receives the required buffer size,
    // which is the required length of the symbolic link name
    SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevInterfaceInfo, NULL, 0, &RequiredSize, NULL);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        // Allocate the buffer which will hold the device path which can be passed to CreateFile function
        pBuffer = (PSP_DEVICE_INTERFACE_DETAIL_DATA)new char[RequiredSize];
        if (!pBuffer)
        {
            cout << "\nMemory allocation failed!\n";
            return false; 
        }
    }
    else 
    {
        cout << "\nSetupDiGetDeviceInterfaceDetail 1 failed! ErrorCode=" << GetLastError() << endl;
        return false;
    }
    
    // If the first call suceeds, get the symbolic link itself.
    pBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    bResult = SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevInterfaceInfo, pBuffer, RequiredSize, &RequiredSize, NULL);
    if (bResult == FALSE)
    {
        cout << "\nSetupDiGetDeviceInterfaceDetail 2 failed! ErrorCode=" << GetLastError() << endl;
        return false;
    }

    rSymbolicLinkName = pBuffer->DevicePath;
    delete[] pBuffer;

    // Release the allocated resources
    if (!SetupDiDestroyDeviceInfoList(hDevInfo))
    {
        cout << "\nSetupDiDestroyDeviceInfoList failed! ErrorCode=" << GetLastError() << endl;
        return false;
    }

    // Default return value
    return true;

} // end SetupApiDeviceInterfaceInstanceEnumerate

///////////////////////////////////////////////////////////////////////////////
// This function reads the device quite a few times
///////////////////////////////////////////////////////////////////////////////
void DeviceRead(wstring SymbolicLinkName)
{
    HANDLE hDevice = NULL;
    char UserModeDataBuffer[256];
    DWORD nBytesRead;

    // Clear the user-mode application buffer
    memset(UserModeDataBuffer, NULL, sizeof(UserModeDataBuffer));

    // Let's open the device using its symbolic link name
    hDevice = CreateFile(   SymbolicLinkName.c_str(),
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL
                            );

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        cout << "\nFailed to open the device! ErrorCode=" << GetLastError() << endl;
        return;
    }

    // Read data from the device multiple times
    for (int i = 0 ; i < 100 ; i++)
    {
        if (!ReadFile(hDevice, UserModeDataBuffer, sizeof(UserModeDataBuffer), &nBytesRead , NULL))
        {
            cout << "\nFailed to read from the device! ErrorCode=" << GetLastError() << endl;
        }
        else
        {
            cout << "\nUser Mode Application Buffer Data = " << UserModeDataBuffer << endl;
        }
    }

    // Now close the handle associated with the device
    if (!CloseHandle(hDevice))
    {
        cout << "\nFailed to close the device.\n";
        return;
    }

    cout << "\nDevice successfully closed.\n";

} // end DeviceRead
