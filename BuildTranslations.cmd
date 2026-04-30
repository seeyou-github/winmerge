@echo off
pushd "%~dp0"
echo Prepare Translations...
rmdir /S /Q "Build\Translations" 2> NUL
mkdir "Build\Translations\WinMerge" 2> NUL
mkdir "Build\Translations\ShellExtension" 2> NUL
copy Translations\WinMerge\ChineseSimplified.po Build\Translations\WinMerge\ > NUL
copy Translations\ShellExtension\ChineseSimplified.po Build\Translations\ShellExtension\ > NUL
powershell.exe -NoProfile -ExecutionPolicy RemoteSigned -File "Translations\StripPoComments.ps1" "Build\Translations" || goto :eof
popd
