# Baumer camera control sample #

*This small application prints Firewire camera capabilities.*

This code uses `FxLib.dll` - Windows-only library that controls some [Baumer Optronic GmbH](http://www.baumer.com) Firewire cameras. The library underlies open source *[Micromanager BaumerOptronic adapter](https://micro-manager.org/wiki/BaumerOptronic)* and proprietary *Leica Acquisition Suite* (as far Baumer produces some Leica DFC cameras).

This code had developed with [Leica DFC 295](http://www.leica-microsystems.com/products/microscope-cameras/industry/details/product/leica-dfc295/).


## Setting up ##

To build this code you need old [Baumer FireWire SDK](http://www.baumer.com/int-en/products/identification-image-processing/software-and-starter-kits/baumer-gapi-sdk/) v1.7.1. For testing you need installed Baumer or Leica camera driver. If you have Leica Acquisition Suite (LAS) on your system, likely you already have one.

Visual studio project in repository has paths compatible with Micromanager BaumerOptronic adapter. So just place SDK in proper location and clone this repo near with other Micro-manager repositories.

    3rdparty\Leica\camera\SDK1.7\BSTDFxLib\x86\Inc
    3rdparty\Leica\camera\SDK1.7\BSTDFxLib\x86\Lib # And so on
    3rdpartypublic
    baumer
    micromanager


## Using ##

You can just run application without compiling anything. Folder `./Release` contain more-or-less updated binary.
