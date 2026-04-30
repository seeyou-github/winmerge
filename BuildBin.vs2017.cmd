cd /d "%~dp0"

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.js Version.in > Version.h.tmp
if errorlevel 1 (
  echo Failed to generate Version.h.
  del Version.h.tmp > NUL 2> NUL
  exit /b 1
)
move /Y Version.h.tmp Version.h > NUL

setlocal
set "VSWHERE=%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo Visual Studio Installer vswhere.exe was not found.
  exit /b 1
)
for /f "tokens=*" %%i in ('call "%VSWHERE%" -version [15.0^,16.0^) -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do (
  set InstallDir=%%i
)
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  call "%InstallDir%\Common7\Tools\vsdevcmd.bat" || goto :eof
) else (
  echo Visual Studio 2017 Build Tools were not found.
  exit /b 1
)

if "%1" == "" (
  call :BuildBin ARM || goto :eof
  call :BuildBin ARM64 || goto :eof
  call :BuildBin x86|| goto :eof
  call :BuildBin x64 || goto :eof
) else (
  call :BuildBin %1 || goto :eof
)

goto :eof

:BuildBin
MSBuild WinMerge.vs2017.sln /t:Rebuild /p:Configuration="Release" /p:Platform="%1" || goto :eof
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" Build\%1\Release\WinMergeU.exe
)

mkdir Build\%1\Release\%APPVER% 2> NUL
copy Build\%1\Release\*.pdb "Build\%1\Release\%APPVER%\"
goto :eof
