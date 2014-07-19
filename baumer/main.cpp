//============================================================================
// Name        : baumer.cpp
// Author      : Eugene Dvoretsky
// Version     :
// Copyright   : MIT
// Description : Baumer Hello World in C++
//============================================================================

#include <iostream>
#include <windows.h>
#include "FxApi.h"
#include "FxError.h"
#include "main.h"

using namespace std;

int main(int argc, char const *argv[])
{
    // int fxReturn = 0;
    FX_InitLibrary();
    cout << "Library successfully loaded (version " << FX_GetVersion() << ")" << endl;
    FX_DeleteLabelInfo();

    // ask for number of Baumer 1394 camera devices
    int NumDevices = 0;
    FX_EnumerateDevices( &NumDevices );
    cout << "Total camera devices: " << NumDevices << endl;

    // ask for information of cameras
    tBOFXCAM_INFO DevInformation;
    for( int devNum = 0; devNum < NumDevices; devNum++ ) {
        FX_DeviceInfo( devNum, &DevInformation );
        FX_LabelDevice( devNum, devNum );
        // identification 16 Bit used, hardware id of camera 
        cout << "CamType: " << DevInformation.CamType << endl;  // FWX14, ...
        // 24 Bit used, baumer ieee id 
        cout << "VendorID (IEEE-VendorID Baumer): " << DevInformation.VendorID << endl;
        // 8 Bit used, serial number of 1394 interface 
        cout << "ChipIDHi (Serial number): " << DevInformation.ChipIDHi << endl;
        // 32 Bit used, serial number of 1394 interface 
        cout << "ChipIDLo (Serial number): " << DevInformation.ChipIDLo << endl;
        // payloadsize of camera device
        cout << "Payloadsize: " << DevInformation.Payloadsize << endl;
        cout << "Speed: " << DevInformation.Speed << endl;
        cout << "DspVersion (of camera): " << DevInformation.DspVersion << endl;
        cout << "HwcVersion (of camera): " << DevInformation.HwcVersion << endl;
        cout << "CamLabel (Device Label): " << DevInformation.CamLabel << endl;
        cout << "CamName: " << DevInformation.CamName << endl;
    }

    // Explore camera current properties
    tBoCameraType   dcBoType;
    tBoCameraStatus dcBoStatus;
    dcBoType.iSizeof = sizeof(dcBoType);  // Set the structure size of before usage
    dcBoStatus.iSizeof = sizeof(dcBoStatus);
    for( int iLabel = 0; iLabel < NumDevices; iLabel++ )
    {
        FX_GetCameraInfo( iLabel, &dcBoType, &dcBoStatus ); // Get current status and type of camera after camera initialization.
        // Max sensor image frame of camera
        cout << "szMaxWindow cx: " << dcBoType.szMaxWindow.cx << " cy: " << dcBoType.szMaxWindow.cy << endl;
        cout << "szCamWindow cx: " << dcBoType.szCamWindow.cx << " cy: " << dcBoType.szCamWindow.cy << endl;
        cout << "szCamWindowRoi bottom: " << dcBoType.szCamWindowRoi.bottom
            << " top: "                   << dcBoType.szCamWindowRoi.top
            << " left: "                  << dcBoType.szCamWindowRoi.left
            << " right: "                 << dcBoType.szCamWindowRoi.right << endl;
        cout << "Gain min " << dcBoType.vGainIoMin << " max: " << dcBoType.vGainIoMax << endl;
        // Seems like 'Cap' shows bool device capabilities.
        cout << "Is binning supported? " << dcBoType.Cap.bBinning << endl;
        cout << "Is gain supported? " << dcBoType.Cap.bGain << endl;

        // Status reading from camera.
        cout << "Current exposure time: " << dcBoStatus.vCurExposTime << endl;
        cout << "Current vCurAmplifFactor (gain): " << dcBoStatus.vCurAmplifFactor << endl;
        cout << "Current vBitPerPix: " << dcBoStatus.vBitPerPix << endl;
    }

    for ( int iLabel = 0; iLabel < NumDevices; iLabel++ )
    {
        getFormatInfo( iLabel );
    }
    //FX_GetXmlCameraDescription(int cLabel, char ** string)
    FX_DeInitLibrary();
    system("pause");
    return 0;
}

// Get all supported image data formats and it's information.
// Capability from device through eFCAMQUERYCAP
void getFormatInfo( int iLabel )
{
    int             nImgFormat;
    tpBoImgFormat*  aImgFormat;
    FX_GetCapability( iLabel, BCAM_QUERYCAP_IMGFORMATS, 0/*UNUSED*/, (void**)&aImgFormat, &nImgFormat );
    printf( "\nBaumer Optronic IEEE1394 Camera Image Data Format\n" );
    // Use the following informations of type tBoImgFormat
    for( int i = 0; i < nImgFormat; i++ )
    {
        printf( "/nFormat[%02d]  WxH:%04dx%04d  %s",
        aImgFormat[i]->iFormat,
        aImgFormat[i]->iSizeX,
        aImgFormat[i]->iSizeY,
        aImgFormat[i]->aName );

		printf("\nBaumer Optronic IEEE1394 Camera Image Format Codes\n");
		getCodeInfo( iLabel, aImgFormat[i]->iFormat );

		printf("\nBaumer Optronic IEEE1394 Search Functionality\n");
		getFunctInfo( iLabel, aImgFormat[i]->iFormat );
    }
}

// Get information of all supported image data coding for a special image format.
// Capability from device through eFCAMQUERYCAP
// 'format' is an eBOIMGCODEINF integer, received from 'tpBoImgFormat.iFormat'.
void getCodeInfo( int iLabel, int format )
{
    int              nImgCode;
    tpBoImgCode *    aImgCode;
    FX_GetCapability( iLabel, BCAM_QUERYCAP_IMGCODES, format, (void**)&aImgCode, &nImgCode );
    // Use the following informations of type tBoImgCode
    for( int i = 0; i < nImgCode; i++ )
    {
        printf( "\nCode[%02d]  Bpp:%d Res:%02d Canals:%d Planes:%d",
            aImgCode[i]->iCode,
            aImgCode[i]->iCanalBytes,
            aImgCode[i]->iCanalBits,
            aImgCode[i]->iCanals,
            aImgCode[i]->iPlanes );
    }
}

// Get information of all supported functions (like binning) for a special image format.
void getFunctInfo( int iLabel, int format )
{
	int               nCamFunction;
	tpBoCamFunction * aCamFunction;
	FX_GetCapability( iLabel, BCAM_QUERYCAP_CAMFUNCTIONS, format, (void**)&aCamFunction, &nCamFunction );
	for( int i = 0; i < nCamFunction; i++ )
	{
		printf( "\nFunction[%02d] %s",
			aCamFunction[i]->iFunction,
			aCamFunction[i]->aName );
	}	
}
