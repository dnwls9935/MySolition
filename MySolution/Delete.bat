rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"
rmdir /s /q ".vs"

rmdir /s /q .\Client\default\x64
rmdir /s /q .\Engine\default\x64
rmdir /s /q .\Tool\x64

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb
del /f /q FrameWork.VC.db
