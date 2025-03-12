@echo off

cd %~dp0

set REL_VIEW_ROOT=..\..\..\..
set FPGA_INCLUDE_PATH=%REL_VIEW_ROOT%\m48_fpga\ip_library\platform_ip

cd %FPGA_INCLUDE_PATH%
set FPGA_IP_SEARCH_PATH=%cd%
cd %~dp0

set USER_COMPONENTS_DIR="%HOME%"\.altera.quartus\ip\21.1\ip_search_path
set USER_COMPONENTS_FILE=%USER_COMPONENTS_DIR%\user_components.ipx

if exist %USER_COMPONENTS_FILE% (
	echo Patching %USER_COMPONENTS_FILE% ...
	move /Y %USER_COMPONENTS_FILE% %USER_COMPONENTS_FILE:.ipx=.bak% >nul
	IF %ERRORLEVEL% NEQ 0 EXIT /B 1
) else (
	if not exist %USER_COMPONENTS_DIR% (
		mkdir %USER_COMPONENTS_DIR%
	)
	echo Creating %USER_COMPONENTS_FILE% ...
)

echo ^<?xml version="1.0" encoding="UTF-8"?^> > %USER_COMPONENTS_FILE%
echo ^<library^> >> %USER_COMPONENTS_FILE%
echo ^<path path="%FPGA_IP_SEARCH_PATH:\=/%/**/*" /^> >> %USER_COMPONENTS_FILE%
echo ^</library^> >> %USER_COMPONENTS_FILE%