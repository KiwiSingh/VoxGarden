@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Install.ps1"
set "vox_install_exit=%errorlevel%"
pause
exit /b %vox_install_exit%
