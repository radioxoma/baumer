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


void getFormatInfo(int iLabel);
void getCodeInfo(int iLabel, int format);
void getFunctInfo(int ilabel, int forma);


int main(int argc, char const *argv[])
{
    int fxReturn = 0;
    int devCount = 0;
    fxReturn = FX_DeleteLabelInfo();
    cout << fxReturn << " FX_DeleteLabelInfo" << endl;
    fxReturn = FX_InitLibrary();
    cout << fxReturn << " FX_InitLibrary (version " << FX_GetVersion() << ")" << endl;

    fxReturn = FX_EnumerateDevices( &devCount );
    cout << fxReturn << " FX_EnumerateDevices" << endl;
    cout << "Total Baumer 1394 camera devices present: " << devCount << endl;

    // ask for information of cameras
    for( int devNum = 0; devNum < devCount; devNum++ ) {
        tBOFXCAM_INFO DevInformation;
        fxReturn = FX_DeviceInfo( devNum, &DevInformation );
        cout << fxReturn << " FX_DeviceInfo" << endl;
        // identification 16 Bit used, hardware id of camera
        cout << "CamType: " << DevInformation.CamType << endl;  // FWX14, ...
        // 24 Bit used, baumer ieee id
        cout << "VendorID (IEEE-VendorID Baumer): " << DevInformation.VendorID << endl;
        // 8 Bit used, serial number of 1394 interface
        cout << "ChipIDHi (Serial number): " << DevInformation.ChipIDHi << endl;
        // 32 Bit used, serial number of 1394 interface
        cout << "ChipIDLo (Serial number): " << DevInformation.ChipIDLo << endl;
        cout << "Payloadsize: " << DevInformation.Payloadsize << endl;
        cout << "Speed: " << DevInformation.Speed << endl;
        cout << "DspVersion (of camera): " << DevInformation.DspVersion << endl;
        cout << "HwcVersion (of camera): " << DevInformation.HwcVersion << endl;
        cout << "CamName: " << DevInformation.CamName << endl;

        // All cameras enumerated from 0 up to `devCount` now. It's necessary
        // to associate every camera device number `devNum` with an arbitrary
        // 'label' integer (second argument). Later we would use the label as
        // camera identifier.
        fxReturn = FX_LabelDevice( devNum, devNum );
        cout                                   << fxReturn
             << " CamLabel: "                  << DevInformation.CamLabel
             << " ->> associated with devNum " << devNum << endl;

        // Explore camera current properties
        tBoCameraType   dcBoType;
        tBoCameraStatus dcBoStatus;
        dcBoType.iSizeof = sizeof(dcBoType);  // Set the structure size of before usage
        dcBoStatus.iSizeof = sizeof(dcBoStatus);

        fxReturn = FX_GetCameraInfo( 0, &dcBoType, &dcBoStatus ); // Get current status and type of camera after camera initialization.
        cout << "eStdImgFormat: " << dcBoType.eStdImgFormat << endl; // Important
        cout << "szMaxWindow cx: " << dcBoType.szMaxWindow.cx
             << " cy: "              << dcBoType.szMaxWindow.cy << endl;
        cout << "szCamWindow cx: " << dcBoType.szCamWindow.cx
             << " cy: "            << dcBoType.szCamWindow.cy << endl;
        cout << "szCamWindowRoi bottom: " << dcBoType.szCamWindowRoi.bottom
             << " top: "                  << dcBoType.szCamWindowRoi.top
             << " left: "                 << dcBoType.szCamWindowRoi.left
             << " right: "                << dcBoType.szCamWindowRoi.right << endl;
        cout << "Gain min " << dcBoType.vGainIoMin
             << " max: "    << dcBoType.vGainIoMax << endl;
        // Seems like 'Cap' shows bool device capabilities.
        cout << "Is binning supported? " << dcBoType.Cap.bBinning << endl;
        cout << "Is gain supported? " << dcBoType.Cap.bGain << endl;

        // Status reading from camera.
        cout << "Current image format: " << dcBoStatus.eCurImgFormat << endl;
        printf( "Current image code[%02d]  Bpp:%d Res:%02d Canals:%d Planes:%d\n",
            dcBoStatus.eCurImgCode.iCode,
            dcBoStatus.eCurImgCode.iCanalBytes,
            dcBoStatus.eCurImgCode.iCanalBits,
            dcBoStatus.eCurImgCode.iCanals,
            dcBoStatus.eCurImgCode.iPlanes );

        cout << "Current exposure time: " << dcBoStatus.vCurExposTime << endl;
        cout << "Current vCurAmplifFactor (gain): " << dcBoStatus.vCurAmplifFactor << endl;
        cout << "Current vBitPerPix: " << dcBoStatus.vBitPerPix << endl;

        if ( DevInformation.CamLabel != -1 ) {
        fxReturn = FX_OpenCamera( devNum );
        cout << fxReturn << " FX_OpenCamera" << endl;

        getFormatInfo( devNum );

        fxReturn = FX_CloseCamera( devNum );
        cout << fxReturn << " FX_CloseCamera" << endl;
        }
        else
            cout << "Something wrong with camera label: " << devNum << endl;
    }
    fxReturn = FX_DeInitLibrary();
    system( "pause" );
    return 0;
}


