# Simusolar
## Airlink-Device Project
Airlink-Device Project was primarily focused on the new Bluetooth-enabled fishing light called NURU.
The project has been extended to host other sub-projects for PAYG control boxes that enable simusolar to have a financed model for not only fishing light customers but also for customers who need other products like pumps and solar refrigerators.
 
## Sub-Project SSL FL and it's Features
SSL FL is a project based on enabling fishermen to have a better fishing light.
However the high brightness is not the only requirement from the fishermen, they also like the capability of finding their lost light and the light to have the ability to PAY AS YOU GO so that some who can't afford the light will be able to buy it in the financed model.
This fishing light is compatible with Enaccess Airlink, this allows sharing of critical information with any compatible devices from any vendor.

- Nexus Resource model compatible
- Can be located when lost 
- Able to have variable brightness up to 10W
- 0.96' tft display which can show the identity of the device and the identity of the owner.
- 5.0 Bluetooth wireless (No gsm, data can be sent to a server via gsm enabled gateway)
- Other features

## Sub-Project PAYGLV, PAYGHV, PAYGFL (FL Charger) and it's Features
Todo
- Nexus Resource model compatible
- TODO

This project is based on nrf52 buttonless bootloader, though the project uses BL modules from Laird, the language used is C and not smart-basic.
The choice of C programming is due to its flexibility of it especially on writing the low-level drivers eg. GFX

## Tech

Simusolar FL uses several open-source projects to work properly:

- NRF52 SDK - Project built on top of it.
- Simusolar Airlink - This app acts as a gateway
- Nexus Keycode - This project provides access to PAYG features

## Installation

