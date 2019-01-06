@echo off

IF NOT EXIST ./build mkdir ./build
cd build

set FLAGS=-DPLATFORM_WINDOWS=1
set CommonCompilerFlags=-MTd -Gm- -nologo -GR- -EHa- -Oi  -W4 -wd4201 -wd4100 -wd4127 -wd4505 -wd4189 -fp:fast  /Z7 

set CommonLinkerFlags=user32.lib Gdi32.lib winmm.lib Ole32.lib

cl  %FLAGS% %CommonCompilerFlags% W:/hot_code_reload/hot_code_reload_launcher.cpp /link    %CommonLinkerFlags% -incremental:no

cd ..




