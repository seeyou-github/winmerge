@echo off
where /q java.exe
if %ERRORLEVEL% == 0 (
  java %*
  goto :eof
)
set "OPENJDK_JAVA_PATH=%~dp0jdk-19.0.2\bin\java.exe"
if not exist "%OPENJDK_JAVA_PATH%" (
  echo Java is not installed. Network downloads are disabled in this build. 1>&2
  exit /b 1
)
"%OPENJDK_JAVA_PATH%" %*
