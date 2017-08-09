:: =============================================================================
:: Jsong Windows build scripts.
::
:: Build the RapidJSON benchmark app.
::
:: Copyright Kristian Garnét.
:: -----------------------------------------------------------------------------

@echo off
cd /d "%~dp0"

:: Variables

set MARCH=-march=native

:: Create the build directory

if not exist build (
  mkdir build

  if %ERRORLEVEL% neq 0 (
    echo "Build error: couldn't create the build directory."
    exit /b %ERRORLEVEL%
  )
)

:: Build the RapidJSON benchmark app

g++ -O3 -mconsole %MARCH% ^
  -I%HOMEPATH%/Downloads/rapidjson-1.1.0/include ^
  -DRAPIDJSON_RECURSIVE -DRAPIDJSON_SSE42 rapid.cpp -o rapid 2> build\rapid.log
if %ERRORLEVEL% neq 0 (echo "Build error: rapid.cpp" && exit /b %ERRORLEVEL%)
