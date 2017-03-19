@echo off

call "c:\program files (x86)\Microsoft Visual Studio 14.0\vc\vcvarsall" x64

mkdir ..\_build
pushd ..\_build
cl -Zi ..\code\win32_handmade.cc user32.lib
popd


