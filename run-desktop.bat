:: main and all of src
gcc main.c src/*.c -o build/desktop/template.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -I src/ -L lib/desktop/ -lraylib -lopengl32 -lgdi32 -lwinmm

:: only main
:: gcc main.c -o build/desktop/template.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -L lib/desktop -lraylib -lopengl32 -lgdi32 -lwinmm

xcopy resources build\desktop\resources /I /Y