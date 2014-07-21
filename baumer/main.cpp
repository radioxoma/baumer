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

using namespace std;
#define FXOK 1


void getFormatInfo(int iLabel); // Print image size, format etc supported by device
void getCodeInfo(int iLabel, int format); // Print more deep info about appropriate image format
void getFunctInfo(int ilabel, int forma); // Print capabilites that can be changed


int main(int argc, char const *argv[])
{
    int fxReturn = 0;
    int DevCount = 0;
    fxReturn = FX_DeleteLabelInfo();
    cout << "\tFX_DeleteLabelInfo\tfxReturn: " << fxReturn << endl;
    fxReturn = FX_InitLibrary();
    cout << "FX_InitLibrary load " << fxReturn << " (version " << FX_GetVersion() << ")" << endl;

    // ask for number of Baumer 1394 camera devices
    fxReturn = FX_EnumerateDevices( &DevCount );
    cout << "\tFX_EnumerateDevices\tfxReturn: " << fxReturn << endl;
    cout << "Total camera devices present: " << DevCount << endl;

    // ask for information of cameras
    for( int devNum = 0; devNum < DevCount; devNum++ ) {
        tBOFXCAM_INFO DevInformation;
        fxReturn = FX_DeviceInfo( devNum, &DevInformation );
        cout << "\tFX_DeviceInfo\t\tfxReturn: " << fxReturn << endl;
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
        cout << "CamName: " << DevInformation.CamName << endl;

        // Associate device internal label with numbers from 0 up to DevCount
        //fxReturn = FX_LabelDevice( DevInformation.CamLabel, devNum );
        cout << "CamLabel before: " << DevInformation.CamLabel << endl;

        fxReturn = FX_LabelDevice( devNum, devNum ); // The second is mine - arbitrary number

        cout << "CamLabel: " << DevInformation.CamLabel
                << " ->> associated with devNum " << devNum
                << " fxReturn: " << fxReturn << endl;
        cout << "CamLabel after: " << DevInformation.CamLabel << endl;

        // Explore camera current properties
        tBoCameraType   dcBoType;
        tBoCameraStatus dcBoStatus;
        dcBoType.iSizeof = sizeof(dcBoType);  // Set the structure size of before usage
        dcBoStatus.iSizeof = sizeof(dcBoStatus);

        FX_GetCameraInfo( 0, &dcBoType, &dcBoStatus ); // Get current status and type of camera after camera initialization.
        // Max sensor image frame of camera
        cout << "\n\nszMaxWindow cx: " << dcBoType.szMaxWindow.cx << " cy: " << dcBoType.szMaxWindow.cy << endl;
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

        fxReturn = FX_OpenCamera( devNum );
        cout << "FX_OpenCamera\t\tfxReturn: " << fxReturn << endl;

        getFormatInfo( devNum );
    }
    FX_DeInitLibrary();
    system("pause");
    return 0;
}

// Get all supported image data formats and it's information.
// Capability from device through eFCAMQUERYCAP
void getFormatInfo( int iLabel )
{
    int fxReturn;
    int             nImgFormat;
    tpBoImgFormat*  aImgFormat;
    fxReturn = FX_GetCapability( iLabel, BCAM_QUERYCAP_IMGFORMATS, 0/*UNUSED*/, (void**)&aImgFormat, &nImgFormat );
    cout << "FX_GetCapability\t\tfxReturn: " << fxReturn << endl;
    printf( "\nBaumer Optronic IEEE1394 Camera Image Data Format\n" );
    // Use the following informations of type tBoImgFormat
    for( int i = 0; i < nImgFormat; i++ )
    {
        printf( "Format[%02d]  WxH:%04dx%04d  %s\n",
        aImgFormat[i]->iFormat,
        aImgFormat[i]->iSizeX,
        aImgFormat[i]->iSizeY,
        aImgFormat[i]->aName );

        printf("\n\nBaumer Optronic IEEE1394 Camera Image Format Codes\n");
        getCodeInfo( iLabel, aImgFormat[i]->iFormat );

        printf("\n\nBaumer Optronic IEEE1394 Search Functionality\n");
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
