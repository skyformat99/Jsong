:: =============================================================================
:: Jsong Windows build scripts.
::
:: Clean up the build environment.
::
:: Copyright Kristian Garnét.
:: -----------------------------------------------------------------------------

@echo off
cd /d "%~dp0"

if exist build (
  del "build\*.o"
  del "build\*.log"

  rmdir build
)
