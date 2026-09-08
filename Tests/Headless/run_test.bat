@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cl /nologo /EHsc /std:c++17 Tests\Headless\test_formations.cpp /Fe:Tests\Headless\runner.exe /Fo:Tests\Headless\test_formations.obj
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    exit /b %ERRORLEVEL%
)
Tests\Headless\runner.exe
