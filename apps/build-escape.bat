:: =============================================================================
:: Jsong Windows build scripts.
::
:: Build the escaper app.
::
:: Copyright Kristian Garnét.
:: -----------------------------------------------------------------------------

@echo off
cd /d "%~dp0"

:: Variables

set MARCH=-march=native
set QUANTUM=%CREATIVE%\QUANTUM
set ULTRAVIOLET=%CREATIVE%\Ultraviolet
set JSONG=%CREATIVE%\Jsong

:: Create the build directory

if not exist build (
  mkdir build

  if %ERRORLEVEL% neq 0 (
    echo "Build error: couldn't create the build directory."
    exit /b %ERRORLEVEL%
  )
)

:: Build the JSON processing utilities

gcc -O3 %MARCH% -fno-align-labels ^
  -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/utils/escape.c -o build\utils.o 2> build\utils.log
if %ERRORLEVEL% neq 0 (echo "Build error: utils.o" && exit /b %ERRORLEVEL%)

:: Build the app

gcc -O3 %MARCH% ^
  -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c escape.c -o build\escape.o 2> build\escape.log
if %ERRORLEVEL% neq 0 (echo "Build error: escape.o" && exit /b %ERRORLEVEL%)

:: Link the executable

gcc -mconsole ^
  build\utils.o build\escape.o ^
  -o escape -lmingw32 -lmsvcr120
