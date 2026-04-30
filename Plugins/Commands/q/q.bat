@echo off
if "%PROCESSOR_ARCHITECTURE%" == "AMD64" goto :next
if "%PROCESSOR_ARCHITEW6432%" == "AMD64" goto :next
if "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
  rem Check if the OS is Windows 11
  (ver | findstr /c:"Version 10.0.2") > NUL && goto :next
)
echo QueryCSV and QueryTSV plugins are only supported on x64 systems
exit /b 1

:next
set "Q_PATH=%~dp0q-AMD64-Windows.exe"
if not exist "%Q_PATH%" (
  echo q command is not installed. Network downloads are disabled in this build. 1>&2
  exit /b 1
)
"%Q_PATH%" %*
