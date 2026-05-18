@echo off
title NOTEIFY - C++ Note Manager Runner
color 0B
echo ========================================================
echo          NOTEIFY - C++ NOTE MANAGER RUNNER              
echo ========================================================
echo.

:: Automatically navigate to the directory where this BAT file is saved
cd /d "%~dp0"

:: Check if the executable exists
if exist ManageNoteSystem.exe (
    echo [INFO] Executable found! Launching program...
    echo.
    ManageNoteSystem.exe
) else (
    echo [WARNING] ManageNoteSystem.exe not found!
    echo [INFO] Compiling source files on-the-fly using g++...
    g++ -Wall -std=c++17 main.cpp User.cpp Note.cpp Notebook.cpp NoteService.cpp -o ManageNoteSystem.exe
    
    if exist ManageNoteSystem.exe (
        echo [SUCCESS] Compilation completed! Launching program...
        echo.
        ManageNoteSystem.exe
    ) else (
        echo [ERROR] Compilation failed! 
        echo Please ensure you have g++ installed and configured on your computer.
    )
)

echo.
echo ========================================================
echo   Session Ended. Thank you for using NOTEIFY!
echo ========================================================
pause
