:: =============================================================================
:: Jsong Windows build scripts.
::
:: Build the console app.
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

set BIG_NUMBERS=-DJSON_BIG_NUMBERS
set LIBQUADMATH=-lquadmath

:: Create the build directory

if not exist build (
  mkdir build

  if %ERRORLEVEL% neq 0 (
    echo "Build error: couldn't create the build directory."
    exit /b %ERRORLEVEL%
  )
)

:: I/O routines

gcc -O3 %MARCH% -I%QUANTUM% ^
  -c %QUANTUM%\quantum\io.c -o build\io.o 2> build\io.log
if %ERRORLEVEL% neq 0 (echo "Build error: io.o" && exit /b %ERRORLEVEL%)

:: Build the DOM API parser

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %BIG_NUMBERS% %JSONG%/json/parse.c -o build\json.o 2> build\json.log
if %ERRORLEVEL% neq 0 (echo "Build error: json.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %BIG_NUMBERS% %JSONG%/json/dom.c -o build\dom.o 2> build\dom.log
if %ERRORLEVEL% neq 0 (echo "Build error: dom.o" && exit /b %ERRORLEVEL%)

:: Build the JSON processing utilities

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/utils/uncomment.c -o build\uncomment.o 2> build\uncomment.log
if %ERRORLEVEL% neq 0 (echo "Build error: uncomment.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/utils/escape.c -o build\escape.o 2> build\escape.log
if %ERRORLEVEL% neq 0 (echo "Build error: escape.o" && exit /b %ERRORLEVEL%)

:: Build the DOM JSON serializer

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %BIG_NUMBERS% %JSONG%/json/serialize/dom.c -o build\serialize.o 2> build\serialize.log
if %ERRORLEVEL% neq 0 (echo "Build error: serialize.o" && exit /b %ERRORLEVEL%)

:: Error messages

gcc -O3 %MARCH% -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/utils/errors.c -o build\errors.o 2> build\errors.log
if %ERRORLEVEL% neq 0 (echo "Build error: errors.o" && exit /b %ERRORLEVEL%)

:: Build the app

gcc -O3 %MARCH% -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %BIG_NUMBERS% json.c -o build\console.o 2> build\console.log
if %ERRORLEVEL% neq 0 (echo "Build error: console.o" && exit /b %ERRORLEVEL%)

:: Link everything together

gcc -mconsole ^
  build\json.o build\dom.o build\uncomment.o build\escape.o build\serialize.o ^
  build\errors.o build\io.o build\console.o -o json %LIBQUADMATH% -lmingw32 -lmsvcr120
