//============================================================================
// Name        : baumer.cpp
// Author      : Eugene Dvoretsky, Vitebsk state medical university, 2014
// Copyright   : MIT
// Description : Baumer camera Hello World in C++
//============================================================================

#include <iostream>
#include <windows.h>

#include "FxApi.h"
#include "FxError.h"

using namespace std;


void getFormatInfo(int iLabel);
void getCodeInfo(int iLabel, int iFormat);
void getFunctInfo(int iLabel, int iFormat);
void getImageShape(int iLabel, int iFormat);


int main(int argc, char const *argv[])
{
    int fxReturn = 0;
    int devCount = 0;
    fxReturn = FX_DeleteLabelInfo();
    cout << fxReturn << " FX_DeleteLabelInfo" << endl;
    fxReturn = FX_InitLibrary();
    cout << fxReturn << " FX_InitLibrary (version " << FX_GetVersion() << ")"
         << endl;

    fxReturn = FX_EnumerateDevices(&devCount);
    cout << fxReturn << " FX_EnumerateDevices" << endl;
    cout << "Total Baumer 1394 camera devices present: " << devCount << endl;

    // ask for information of cameras
    for(int devNum = 0; devNum < devCount; devNum++) {
        tBOFXCAM_INFO DevInformation;
        fxReturn = FX_DeviceInfo(devNum, &DevInformation);
        cout << fxReturn << " FX_DeviceInfo" << endl;
        // identification 16 Bit used, hardware id of camera
        cout << "CamType: " << DevInformation.CamType << endl;  // FWX14, ...
        // 24 Bit used, baumer ieee id
        cout << "IEEE-VendorID Baumer: " << DevInformation.VendorID << endl;
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
        fxReturn = FX_LabelDevice(devNum, devNum);
        cout                                   << fxReturn
             << " Device with CamLabel: "      << DevInformation.CamLabel
             << " ->> associated with devNum " << devNum << endl;

        tBoCameraType   dcBoType;
        tBoCameraStatus dcBoStatus;
        // Set the structure size of before usage
        dcBoType.iSizeof = sizeof(dcBoType);
        dcBoStatus.iSizeof = sizeof(dcBoStatus);

        fxReturn = FX_OpenCamera(devNum);
        cout << fxReturn << " FX_OpenCamera" << endl;
        if (fxReturn == 1)
            cout << "\n\t*** Camera successfully initialized ***\n" << endl;

        // Explore camera current properties.
        // Makes sense *only* if the camera was open.
        fxReturn = FX_GetCameraInfo(devNum, &dcBoType, &dcBoStatus);
        // Camera 'type' reading
        cout << "eStdImgFormat: " << dcBoType.eStdImgFormat << endl;
        cout << "szMaxWindow cx: " << dcBoType.szMaxWindow.cx
             << " cy: "              << dcBoType.szMaxWindow.cy << endl;
        cout << "szCamWindow cx: " << dcBoType.szCamWindow.cx
             << " cy: "            << dcBoType.szCamWindow.cy << endl;
        cout << "szCamWindowRoi bottom: " << dcBoType.szCamWindowRoi.bottom
             << " top: "                  << dcBoType.szCamWindowRoi.top
             << " left: "                 << dcBoType.szCamWindowRoi.left
             << " right: "                << dcBoType.szCamWindowRoi.right
             << endl;
        cout << "Gain min " << dcBoType.vGainIoMin
             << " max: "    << dcBoType.vGainIoMax << endl;
        // Seems like 'Cap' shows bool device capabilities.
        cout << "Is binning supported? " << dcBoType.Cap.bBinning << endl;
        cout << "Is gain supported? " << dcBoType.Cap.bGain << endl;

        // Camera status reading
        cout << "Current image format: " << dcBoStatus.eCurImgFormat << endl;
        getImageShape(devNum, dcBoStatus.eCurImgFormat);

        printf("Current image code[%02d] Bpp:%d Res:%02d Canals:%d Planes:%d\n",
            dcBoStatus.eCurImgCode.iCode,
            dcBoStatus.eCurImgCode.iCanalBytes,
            dcBoStatus.eCurImgCode.iCanalBits,
            dcBoStatus.eCurImgCode.iCanals,
            dcBoStatus.eCurImgCode.iPlanes);

        cout << "Current exposure time: " << dcBoStatus.vCurExposTime << endl;
        cout << "Current vCurAmplifFactor (gain): "
             << dcBoStatus.vCurAmplifFactor << endl;
        cout << "Current vBitPerPix: " << dcBoStatus.vBitPerPix << endl;

        getFormatInfo(devNum);

        fxReturn = FX_CloseCamera(devNum);
        cout << fxReturn << " FX_CloseCamera" << endl;
    }
    fxReturn = FX_DeInitLibrary();
    system("pause");
    return 0;
}


