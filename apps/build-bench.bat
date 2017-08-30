:: =============================================================================
:: Jsong Windows build scripts.
::
:: Build the benchmark app.
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
set VALIDWHITESPACE=-DJSON_VALID_WHITESPACE

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

:: Build the DOM API parsers

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %VALIDWHITESPACE% %JSONG%/json/parse.c -o build\jsonz.o 2> build\jsonz.log
if %ERRORLEVEL% neq 0 (echo "Build error: jsonz.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c -DJSON_EXPLICIT %VALIDWHITESPACE% %JSONG%/json/parse.c -o build\jsonb.o 2> build\jsonb.log
if %ERRORLEVEL% neq 0 (echo "Build error: jsonb.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c -DJSON_STREAM %VALIDWHITESPACE% %JSONG%/json/parse.c -o build\jsons.o 2> build\jsons.log
if %ERRORLEVEL% neq 0 (echo "Build error: jsons.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/dom.c -o build\dom.o 2> build\dom.log
if %ERRORLEVEL% neq 0 (echo "Build error: dom.o" && exit /b %ERRORLEVEL%)

:: Build the SAX API parsers

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c -DJSON_SAX %VALIDWHITESPACE% %JSONG%/json/parse.c -o build\jsaxz.o 2> build\jsaxz.log
if %ERRORLEVEL% neq 0 (echo "Build error: jsaxz.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c -DJSON_SAX -DJSON_EXPLICIT %VALIDWHITESPACE% %JSONG%/json/parse.c -o build\jsaxb.o 2> build\jsaxb.log
if %ERRORLEVEL% neq 0 (echo "Build error: jsaxb.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c -DJSON_SAX -DJSON_STREAM %VALIDWHITESPACE% %JSONG%/json/parse.c -o build\jsaxs.o 2> build\jsaxs.log
if %ERRORLEVEL% neq 0 (echo "Build error: jsaxs.o" && exit /b %ERRORLEVEL%)

gcc -O3 %MARCH% -fno-align-labels -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/sax.c -o build\sax.o 2> build\sax.log
if %ERRORLEVEL% neq 0 (echo "Build error: sax.o" && exit /b %ERRORLEVEL%)

:: Error messages

gcc -O3 %MARCH% -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c %JSONG%/json/utils/errors.c -o build\errors.o 2> build\errors.log
if %ERRORLEVEL% neq 0 (echo "Build error: errors.o" && exit /b %ERRORLEVEL%)

:: Build the app

gcc -O3 %MARCH% -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c bench.c -o build\bench.o 2> build\bench.log
if %ERRORLEVEL% neq 0 (echo "Build error: bench.o" && exit /b %ERRORLEVEL%)

:: Link the executable

gcc -mconsole ^
  build\jsonz.o build\jsonb.o build\jsons.o build\dom.o ^
  build\jsaxz.o build\jsaxb.o build\jsaxs.o build\sax.o ^
  build\io.o build\errors.o build\bench.o ^
  -o bench -lmingw32 -lmsvcr120
