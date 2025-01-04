net session >nul 2>&1
if %errorlevel% neq 0 (
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)
sc stop AntiRansomKernel
del C:\Windows\System32\drivers\AntiRansomKernel.sys
copy E:\AntiRansomKernel.sys C:\Windows\System32\drivers
sc start AntiRansomKernel
sc query AntiRansomKernel
pause