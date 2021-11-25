rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"

rmdir /s /q .\Client\default\Debug
rmdir /s /q .\Client\default\ipch
rmdir /s /q .\Client\default\Release
rmdir /s /q .\Client\default\x64

rmdir /s /q .\Engine\default\Debug
rmdir /s /q .\Engine\default\x64

rmdir /s /q .\Tool\default\x64

del /f /q .\Client\bin\Client.exe
del /f /q .\Client\bin\Client.ilk
del /f /q .\Client\bin\Client.pdb

del /f /q MySolution.VC.db
