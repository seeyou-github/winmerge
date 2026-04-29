pushd "%~dp0"
call BuildManual.cmd || goto :eof
call BuildBin.cmd %1 %2 || goto :eof
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || goto :eof
popd
call BuildTranslations.cmd || goto :eof
call BuildInstaller.cmd %1 %2 || goto :eof
call BuildArc.cmd %1 %2 || goto :eof
popd
