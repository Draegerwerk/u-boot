@echo off

echo Select type: 'A2' (small FPGA) or 'A4' (large FPGA, default)
set /P type=Enter type:
set SCRIPT_START_DIR=%cd%
cd %~dp0

:: Get root path to pass it to the tcl script
cd %~dp0../
SET FPGA_ROOT_PATH=%cd%
cd %~dp0../../../../
set REPO_ROOT_PATH=%cd%
cd %~dp0

:: Paths and constants
SET PROJECT=M48Test
set PROJECT_ID="M48Test"

SET REL_VIEW_ROOT=..\..\..\..
SET REL_VIEW_ROOT_FROM_BUILD=..\..\..\..\..

SET BUILD_DIR=%REPO_ROOT_PATH%\bld\bin\fpgaBuild\%PROJECT%
set FPGA_IMAGE_SOURCE=%BUILD_DIR%\quartus\output_files\m48_default_system.rbf
if %type%==A2 (set FPGA_IMAGE_TARGET=%FPGA_ROOT_PATH%\bin\m48_cv_se_a2.img) else (set FPGA_IMAGE_TARGET=%FPGA_ROOT_PATH%\bin\m48_default_system.img)

:: Prepare output directory
echo Preparing build directory at %BUILD_DIR%...
rd /s /q %BUILD_DIR% >nul
md %BUILD_DIR%\quartus
md %BUILD_DIR%\scripts
echo Copying tcl scripts to build directory...
copy create_quartus_project.tcl %BUILD_DIR%\scripts\create_quartus_project.tcl >nul
if %type%==A2 (copy m48_cv_se_a2.tcl %BUILD_DIR%\scripts\m48_default_system.tcl >nul) else (copy m48_default_system.tcl %BUILD_DIR%\scripts\m48_default_system.tcl >nul)
if %type%==A2 (copy ..\quartus\m48_cv_se_a2.qsys %BUILD_DIR%\quartus\m48_soc.qsys >nul) else (copy ..\quartus\m48_soc.qsys %BUILD_DIR%\quartus\m48_soc.qsys >nul)

:: Get quartus
IF "%quartus_DIR%" == "" (
CALL "%REL_VIEW_ROOT%\plain\tools\local_tools.cmd" --makefile-dir "%BUILD_DIR%\scripts" Quartus
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
CALL "%BUILD_DIR%\scripts\tools_env_Quartus.cmd"
)

:: Set parameters for quartus
set QUARTUS_ROOTDIR=%quartus_DIR%\quartus
set QSYS_ROOTDIR=%quartus_DIR%\quartus\sopc_builder\bin

:: Prepare the ip search path in the local home dir
echo Checking ipx file for ip search path definition...
CALL %~dp0prepareIpSearchPath.cmd

:: Build FPGA
echo Invoking Quartus...
cd %BUILD_DIR%\quartus
%QUARTUS_ROOTDIR%\bin64\quartus_sh -t ../scripts/create_quartus_project.tcl
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
%QSYS_ROOTDIR%\qsys-generate m48_soc.qsys --synthesis=VERILOG --search-path=%REPO_ROOT_PATH%\m48_fpga\ip_library\platform_ip,$
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
%QUARTUS_ROOTDIR%\bin64\quartus_sh -t ../scripts/m48_default_system.tcl %FPGA_ROOT_PATH:\=/%
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

:: Pack FPGA image
echo Packing FPGA image...
:: Get mkimage path from wrenv.exe and save it to variable
call %REPO_ROOT_PATH%\plain\tools\drama.cmd ^
     %REPO_ROOT_PATH%\m48_fpga\fpgaMkimage.cmake ^
     -DPROJECT_ID=%PROJECT_ID% -DFPGA_IMAGE_SOURCE=%FPGA_IMAGE_SOURCE% -DFPGA_IMAGE_TARGET=%FPGA_IMAGE_TARGET%

IF %ERRORLEVEL% NEQ 0 GOTO ERROR

echo FPGA image created successfully...
pause
cd %SCRIPT_START_DIR%
exit /B 0

:ERROR
echo Failed!
pause
cd %SCRIPT_START_DIR%
exit /B 1