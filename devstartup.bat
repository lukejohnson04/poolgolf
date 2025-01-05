@echo off
call "%VCVARS_PATH%\vcvarsall.bat" x64
call "misc/emacs.bat"
cd build
cmd /k