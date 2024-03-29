@echo off

ctime -begin handmade_hero.ctm

set CommonCompilerFlags=-O2 -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4244 -wd4456 -wd4477 -wd4457 -wd4312 -wd4838 -FC -Z7
set CommonCompilerFlags=-DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 %CommonCompilerFlags% 
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib opengl32.lib

IF "%HANDMADE_STREAMING%"=="" goto nostreaming
set CommonCompilerFlags=%CommonCompilerFlags% -DHANDMADE_STREAMING=1
:nostreaming

REM TODO - can we just build both with one exe?

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

del *.pdb > NUL 2> NUL

REM Simple preprocessor
REM cl %CommonCompilerFlags% -D_CRT_SECURE_NO_WARNINGS ..\handmade\code\simple_preprocessor.cpp /link %CommonLinkerFlags%
REM pushd ..\handmade\code
REM ..\..\build\simple_preprocessor.exe > handmade_generated.h
REM popd

REM Asset file builder build
REM cl %CommonCompilerFlags% -D_CRT_SECURE_NO_WARNINGS ..\handmade\code\test_asset_builder.cpp /link %CommonLinkerFlags%

REM 32-bit build
REM cl %CommonCompilerFlags% ..\handmade\code\win32_handmade.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
REM Optimization switches /wO2
echo WAITING FOR PDB > lock.tmp
cl %CommonCompilerFlags% -I..\iaca-win64\ ..\handmade\code\handmade.cpp -Fmhandmade.map -LD /link -incremental:no -opt:ref -PDB:handmade_%random%.pdb -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender -EXPORT:DEBUGGameFrameEnd
set LastError=%ERRORLEVEL%
del lock.tmp
cl %CommonCompilerFlags% ..\handmade\code\win32_handmade.cpp -Fmwin32_handmade.map /link %CommonLinkerFlags%
popd

ctime -end handmade_hero.ctm %LastError%
