@echo off
rem Builds, tests and packages Cutline on Windows with one command, after checking that every
rem prerequisite is installed. Run it from any command prompt:
rem
rem     scripts\build-windows.bat            build, test, install into dist\ and start the app
rem     scripts\build-windows.bat --no-run   the same, without starting the app
rem
rem Optional environment variables:
rem     QT_DIR          Qt's msvc2022_64 folder (default: searched under C:\Qt and %USERPROFILE%\Qt)
rem     VCPKG_ROOT      a git clone of vcpkg (default: searched in common places, else cloned)
rem     CUTLINE_PRESET  CMake preset to use (default: release)
setlocal EnableExtensions

set "SOURCE_DIR=%~dp0.."
pushd "%SOURCE_DIR%" || exit /b 1
if not defined CUTLINE_PRESET set "CUTLINE_PRESET=release"

rem The Visual Studio environment replaces VCPKG_ROOT with Visual Studio's own vcpkg, which
rem cannot use the version baseline pinned in vcpkg.json. Remember the caller's value, minus that.
set "REQUESTED_VCPKG=%VCPKG_ROOT%"
if not defined REQUESTED_VCPKG goto :find_msvc
echo "%REQUESTED_VCPKG%" | find /i "Microsoft Visual Studio" >nul && set "REQUESTED_VCPKG="
:find_msvc

rem ------------------------------------------------------------------ C++ compiler (MSVC x64)
if /i "%VSCMD_ARG_TGT_ARCH%"=="x64" goto :have_msvc
if not exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" goto :no_msvc
rem Run vswhere from its folder: a quoted path containing "(x86)" breaks for /f.
pushd "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
set "VS_PATH="
for /f "usebackq delims=" %%I in (`vswhere.exe -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%I"
popd
if not defined VS_PATH goto :no_msvc
echo Using Visual Studio at %VS_PATH%
call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >nul
:have_msvc
where cl >nul 2>&1 || goto :no_msvc
where ninja >nul 2>&1 || goto :no_ninja

rem ------------------------------------------------------------------------------------- vcpkg
set "VCPKG_ROOT="
if defined REQUESTED_VCPKG call :try_vcpkg "%REQUESTED_VCPKG%"
if not defined VCPKG_ROOT call :try_vcpkg "%SOURCE_DIR%\..\vcpkg"
if not defined VCPKG_ROOT call :try_vcpkg "%SOURCE_DIR%\..\..\vcpkg"
if not defined VCPKG_ROOT call :try_vcpkg "%USERPROFILE%\vcpkg"
if not defined VCPKG_ROOT call :try_vcpkg "%USERPROFILE%\Documents\vcpkg"
if not defined VCPKG_ROOT if defined OneDrive call :try_vcpkg "%OneDrive%\Documents\vcpkg"
if not defined VCPKG_ROOT call :try_vcpkg "C:\vcpkg"
if not defined VCPKG_ROOT call :try_vcpkg "C:\dev\vcpkg"
if defined VCPKG_ROOT goto :have_vcpkg
set "VCPKG_ROOT=%USERPROFILE%\vcpkg"
echo vcpkg not found; cloning it into %VCPKG_ROOT%
git clone https://github.com/microsoft/vcpkg "%VCPKG_ROOT%" || goto :step_failed
:have_vcpkg
if not exist "%VCPKG_ROOT%\vcpkg.exe" call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics
if not exist "%VCPKG_ROOT%\vcpkg.exe" goto :step_failed
echo Using vcpkg at %VCPKG_ROOT%

rem ---------------------------------------------------------------------------------------- Qt
if defined QT_DIR goto :check_qt
for /d %%D in ("C:\Qt\6.*" "%USERPROFILE%\Qt\6.*") do if exist "%%~D\msvc2022_64\lib\cmake\Qt6\Qt6Config.cmake" set "QT_DIR=%%~D\msvc2022_64"
rem Prefer 6.12 when several Qt versions are installed (the loop above keeps the last match).
for /d %%D in ("C:\Qt\6.12.*" "%USERPROFILE%\Qt\6.12.*") do if exist "%%~D\msvc2022_64\lib\cmake\Qt6\Qt6Config.cmake" set "QT_DIR=%%~D\msvc2022_64"
:check_qt
if not exist "%QT_DIR%\lib\cmake\Qt6\Qt6Config.cmake" goto :no_qt
echo Using Qt at %QT_DIR%
set "PATH=%QT_DIR%\bin;%PATH%"

rem ------------------------------------------------------------------- configure, build, test
set "BUILD_DIR=build\%CUTLINE_PRESET%"
rem A configure that failed earlier (e.g. without a compiler) leaves a cache but no build.ninja.
if exist "%BUILD_DIR%\CMakeCache.txt" if not exist "%BUILD_DIR%\build.ninja" rmdir /s /q "%BUILD_DIR%"

cmake --preset %CUTLINE_PRESET% "-DCMAKE_PREFIX_PATH=%QT_DIR%" || goto :step_failed
cmake --build --preset %CUTLINE_PRESET% || goto :step_failed
ctest --preset %CUTLINE_PRESET% || goto :step_failed
cmake --install "%BUILD_DIR%" --prefix dist || goto :step_failed

echo.
echo Success. The app is in %CD%\dist\bin\Cutline.exe
if /i not "%~1"=="--no-run" start "" "dist\bin\Cutline.exe"
popd
exit /b 0

rem ------------------------------------------------------------------------------- helpers
:try_vcpkg
rem A usable vcpkg is a git clone (the version baseline needs its history) with the toolchain.
if exist "%~1\scripts\buildsystems\vcpkg.cmake" if exist "%~1\.git" set "VCPKG_ROOT=%~f1"
exit /b 0

:no_msvc
echo.
echo MISSING: the Visual Studio C++ compiler (MSVC, 64-bit).
echo Install it with this command (about 5 GB; accept the admin prompt), then run this script again:
echo.
echo   winget install Microsoft.VisualStudio.2022.BuildTools --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.CMake.Project --includeRecommended"
echo.
echo If Visual Studio is already installed, open the Visual Studio Installer, click Modify and
echo tick "Desktop development with C++".
goto :fail

:no_ninja
echo.
echo MISSING: Ninja. Open the Visual Studio Installer, click Modify, and under
echo "Desktop development with C++" tick "C++ CMake tools for Windows".
goto :fail

:no_qt
echo.
echo MISSING: Qt 6 for MSVC 2022 64-bit (looked under C:\Qt and %USERPROFILE%\Qt).
echo Qt from Python (PySide6, PyQt) does not count: it has no C++ headers or CMake files.
echo Install it without a Qt account using aqtinstall (its development version reads Qt 6.12):
echo.
echo   python -m pip install "aqtinstall @ git+https://github.com/miurahr/aqtinstall.git"
echo   python -m aqt install-qt windows desktop 6.12.0 win64_msvc2022_64 --outputdir C:\Qt
echo.
echo Or add the "MSVC 2022 64-bit" component of Qt 6.12 with the Qt Maintenance Tool, or set
echo QT_DIR to an existing Qt msvc2022_64 folder before running this script.
if exist "C:\Qt" echo. & echo Found under C:\Qt: & dir /b /ad "C:\Qt"
goto :fail

:step_failed
echo.
echo FAILED: the step above did not succeed. Share the output above when asking for help.
goto :fail

:fail
popd
exit /b 1