/**
 * @brief Get all supported image data formats and it's information (e.g. size).
 * @details Get capability from device through eFCAMQUERYCAP
 *
 * @param iLabel Camera device label
 */
void getFormatInfo( int iLabel )
{
    int fxReturn = 0;
    int nImgFormat;
    tpBoImgFormat* aImgFormat;

    printf( "\n\nBaumer Optronic IEEE1394 Camera Image Data Format\n" );
    fxReturn = FX_GetCapability( iLabel, BCAM_QUERYCAP_IMGFORMATS, 0/*UNUSED*/, (void**)&aImgFormat, &nImgFormat );
    cout << fxReturn << " FX_GetCapability" << endl;
    // Use the following informations of type tBoImgFormat
    for( int i = 0; i < nImgFormat; i++ ) {
        printf( "Format[%02d]  WxH:%04dx%04d  %s\n",
        aImgFormat[i]->iFormat,
        aImgFormat[i]->iSizeX,
        aImgFormat[i]->iSizeY,
        aImgFormat[i]->aName );

        getCodeInfo( iLabel, aImgFormat[i]->iFormat );
        getFunctInfo( iLabel, aImgFormat[i]->iFormat );
    }
}


/**
 * @brief Get information of all supported image data coding for a special
 * image format.
 * @details Capability from device through eFCAMQUERYCAP 'format' is an
 * eBOIMGCODEINF integer, received from 'tpBoImgFormat.iFormat'
 *
 * @param iLabel Camera device label
 * @param format
 */
void getCodeInfo( int iLabel, int format )
{
    int fxReturn = 0;
    int nImgCode;
    tpBoImgCode* aImgCode;

    printf("\n\nBaumer Optronic IEEE1394 Camera Image Format Codes\n");
    fxReturn = FX_GetCapability( iLabel, BCAM_QUERYCAP_IMGCODES, format, (void**)&aImgCode, &nImgCode );
    // Use the following informations of type tBoImgCode
    for( int i = 0; i < nImgCode; i++ ) {
        printf( "Code[%02d]  Bpp:%d Res:%02d Canals:%d Planes:%d\n",
            aImgCode[i]->iCode,
            aImgCode[i]->iCanalBytes,
            aImgCode[i]->iCanalBits,
            aImgCode[i]->iCanals,
            aImgCode[i]->iPlanes );
    }
}


/**
 * @brief Print camera capabilities that can be changed
 * @details Get information of all supported functions (like binning) for a
 * special image format.
 *
 * @param iLabel Camera device label
 * @param format
 */
void getFunctInfo( int iLabel, int format )
{
    int fxReturn = 0;
    int nCamFunction;
    tpBoCamFunction* aCamFunction;

    printf("\n\nBaumer Optronic IEEE1394 Search Functionality\n");
    fxReturn = FX_GetCapability( iLabel, BCAM_QUERYCAP_CAMFUNCTIONS, format, (void**)&aCamFunction, &nCamFunction );
    for( int i = 0; i < nCamFunction; i++ ) {
        printf( "\nFunction[%02d] %s",
            aCamFunction[i]->iFunction,
            aCamFunction[i]->aName );
    }
}