Simusolar FL requires [todo](https://) v17+ to run.

Install the dependencies and devDependencies
```sh
Nordic SDK provides multiple ways of building their firmware, Keil ARM_MDK provides the fastest way of getting started with SDK, however, this complier is expensive.
GCC-arm and eclipse is also an option and it is free though it requires some time in configuration.

**Method 1: Keil**
https://www2.keil.com/mdk5

**Method 2: gcc**
https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/development-with-eclipse-and-gcc
```

For production environments...

```sh
**Hardware**
To load the firmware to the BL-Chip, Segger J-link hardware-connected SWD pins are required.

**Software**
There are two options:
1. nRf connect/ programmer
2. nrf-command-line-tools
```

## Development

Want to contribute? Great!

Simusolar-FL uses Keil MDK or GCC fast developing.
Make a change in your file, compile and instantaneously see your updates!

For Debug, The project uses Segger J-link hardware and SWD pins.

Debug messages:


#### Ready to start?:
Below Steps are necessary to get started.
```sh
1. Install hardware requirements
   a) nRf Connect for desktop and programmer: https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-desktop
   b) nRf-commandline tools: https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools/download
   c) segger j-link: https://www.segger.com/downloads/jlink/
   d) nRf Connect APP on mobile: https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-mobile

2. Choose developement tools of your choice, for now there are two compilers that are fully supported, the ARM-MDK and GCC-ARM.
   a) For ARM-MDK (Windows) the way is straight and easy, however the advantages comes with high price.
      Step1: Download and install keil from: https://www2.keil.com/mdk5
      Step2: Install targets (nrf52832 and nrf52833) packages using package installer
      Step3: Compile but don not load (project under either firmware\...\arm5_no_packs) 
      
   b) For GCC (Windows), you have a long way to go.
      Step1: Read carefully the resource attached: https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/development-with-eclipse-and-gcc
      Step2: Install Make: http://gnuwin32.sourceforge.net/packages/make.htm
      Step3: Install CoreUtil: http://gnuwin32.sourceforge.net/packages/coreutils.htm
      Step4: Install gcc--arm: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
      Step5: Install IDE of you choice (Preferably Eclipse)
      Step6: Add System Environments for \bin directories of make, coreutils and gcc
      Step7: Modify gcc configuration found under components\toolchain\gcc\Makefile.windows
      Step8: Complete instructions from above link attached for IDE configuration
      
3. Loading the firmware to flash
   Now you can compile, then you need to load the program to flash. The project uses the softdevice and bootloader, these needs to loaded before the application.
      Step1: Run full erase using nRf connect programmer (Necessary to remove smart basic)
      Step2: Flash softdevice using nRf connect programmer (softdevice are available at \components\softdevice)
      Step3: Compile bootloader using instructions under Building for source
      Step4: Flash bootloader (Found under\firmware\dfu, use the _debug for development), see if it has started advertising
      Step5: Compile application (Found under \firmware\ble_peripheral), for now do not load as the signatures does not match with bootloader
      Step6: Make OTA application using instruction under Building for source (5)
      Step7: Copy the compressed OTA Application to smart phone
      Step8: Send Applicaiton via bluetooth via nrf connect/ dfu
      If you flashed bootloader ending with _debug, you will be able to flash the application using nRf connect desktop and j-link
   
4. Learning Resources:
a) Advertising, service and characteristics: 
      https://www.notion.so/simusolar/Create-foundation-for-connectable-adverts-with-encoded-status-capability-on-FL-714c08c233634e0cbaf35c2066a77fcb
      https://www.notion.so/simusolar/BL652-3-Formated-advertising-7ed4b6e1b26447f78c5ec367069d57df
      SDK17: ble_peripheral\ble_app_blinky example
      
b) OTA Capability: 
      https://www.notion.so/simusolar/Create-foundation-for-OTA-capability-on-FL-443d547aedc44794af54007df8388989
      SDK17: ble_peripheral\ble_app_buttonless_dfu example
   
c) Flash storage: 
      https://www.notion.so/simusolar/Foundational-for-data-storage-16bae219953a469ca5943bfeca5232c9
      SDK17: peripheral\flash_fstorage example
   
d) Uart:
      SDK17: ble_peripheral\ble_app_uart example

e) LCD and GFX Library:
      SDK17: peripheral\gfx example
      https://www.notion.so/simusolar/Customer-can-turn-SSL-fishing-light-on-off-and-view-status-fef5ddb0934044a6abe3538515e406c2
      http://www.eran.io/the-dot-factory-an-lcd-font-and-image-generator/

f) Timers and rtc: 
      SDK17: peripheral\rtc example
      SDK17: peripheral\timer example

g) GPIOs,buttons and LEDs
      SDK17: peripheral\pin_change_int example

h) Temperature
      SDK17: peripheral\temperature example
  
i) Battery decision and logic:
      https://www.notion.so/simusolar/Create-low-power-mode-power-budget-for-Fishing-Light-for-BL653-fdff43ad9d1e47fabfa978dfc4b1ba49#09748d9f6ff14b7f815377282f962fce

j) CBOR:
      https://en.wikipedia.org/wiki/CBOR
      https://cbor.io/
      https://cbor.me/
      
5) Important files and roles:
   sdk_config.h: Master configuration file
   ssl_config.h: SSL Based configuration
   ssl_global_variables.h: Master varibles and structures
```
#### Building for source

Simusolar-FL uses a secure bootloader, which requires both private and public keys.
```sh
**Steps for keys creation:**
source: https://devzone.nordicsemi.com/nordic/short-range-guides/b/software-development-kit/posts/getting-started-with-nordics-secure-dfu-bootloader

1. Download nrfutil from https://github.com/NordicSemiconductor/pc-nrfutil/releases

2. Generate a private key using the command: nrfutil.exe keys generate private.key

3. Generate a public key using the command: nrfutil keys display --key pk --format code private.key --out_file public_key.c

4. Use Oberon crypto library instead of uECC in sdk_config.h change the following:
   NRF_CRYPTO_BACKEND_MICRO_ECC_ENABLED to 0
   NRF_CRYPTO_BACKEND_OBERON_ENABLED to 1
   And remove micro_ecc_lib_nrf52.lib file in the project list. 

5. Make an ota application:
a) nrf52832:
   nrfutil pkg generate --hw-version 52 --application-version 1 --application nrf52832_xxaa.hex --sd-req 0x0101 --key-file private.key app_dfu_package.zip
   --sd-req 0x0101 ***** check documentation

b) nrf52833:
   nrfutil pkg generate --hw-version 52 --application-version 1 --application nrf52833_xxaa.hex --sd-req 0x0100 --key-file private.key 0pa1_dfu_package.zip
   --sd-req 0x0100 ***** check documentation

6) To test, copy the file to the phone and use the nRf connect APP to flash the ble device using OTA.


**Steps for merging the softdevice, bootloader and application into one file:**
1. Generate bootloader setting and Merge with bootloader:
a) nrf52832:
   nrfutil settings generate --family NRF52 --application nrf52832_xxaa.hex --application-version 0 --bootloader-version 0 --bl-settings-version 2 bootloader_setting652.hex

b) nrf52833:
   nrfutil settings generate --family NRF52 --application nrf52833_xxaa.hex --application-version 0 --bootloader-version 0 --bl-settings-version 2 bootloader_setting.hex

2. Start to merge:
a) nrf52832 with debug:
   mergehex --merge nrf52832_xxaa_s132.hex bootloader_setting652.hex --output bootloader_with_setting652.hex
   mergehex --merge bootloader_with_setting652.hex nrf52832_xxaa.hex --output bootloader_with_setting_app652.hex
   mergehex --merge bootloader_with_setting_app652.hex s132_nrf52_7.2.0_softdevice.hex --output bl652_debug.hex
   
b) nrf52833:
   mergehex --merge nrf52833_xxaa_s140.hex bootloader_setting.hex --output bootloader_with_setting.hex
   mergehex --merge bootloader_with_setting.hex nrf52833_xxaa.hex --output bootloader_with_setting_app.hex
   mergehex --merge bootloader_with_setting_app.hex s140_nrf52_7.2.0_softdevice.hex --output bootloader_with_setting_app_softdevice.hex   
   
c) nrf52833 with debug:
   mergehex --merge nrf52833_xxaa_s140_debug.hex bootloader_setting.hex --output bootloader_with_setting_debug.hex
   mergehex --merge bootloader_with_setting_debug.hex nrf52833_xxaa.hex --output bootloader_with_setting_app_debug.hex
   mergehex --merge bootloader_with_setting_app_debug.hex s140_nrf52_7.2.0_softdevice.hex --output bl653_debug.hex

   ***** Make sure the said files are in the directory

## License

*Simusolar and Dev partners*