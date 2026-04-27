@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"
:restart

:: This is free and unencumbered software released into the public domain.
:: 
:: Anyone is free to copy, modify, publish, use, compile, sell, or
:: distribute this software, either in source code form or as a compiled
:: binary, for any purpose, commercial or non-commercial, and by any
:: means.

for %%a in (%*) do set "%%~a=1"
if not "%msvc%"=="1" if not "%gcc%"=="1" set msvc=1
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]
if "%msvc%"=="1"    set gcc=0 && echo [msvc compile]
if "%gcc%"=="1"     set msvc=0 && echo [gcc compile]

set cl_common=      /I..\src\ /nologo /FC /EHsc /Z7 /Zc:preprocessor /std:c++latest /permissive- /W4 /WX
set cl_debug=       cl /Od /Ob1 /DBUILD_DEBUG=1 %cl_common%
set cl_release=     cl /O2 /DBUILD_DEBUG=0 %cl_common%
set cl_link=        /link /MANIFEST:EMBED /INCREMENTAL:NO /opt:ref /opt:icf
set cl_out=         /out:

set gcc_common=    -I..\src\ -std=c++23 -Wall -Wextra -Werror -pedantic -Wno-missing-field-initializers -Wno-cast-function-type
set gcc_debug=     g++ -O0 -g -DBUILD_DEBUG=1 %gcc_common%
set gcc_release=   g++ -O2 -DNDEBUG -DBUILD_DEBUG=0 %gcc_common%
set gcc_link=      -lgdi32 -lopengl32 -luser32 -lstdc++exp
set gcc_out=       -o

if "%msvc%"=="1"    set compile_debug=%cl_debug%
if "%msvc%"=="1"    set compile_release=%cl_release%
if "%msvc%"=="1"    set compile_link=%cl_link%
if "%msvc%"=="1"    set out=%cl_out%

if "%gcc%"=="1"    set compile_debug=%gcc_debug%
if "%gcc%"=="1"    set compile_release=%gcc_release%
if "%gcc%"=="1"    set compile_link=%gcc_link%
if "%gcc%"=="1"    set out=%gcc_out%

if "%debug%"=="1"   set compile=%compile_debug%
if "%release%"=="1" set compile=%compile_release%

if not exist build mkdir build

pushd build
if "%runtime%"=="1"         set didbuild=1 && %compile% ..\src\main.cpp %compile_link% %out%lambda.exe || exit /b 1
popd

if "%didbuild%"=="" (
    echo [WARNING] no build target specified
    exit /b 1
)