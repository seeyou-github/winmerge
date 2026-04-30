@echo off
setlocal enabledelayedexpansion
set "CACHE_DIR=%~dp0..\..\AppData\Data\Commands\dumpbin"
set "CACHE_FILE=%CACHE_DIR%\dumpbinpath.txt"
if exist "%CACHE_FILE%" (
  for /f "usebackq tokens=*" %%i in ("%CACHE_FILE%") do set "DUMPBIN_PATH=%%i"
)
if not exist "!DUMPBIN_PATH!" (
  if exist "%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" (
    for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -latest -products * -property installationPath`) do (
      set "InstallDir=%%i"
    )
  )
  if exist "!InstallDir!\Common7\Tools\vsdevcmd.bat" (
    call "!InstallDir!\Common7\Tools\vsdevcmd.bat" > NUL
  ) else (
    echo Visual Studio not installed
    exit /b 1
  )
  mkdir "%CACHE_DIR%" 2> NUL
  where dumpbin.exe > "%CACHE_FILE%"
  if exist "%CACHE_FILE%" (
    for /f "usebackq tokens=*" %%i in ("%CACHE_FILE%") do set "DUMPBIN_PATH=%%i"
  )
)
"!DUMPBIN_PATH!" %*
