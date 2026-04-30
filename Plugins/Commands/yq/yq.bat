@echo off
set "YQ_PATH=%~dp0yq_windows_386.exe"
if not exist "%YQ_PATH%" (
  echo yq command is not installed. Network downloads are disabled in this build. 1>&2
  exit /b 1
)
"%YQ_PATH%" %*
