@echo off

cd %~dp0

:: Get root path to pass it to the script
cd %~dp0../
set FPGA_ROOT_PATH=%cd%
cd %~dp0

:: Paths and constants
set PROJECT=M48Test
set SRC=%FPGA_ROOT_PATH%\srcPreloader
set REL_VIEW_ROOT=..\..\..\..
set BUILD_DIR_PREL=%REL_VIEW_ROOT%\bld\bin\UBoot\Up2\%PROJECT%
set BSP_SETTINGS_PATH=%REL_VIEW_ROOT%\bld\bin\fpgaBuild\Ax5x\quartus\software
set QUARTUS_ROOTDIR=

:: Get preloadgen (included in quartus package)
IF "%AlteraPreloadergen_DIR%" == "" (
CALL "%REL_VIEW_ROOT%\plain\tools\local_tools.cmd" -i "%REL_VIEW_ROOT%\tools\utils\Make\tools\toolsConfig.ini" --makefile-dir "%BUILD_DIR%\scripts" Quartus
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
CALL "%BUILD_DIR%\scripts\tools_env_Quartus.cmd"
)

:: Generate preloader sources
echo Generating preloader sources...
rmdir /Q /S %BSP_SETTINGS_PATH%
mkdir %BSP_SETTINGS_PATH%
copy settings.bsp %BSP_SETTINGS_PATH% >nul
cd %BSP_SETTINGS_PATH%
%AlteraPreloadergen_DIR%\host_tools\altera\preloadergen\bsp-generate-files.exe --bsp-dir . --settings settings.bsp --silent
IF %ERRORLEVEL% NEQ 0 (
goto ERROR
)
robocopy %BSP_SETTINGS_PATH%\generated %FPGA_ROOT_PATH%\srcPreloader *.c *.h /s /NFL /NDL /NJH /NJS /nc /ns /np
cd %~dp0

:: Prepare output directory
echo Preparing build directory at %BUILD_DIR_PREL%...
rmdir /Q /S %REL_VIEW_ROOT%\bld\bin\UBoot\Up2
del %BUILD_DIR_PREL%\preloader-mkpimage.bin

:: Build Preloader
echo Build Preloader...
call %REL_VIEW_ROOT%\u-boot\draeger\build\mkuboot.cmd -up2 [-spl] -dev %PROJECT% -fpga %SRC%
IF %ERRORLEVEL% NEQ 0 (
goto ERROR
)

copy %BUILD_DIR_PREL%\preloader-mkpimage.bin %FPGA_ROOT_PATH%\bin\preloader-mkpimage.bin
echo Preloader created successfully...

pause
exit /B 0

:ERROR
echo Failed!
pause
exit /B 1