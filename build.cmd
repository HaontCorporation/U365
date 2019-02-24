@echo off

call BuildEnv\setvars.cmd

echo.
echo Building U365...
echo ================
echo.

make

pause
