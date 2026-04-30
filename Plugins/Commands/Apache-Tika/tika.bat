@echo off
setlocal EnableDelayedExpansion
set "TikaJar=tika-app-3.0.0.jar"
set "TIKA_PATH=%~dp0%TikaJar%"
if not exist "%TIKA_PATH%" (
  echo Apache Tika is not installed. Network downloads are disabled in this build. 1>&2
  exit /b 1
)
set TEMP_FILE=%TEMP%\tempfile_%RANDOM%%~x1
chcp 1252 >NUL
(echo "%~1") | findstr /C:"%~1" >NUL
if errorlevel 1 (
  if /i "%~1"=="%~s1" (
    mklink /h "%TEMP_FILE%" "%~1" >NUL 2>NUL
    if errorlevel 1 (
      copy "%~1" "%TEMP_FILE%" >NUL
    )
    call "%~dp0..\Java\java.bat" -jar "%TIKA_PATH%" %3 %4 %5 %6 %7 %8 %9 "%TEMP_FILE%" > "%~2"
    del "%TEMP_FILE%" >NUL
  ) else (
    call "%~dp0..\Java\java.bat" -jar "%TIKA_PATH%" %3 %4 %5 %6 %7 %8 %9 "%~s1" > "%~2"
  )
) else (
  call "%~dp0..\Java\java.bat" -jar "%TIKA_PATH%" %3 %4 %5 %6 %7 %8 %9 "%~1" > "%~2"
)
