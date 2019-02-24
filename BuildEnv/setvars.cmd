@echo off

echo.
echo U365 Build Environment for Windows
echo (C) Haont 2019
echo.

set U365_Target=i686-elf
set U365_Toolset=ghost-%U365_Target%-tools
echo Toolset: %U365_Toolset%

set ToolSetFP=%~dp0%U365_Toolset%

rem echo %ToolSetFP%

set path=%path%;%ToolSetFP%;%ToolSetFP%\bin;%ToolSetFP%\%U365_Target%\bin
