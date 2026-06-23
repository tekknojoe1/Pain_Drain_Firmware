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

REM Production image = bootloader + App0 only. The App1 slot starts empty and
REM is filled over the air. To build a both-slots A/B test image, run
REM   python build_slot.py 1 --version 2
REM and merge PainDrain_slot1.hex in manually (see git history for the block).

echo Post-build complete

endlocal
exit /b 0
