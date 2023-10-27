@echo off
cd..
cd..
SET PROJECT_PATH=%CD%
cd collectable\Working
SET WORKING_PATH=%CD%

cd /d "%PROJECT_PATH%\collectable\Flash"
attrib +r ble_build_all.bat
del /Q *.* > nul 2>&1
attrib -r ble_build_all.bat

@echo Start building from keil
@echo off
REM Compile arm-mdk projects
cd /d "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10040\s132\arm5_no_packs"
UV4 -j0 -b firmware_ble_peripheral_ble_app_payg_x_ota_pca10040_s132_arm5_no_packs_ble_app_buttonless_dfu_pca10040_s132.uvprojx -t"nrf52832_xxaa"
IF EXIST "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10040\s132\arm5_no_packs\_build\nrf52832_xxaa.axf" (
  ECHO Build nrf52832_xxaa successful
) ELSE (
  ECHO Build nrf52832_xxaa failed
  pause
)

cd /d "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10100\s140\arm5_no_packs"
UV4 -j0 -b ble_app_buttonless_dfu_pca10100_s140.uvprojx -t"nrf52833_xxaa"
IF EXIST "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10100\s140\arm5_no_packs\_build\nrf52833_xxaa.axf" (
  ECHO Build nrf52833_xxaa successful
) ELSE (
  ECHO Build nrf52833_xxaa failed
  pause
)

cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble\arm5_no_packs"
UV4 -j0 -b secure_bootloader_ble_s132_pca10040.uvprojx -t"nrf52832_xxaa_s132"
IF EXIST "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble\arm5_no_packs\_build\nrf52832_xxaa_s132.axf" (
  ECHO Build nrf52832_xxaa_s132 successful
) ELSE (
  ECHO Build nrf52832_xxaa_s132 failed
  pause
)

cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble_debug\arm5_no_packs"
UV4 -j0 -b secure_bootloader_ble_s132_pca10040_debug.uvprojx -t"nrf52832_xxaa_s132"
IF EXIST "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble_debug\arm5_no_packs\_build\nrf52832_xxaa_s132.axf" (
  ECHO Build nrf52832_xxaa_s132 debug successful
) ELSE (
  ECHO Build nrf52832_xxaa_s132 debug failed
  pause
)

cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble\arm5_no_packs"
UV4 -j0 -b secure_bootloader_ble_s140_pca10100.uvprojx -t"nrf52833_xxaa_s140"
IF EXIST "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble\arm5_no_packs\_build\nrf52833_xxaa_s140.axf" (
  ECHO Build nrf52833_xxaa_s140 successful
) ELSE (
  ECHO Build nrf52833_xxaa_s140 failed
  pause
)

cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble_debug\arm5_no_packs"
UV4 -j0 -b secure_bootloader_ble_s140_pca10100_debug.uvprojx -t"nrf52833_xxaa_s140"
IF EXIST "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble_debug\arm5_no_packs\_build\nrf52833_xxaa_s140.axf" (
  ECHO Build nrf52833_xxaa_s140 debug successful
) ELSE (
  ECHO Build nrf52833_xxaa_s140 debug failed
  pause
)

REM Build gcc-based projects
@echo Start building from GCC
@echo off
REM Compile arm-mdk projects
cd /d "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10040\s132\armgcc"
make
cd /d "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10100\s140\armgcc"
make

cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble\armgcc"
make
cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble_debug\armgcc"
make
cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble\armgcc"
make
cd /d "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble_debug\armgcc"
make


REM Copy files to ouput, for now we use arm-mdk as primary compiler
copy "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10040\s132\arm5_no_packs\_build\nrf52832_xxaa.hex" "%WORKING_PATH%"
copy "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble\arm5_no_packs\_build\nrf52832_xxaa_s132.hex" "%WORKING_PATH%"
copy "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10040_s132_ble_debug\arm5_no_packs\_build\nrf52832_xxaa_s132.hex" "%WORKING_PATH%\nrf52832_xxaa_s132_debug.hex"

