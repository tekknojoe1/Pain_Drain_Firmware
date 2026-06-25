@echo off
REM Post-build script for PainDrain app with bootloader
REM Parameters: %1=OutputDir, %2=ProjectName

setlocal enabledelayedexpansion

set OUTPUT_DIR=%~1
set PROJECT_NAME=%~2
set SCRIPT_DIR=%~dp0..\..

REM Paths to hex files
set BOOTLOADER_HEX=%SCRIPT_DIR%\Bootloader\Bootloader_BLE_App101.cydsn\CortexM4\ARM_GCC_541\Debug\Bootloader_BLE_App101.hex
set APP_HEX=%OUTPUT_DIR%\%PROJECT_NAME%.hex
set MERGED_HEX=%OUTPUT_DIR%\%PROJECT_NAME%.hex

echo.
echo Post-build: Merging hex files
echo  Bootloader: %BOOTLOADER_HEX%
echo  App:        %APP_HEX%
echo  Output:     %MERGED_HEX%

REM Check if bootloader hex exists
if not exist "%BOOTLOADER_HEX%" (
    echo ERROR: Bootloader hex file not found: %BOOTLOADER_HEX%
    exit /b 1
)

REM Check if app hex exists
if not exist "%APP_HEX%" (
    echo ERROR: App hex file not found: %APP_HEX%
    exit /b 1
)

REM Run merge script
python "%SCRIPT_DIR%\merge_hex.py" "%BOOTLOADER_HEX%" "%APP_HEX%" "%MERGED_HEX%"
if errorlevel 1 (
    echo ERROR: Hex merge failed
    exit /b 1
)

REM Production flash image = bootloader + App0 only. The App1 slot starts empty
REM and is filled over the air.

REM ---------------------------------------------------------------------------
REM Generate the per-slot OTA update files (.cyacd2) so they are ready to send
REM via CySmart / the mobile app with no manual step. Each slot image carries the
REM version.h version (MAJOR*10000+MINOR*100+PATCH); send the .cyacd2 for the slot
REM the target device is NOT currently running. Failures here only WARN -- the
REM flashable image above is already written, so a .cyacd2 hiccup never blocks a
REM normal build/flash. Output: <OutputDir>\PainDrain_slot0.cyacd2 / _slot1.cyacd2
REM ---------------------------------------------------------------------------
echo.
echo Post-build: generating OTA .cyacd2 files (both slots)
python "%SCRIPT_DIR%\build_slot.py" 0
if errorlevel 1 echo WARNING: slot0 .cyacd2 generation failed
python "%SCRIPT_DIR%\build_slot.py" 1
if errorlevel 1 echo WARNING: slot1 .cyacd2 generation failed

echo Post-build complete

endlocal
exit /b 0
