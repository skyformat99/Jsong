:: =============================================================================
:: Jsong Windows build scripts.
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

:: C++ compatibility test

g++ -mconsole %MARCH% ^
  -I%QUANTUM% -I%ULTRAVIOLET% -I%JSONG% ^
  -c compat.cpp -o build\compat.o 2> build\compat.log
if %ERRORLEVEL% neq 0 (echo "Build error: compat.cpp" && exit /b %ERRORLEVEL%)
