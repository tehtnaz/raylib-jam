rmdir /S /Q build\
mkdir build\desktop
mkdir build\web
call run-desktop.bat
call run-web.bat
7z a build\desktop.zip build\desktop\
7z a build\web.zip build\web\