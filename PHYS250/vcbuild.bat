pushd src
cl /Fe../bin/main.exe /Fo../bin/ /EHsc ^
 ^
engine\render\window.c ^
engine\render\surface.c ^
engine\input\input.c ^
engine\input\button.c ^
engine\input\keyboard.c ^
engine\core\coreengine.c ^
engine\io\stb_image.c ^
 ^
main.c ^
 ^
/I../include ../lib/SDL2.lib
popd