copy "%PROJECT_PATH%\firmware\ble_peripheral\ble_app_x_ota\pca10100\s140\arm5_no_packs\_build\nrf52833_xxaa.hex" "%WORKING_PATH%"
copy "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble\arm5_no_packs\_build\nrf52833_xxaa_s140.hex" "%WORKING_PATH%"
copy "%PROJECT_PATH%\firmware\dfu\secure_bootloader\pca10100_s140_ble_debug\arm5_no_packs\_build\nrf52833_xxaa_s140.hex" "%WORKING_PATH%\nrf52833_xxaa_s140_debug.hex"

REM Copy softdevice
copy "%PROJECT_PATH%\components\softdevice\s132\hex\s132_nrf52_7.2.0_softdevice.hex" "%WORKING_PATH%"
copy "%PROJECT_PATH%\components\softdevice\s140\hex\s140_nrf52_7.2.0_softdevice.hex" "%WORKING_PATH%"

REM Start merge hex files to single file per project
cd /d "%WORKING_PATH%"
nrfutil settings generate --family NRF52 --application nrf52832_xxaa.hex --application-version 0 --bootloader-version 0 --bl-settings-version 2 bootloader_setting652.hex
mergehex --merge nrf52832_xxaa_s132.hex bootloader_setting652.hex --output bootloader_with_setting652.hex
mergehex --merge bootloader_with_setting652.hex nrf52832_xxaa.hex --output bootloader_with_setting_app652.hex
mergehex --merge bootloader_with_setting_app652.hex s132_nrf52_7.2.0_softdevice.hex --output bl652.hex

mergehex --merge nrf52832_xxaa_s132_debug.hex bootloader_setting652.hex --output bootloader_with_setting652.hex
mergehex --merge bootloader_with_setting652.hex nrf52832_xxaa.hex --output bootloader_with_setting_app652.hex
mergehex --merge bootloader_with_setting_app652.hex s132_nrf52_7.2.0_softdevice.hex --output bl652_debug.hex


cd /d "%WORKING_PATH%"
nrfutil settings generate --family NRF52 --application nrf52833_xxaa.hex --application-version 0 --bootloader-version 0 --bl-settings-version 2 bootloader_setting.hex
mergehex --merge nrf52833_xxaa_s140_debug.hex bootloader_setting.hex --output bootloader_with_setting_debug.hex
mergehex --merge bootloader_with_setting_debug.hex nrf52833_xxaa.hex --output bootloader_with_setting_app_debug.hex
mergehex --merge bootloader_with_setting_app_debug.hex s140_nrf52_7.2.0_softdevice.hex --output bl653_debug.hex

mergehex --merge nrf52833_xxaa_s140.hex bootloader_setting.hex --output bootloader_with_setting.hex
mergehex --merge bootloader_with_setting.hex nrf52833_xxaa.hex --output bootloader_with_setting_app.hex
mergehex --merge bootloader_with_setting_app.hex s140_nrf52_7.2.0_softdevice.hex --output bl653.hex

REM Copy files to collectables and create batch files for easy flash download
set day=%date:~0,2%
set month=%date:~3,2%
set year=%date:~6,4%
set today=%year%%month%%day%
set mytime1=%time:~0,2%
set mytime2=%time:~3,2%
set mytime=%mytime1%%mytime2%
set "mytime=%mytime: =%"
set "today=%today: =%"

cd /d "%WORKING_PATH%"
copy "%WORKING_PATH%\bl652.hex" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl652_x.hex"
copy "%WORKING_PATH%\bl652_debug.hex" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl652_x_debug.hex"
copy "%WORKING_PATH%\bl653_debug.hex" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl653_x_debug.hex"
copy "%WORKING_PATH%\bl653.hex" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl653_x.hex"

cd /d "%PROJECT_PATH%\collectable\Flash\"
echo @echo off> "%today%_%mytime%_bl652_x_debug.bat"
echo nrfjprog --eraseall>>"%today%_%mytime%_bl652_x_debug.bat"
echo nrfjprog -f NRF52 --program %today%_%mytime%_bl652_x_debug.hex>>"%today%_%mytime%_bl652_x_debug.bat"
echo.>> "%today%_%mytime%_bl652_x_debug.bat"
echo @echo Done!>> "%today%_%mytime%_bl652_x_debug.bat"
echo nrfjprog --reset>> "%today%_%mytime%_bl652_x_debug.bat"
echo pause>> "%today%_%mytime%_bl652_x_debug.bat"

