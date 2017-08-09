:: =============================================================================
:: Jsong Windows build scripts.
::
:: Build the formatter app.
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

:: Build the SAX API parser

gcc -O3 %MARCH% -fno-align-labels ^
  -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c -DJSON_SAX -DJSON_STREAM -DJSON_STRING_NUMBERS -DJSON_LINE_COL ^
  %JSONG%/json/json.c -o build\jsax.o 2> build\jsax.log
if %ERRORLEVEL% neq 0 (echo "Build error: jsax.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/sax.c -o build\sax.o 2> build\sax.log
if %ERRORLEVEL% neq 0 (echo "Build error: sax.o" && exit /b %ERRORLEVEL%)

:: Build the JSON processing utilities

gcc -O3 %MARCH% -fno-align-labels ^
  -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/utils.c -o build\utils.o 2> build\utils.log
if %ERRORLEVEL% neq 0 (echo "Build error: utils.o" && exit /b %ERRORLEVEL%)

:: Build the SAX JSON serializer

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/serialize/sax.c -o build\serialize.o 2> build\serialize.log
if %ERRORLEVEL% neq 0 (echo "Build error: serialize.o" && exit /b %ERRORLEVEL%)

:: Error messages

gcc -O3 %MARCH% -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/utils/errors.c -o build\errors.o 2> build\errors.log
if %ERRORLEVEL% neq 0 (echo "Build error: errors.o" && exit /b %ERRORLEVEL%)

:: Build the app

gcc -O3 %MARCH% ^
  -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c jsax.c -o build\formatter.o 2> build\formatter.log
if %ERRORLEVEL% neq 0 (echo "Build error: formatter.o" && exit /b %ERRORLEVEL%)

:: Link the executable

gcc -mconsole ^
  build\jsax.o build\sax.o build\utils.o build\serialize.o build\errors.o ^
  build\formatter.o -o jsax -lmingw32 -lmsvcr120
