pushd src
gcc -o../bin/main.exe -std=c99 ^
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
-I../include -L../lib -lSDL2
popd