/**
 * @brief Get all supported image data formats and it's information (e.g. size).
 * @details Get capability from device through eFCAMQUERYCAP
 *
 * @param iLabel Camera device label
 */
void getFormatInfo(int iLabel)
{
    int fxReturn = 0;
    int nImgFormat;
    tpBoImgFormat* aImgFormat;

    printf("\n\nAvailable camera image data format\n");
    fxReturn = FX_GetCapability(iLabel, BCAM_QUERYCAP_IMGFORMATS, 0/*UNUSED*/,
        (void**)&aImgFormat, &nImgFormat);
    cout << fxReturn << " FX_GetCapability" << endl;
    // Use the following informations of type tBoImgFormat
    for(int i = 0; i < nImgFormat; i++) {
        printf("\n\n\tFormat[%02d]  WxH:%04dx%04d\n\t%s\n",
        aImgFormat[i]->iFormat,
        aImgFormat[i]->iSizeX,
        aImgFormat[i]->iSizeY,
        aImgFormat[i]->aName);

        getCodeInfo(iLabel, aImgFormat[i]->iFormat);
        getFunctInfo(iLabel, aImgFormat[i]->iFormat);
    }
}


/**
 * @brief Get information of all supported image data coding for a special
 * image format.
 * @details Capability from device through eFCAMQUERYCAP 'format' is an
 * eBOIMGCODEINF integer, received from 'tpBoImgFormat.iFormat'
 *
 * @param iLabel Camera device label
 * @param iFormat Image format number
 */
void getCodeInfo(int iLabel, int iFormat)
{
    int fxReturn = 0;
    int nImgCode;
    tpBoImgCode* aImgCode;

    printf("\nAvailable camera image Format Codes\n\n");
    fxReturn = FX_GetCapability(iLabel, BCAM_QUERYCAP_IMGCODES, iFormat,
        (void**)&aImgCode, &nImgCode);
    // Use the following informations of type tBoImgCode
    for(int i = 0; i < nImgCode; i++) {
        printf("Code[%02d] Bpp:%d Res:%02d Canals:%d Planes:%d\n",
            aImgCode[i]->iCode,
            aImgCode[i]->iCanalBytes,
            aImgCode[i]->iCanalBits,
            aImgCode[i]->iCanals,
            aImgCode[i]->iPlanes);
    }
}


/**
 * @brief Print camera capabilities that can be changed
 * @details Get information of all supported functions (like binning) for a
 * special image format.
 *
 * @param iLabel Camera device label
 * @param iFormat Image format number
 */
void getFunctInfo(int iLabel, int iFormat)
{
    int fxReturn = 0;
    int nCamFunction;
    tpBoCamFunction* aCamFunction;

    printf("\n\nBaumer Optronic IEEE1394 Search Functionality\n\n");
    fxReturn = FX_GetCapability(iLabel, BCAM_QUERYCAP_CAMFUNCTIONS, iFormat,
        (void**)&aCamFunction, &nCamFunction);
    for(int i = 0; i < nCamFunction; i++) {
        printf("Function[%02d] %s\n",
            aCamFunction[i]->iFunction,
            aCamFunction[i]->aName);
    }
}


/**
 * @brief Detect frame properties for specifed image format enum value.
 * @details Detect frame properties (e.g. width, height) for specifed image
 * format enum value.
 *
 * @param iLabel Camera device label
 * @param iFormat Image format number
 */
void getImageShape(int iLabel, int iFormat)
{
    bool isFound = false;
    int fxReturn = 0;
    int nImgFormat;
    tpBoImgFormat* aImgFormat;

    fxReturn = FX_GetCapability(iLabel, BCAM_QUERYCAP_IMGFORMATS, 0/*UNUSED*/,
        (void**)&aImgFormat, &nImgFormat);
    cout << fxReturn << " FX_GetCapability" << endl;
    for(int i = 0; i < nImgFormat; i++) {
        if (iFormat == aImgFormat[i]->iFormat) {
            isFound = true;
            printf("Format detected: %02d, WxH:%04dx%04d  %s\n",
                aImgFormat[i]->iFormat,
                aImgFormat[i]->iSizeX,
                aImgFormat[i]->iSizeY,
                aImgFormat[i]->aName);
        }
    }
    if (isFound == false) {
        printf("Unable to found image format: %02d", iFormat);
    }
}
