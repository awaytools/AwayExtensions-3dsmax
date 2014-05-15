@echo off
echo.
set programmname=3dsmax 2011 32 bit
echo AwayExtensions-3dsmax Installer for %programmname%
echo.
echo.
echo Detected pathes:
echo.
set programmpath=%ProgramFiles%\Autodesk\3ds Max 2011\

iF DEFINED ProgramFiles(x86) (set programmpath="%ProgramFiles(x86)%\Autodesk\3ds Max 2011\")

::Do not use the env var for 2011 as it starts with a Number, and therefore does not work
::IF DEFINED 3DSMAX_2011_PATH (set programmpath=%3DSMAX_2011_PATH%)
set programmpath=%programmpath:"=%
set exportername=maxawdexporter_2011_32.dle
set toolbarname=AWDToolBar_2011_2012.CUI
set uninstallername=uninstall_awayextensions3dsmax_2011_x32.cmd

if EXIST "%programmpath%" (echo  - programm:  "%programmpath%") ELSE (goto exitNo3dspath)
if not EXIST "%programmpath%3dsmax.exe" (goto exitNo3dsexe)
set pluginsPath=%programmpath%plugins\
if EXIST "%pluginsPath%" (echo  - plugins:  "%pluginsPath%") ELSE (goto exitNoPluginsPath)
set localUIpath=%LOCALAPPDATA%\Autodesk\3dsMax\2011 - 32bit\enu\UI\
if EXIST "%localUIpath%" (echo  - local-UI: "%localUIpath%") ELSE (goto exitNoLocalUI)
set macrosPath=%localUIpath%usermacros\
if EXIST "%macrosPath%" (echo  - usermacros: local-UI + "\usermacros") ELSE (goto exitNoUserMacros)
set iconsPath=%localUIpath%usericons\
if EXIST "%iconsPath%" (echo  - usericon:   local-UI + "\usericons") ELSE (goto exitNoUserIcons)
set uiPath=%programmpath%UI\
if EXIST "%uiPath%" (echo  - UI: "%uiPath%") ELSE (goto exitNoUI)

echo.
echo.
goto checkif3dsmaxruns
exit /b

:checkIfExists
   if EXIST "%macrosPath%\AWDCommands\" (goto askForUninstall)
   if EXIST "%pluginsPath%AwayExtensions3dsMax\" (goto askForUninstall)
   if EXIST "%pluginsPath%%exportername%" (goto askForUninstall)
   if EXIST "%iconsPath%\AwayExtensionIcons_16a.bmp" (goto askForUninstall)
   if EXIST "%iconsPath%\AwayExtensionIcons_16i.bmp" (goto askForUninstall)
   if EXIST "%iconsPath%\AwayExtensionIcons_24a.bmp" (goto askForUninstall)
   if EXIST "%iconsPath%\AwayExtensionIcons_24i.bmp" (goto askForUninstall)
   if EXIST "%iconsPath%\AWDAbout.bmp" (goto askForUninstall)
   if EXIST "%uiPath%%toolbarname%" (goto askForUninstall)

   set /p answer=Install AwayExtensions for %programmname% (Y/N)?
   if /i "%answer:~,1%" EQU "Y" goto install
   exit /b

:askForUninstall
   set /p answer=What action should be performed (I=Install/Update/Repair R=Remove E=Exit)?
   if /i "%answer:~,1%" EQU "I" goto install
   if /i "%answer:~,1%" EQU "R" goto remove
   if /i "%answer:~,1%" EQU "E" exit /b
   echo No valid input!
   goto askForUninstall


:checkif3dsmaxruns
   tasklist /FI "imagename eq 3dsmax.exe" 2>NUL | find /I /N "3dsmax.exe">NUL && (goto asktoexit3dsmax)||(goto checkIfExists)


:asktoexit3dsmax
   echo 3dsmax appears to be running. Please exit all running 3dsmax-applications!
   pause
   tasklist /FI "imagename eq 3dsmax.exe" 2>NUL | find /I /N "3dsmax.exe">NUL && (goto asktoexit3dsmax)||(goto checkIfExists)


:install

   echo.
   echo Start Installation
   echo.
   echo      Remove existing files (uninstall)
   echo.
   if EXIST "%macrosPath%\AWDCommands\" (rmdir /s /q "%macrosPath%\AWDCommands\")
   if EXIST "%iconsPath%\AwayExtensionIcons_16a.bmp" (del /q "%iconsPath%\AwayExtensionIcons_16a.bmp")
   if EXIST "%iconsPath%\AwayExtensionIcons_16i.bmp" (del /q "%iconsPath%\AwayExtensionIcons_16i.bmp")
   if EXIST "%iconsPath%\AwayExtensionIcons_24a.bmp" (del /q "%iconsPath%\AwayExtensionIcons_24a.bmp")
   if EXIST "%iconsPath%\AwayExtensionIcons_24i.bmp" (del /q "%iconsPath%\AwayExtensionIcons_24i.bmp")
   if EXIST "%iconsPath%\AWDAbout.bmp" (del /q "%iconsPath%\AWDAbout.bmp")
   if EXIST "%pluginsPath%AwayExtensions3dsMax\" (rmdir /s /q "%pluginsPath%AwayExtensions3dsMax\")
   if EXIST "%pluginsPath%%exportername%" (del /q "%pluginsPath%%exportername%") 
   if EXIST "%uiPath%%toolbarname%" (del /q "%uiPath%%toolbarname%")
   echo      Copy new files (install)
   echo.
   echo Copy Folder 'AwayExtensions3dsMax' to the 3dsmax/plugin folder
   xcopy /q/s/e/y/x "%~dp0\AwayExtensions3dsMax" "%pluginsPath%"
   echo.
   echo Copy File 'AWDExporter\%exportername%' to the plugin folder
   xcopy /q/y/x "%~dp0\AWDExporter\%exportername%" "%pluginsPath%"
   echo.
   echo Copy File 'AWDUninstaller\%uninstallername%' to the plugin folder
   xcopy /q/y/x "%~dp0\AWDUninstaller\%uninstallername%" "%pluginsPath%\AwayExtensions3dsMax"
   echo.
   echo Copy Folder 'AWDCommands' to the usermacros folder
   xcopy /q/s/e/y/x "%~dp0\AWDCommands" "%macrosPath%"
   echo.
   echo Copy Files from folder 'AWDIcons' to the usericons folder
   xcopy /q/s/e/y/x "%~dp0\AWDIcons" "%iconsPath%"
   echo.
   echo Copy File 'AWDToolbars\%toolbarname%' to the 3dsmax/UI folder
   xcopy /q/y/x "%~dp0\AWDToolbars\%toolbarname%" "%uiPath%"
   echo.
   echo      Installation complete.
   echo.
   set /p answer=Press ENTER to close this window and open %programmname%...   
   start "" "%programmpath%\3dsmax.exe"
   exit /b

:remove
   echo.
   echo      Start Uninstallation
   if EXIST "%macrosPath%\AWDCommands\" (rmdir /s /q "%macrosPath%\AWDCommands\") ELSE (echo "Folder: AWDCommands could not be found in usermacros")
   if EXIST "%iconsPath%\AwayExtensionIcons_16a.bmp" (del /q "%iconsPath%\AwayExtensionIcons_16a.bmp") ELSE (echo "File: AwayExtensionIcons_16a.bmp could not be found in usericons")
   if EXIST "%iconsPath%\AwayExtensionIcons_16i.bmp" (del /q "%iconsPath%\AwayExtensionIcons_16i.bmp") ELSE (echo "File: AwayExtensionIcons_16i.bmp could not be found in usericons")
   if EXIST "%iconsPath%\AwayExtensionIcons_24a.bmp" (del /q "%iconsPath%\AwayExtensionIcons_24a.bmp") ELSE (echo "File: AwayExtensionIcons_24a.bmp could not be found in usericons")
   if EXIST "%iconsPath%\AwayExtensionIcons_24i.bmp" (del /q "%iconsPath%\AwayExtensionIcons_24i.bmp") ELSE (echo "File: AwayExtensionIcons_24i.bmp could not be found in usericons")
   if EXIST "%iconsPath%\AWDAbout.bmp" (del /q "%iconsPath%\AWDAbout.bmp")ELSE (echo "File: AWDAbout.bmp could not be found in usericons")
   if EXIST "%pluginsPath%AwayExtensions3dsMax\" (rmdir /s /q "%pluginsPath%AwayExtensions3dsMax\") ELSE (echo "Folder: AwayExtensions3dsMax could not be found in plugin folder")
   if EXIST "%pluginsPath%%exportername%" (del /q "%pluginsPath%%exportername%") ELSE (echo "File: %exportername% could not be found in plugin folder")
   if EXIST "%uiPath%%toolbarname%" (del /q "%uiPath%%toolbarname%") ELSE (echo "File: %toolbarname% could not be found in the UI folder")
   echo.
   echo      Uninstallation complete
   echo.
   set /p answer=Press ENTER to close this window... 
   exit /b
   
:exitNo3dspath
   echo !!!Could not find the 3dsmax system path. Installation cancelled!!!
   set /p answer=Press ENTER to close this window... 
   exit /b
:exitNoPluginsPath
   echo !!!Could not find the 3dsmax plugin path. Installation cancelled!!!
   set /p answer=Press ENTER to close this window... 
   exit /b
:exitNoUserMacros
   echo !!!Could not find the 3dsmax usermacros folder. Installation cancelled!!!
   set /p answer=Press ENTER to close this window... 
   exit /b
:exitNoUserIcons
   echo !!!Could not find the 3dsmax userIcons folder. Installation cancelled!!!
   set /p answer=Press ENTER to close this window... 
   exit /b
:exitNoUI
   echo !!!Could not find the 3dsmax UI path. Installation cancelled!!!
   set /p answer=Press ENTER to close this window... 
   exit /b
:exitNoLocalUI
   echo !!!Could not find the local 3dsmax UI path. Installation cancelled!!!
   set /p answer=Press ENTER to close this window... 
   exit /b
:exitNo3dsexe
   echo !!!Could not find the 3dsmax.exe for %programmname%!!!
   set /p answer=Press ENTER to close this window... 
   exit /b
   