echo @echo off> "%today%_%mytime%_bl652_x.bat"
echo nrfjprog --eraseall>>"%today%_%mytime%_bl652_x.bat"
echo nrfjprog -f NRF52 --program %today%_%mytime%_bl652_x.hex>>"%today%_%mytime%_bl652_x.bat"
echo.>> "%today%_%mytime%_bl652_x.bat"
echo @echo Done!>> "%today%_%mytime%_bl652_x.bat"
echo nrfjprog --reset>> "%today%_%mytime%_bl652_x.bat"
echo pause>> "%today%_%mytime%_bl652_x.bat"

echo @echo off> "%today%_%mytime%_bl653_x_debug.bat"
echo nrfjprog --eraseall>>"%today%_%mytime%_bl653_x_debug.bat"
echo nrfjprog -f NRF52 --program %today%_%mytime%_bl653_x_debug.hex>>"%today%_%mytime%_bl653_x_debug.bat"
echo.>> "%today%_%mytime%_bl653_x_debug.bat"
echo @echo Done!>> "%today%_%mytime%_bl653_x_debug.bat"
echo nrfjprog --reset>> "%today%_%mytime%_bl653_x_debug.bat"
echo pause>> "%today%_%mytime%_bl653_x_debug.bat"

echo @echo off> "%today%_%mytime%_bl653_x.bat"
echo nrfjprog --eraseall>>"%today%_%mytime%_bl653_x.bat"
echo nrfjprog -f NRF52 --program %today%_%mytime%_bl653_x.hex>>"%today%_%mytime%_bl653_x.bat"
echo.>> "%today%_%mytime%_bl653_x.bat"
echo @echo Done!>> "%today%_%mytime%_bl653_x.bat"
echo nrfjprog --reset>> "%today%_%mytime%_bl653_x.bat"
echo pause>> "%today%_%mytime%_bl653_x.bat"

REM Zip files for easy transfer to manufacturer
REM tar.exe -cf "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl652_x_debug.tar" "%today%_%mytime%_bl652_x_debug.bat" "%today%_%mytime%_bl652_x_debug.hex"
REM tar.exe -cf "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl653_x_debug.tar" "%today%_%mytime%_bl653_x_debug.bat" "%today%_%mytime%_bl653_x_debug.hex"
rar a -idq "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl652_x_debug.zip" "%today%_%mytime%_bl652_x_debug.bat" "%today%_%mytime%_bl652_x_debug.hex"
rar a -idq "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl652_x.zip" "%today%_%mytime%_bl652_x.bat" "%today%_%mytime%_bl652_x.hex"
rar a -idq "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl653_x_debug.zip" "%today%_%mytime%_bl653_x_debug.bat" "%today%_%mytime%_bl653_x_debug.hex"
rar a -idq "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl653_x.zip" "%today%_%mytime%_bl653_x.bat" "%today%_%mytime%_bl653_x.hex"

REM Create OTA upload files
cd /d "%WORKING_PATH%"
nrfutil pkg generate --hw-version 52 --application-version 1 --application nrf52832_xxaa.hex --sd-req 0x0101 --key-file private.key bl652_debug_app_dfu_package.zip
nrfutil pkg generate --hw-version 52 --application-version 1 --application nrf52833_xxaa.hex --sd-req 0x0100 --key-file private.key bl653_debug_app_dfu_package.zip
copy "%WORKING_PATH%\bl652_debug_app_dfu_package.zip" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl652_x_debug_ota_x_package.zip"
copy "%WORKING_PATH%\bl652_debug_app_dfu_package.zip" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl652_x_airlink_ota_x_package.zip"
copy "%WORKING_PATH%\bl653_debug_app_dfu_package.zip" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl653_x_debug_ota_x_package.zip"
copy "%WORKING_PATH%\bl653_debug_app_dfu_package.zip" "%PROJECT_PATH%\collectable\Flash\%today%_%mytime%_bl653_x_airlink_x_dfu_package.zip"

pause