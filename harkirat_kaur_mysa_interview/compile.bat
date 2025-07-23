@echo off
REM --------------------------------------------------
REM compile.bat – Installs/updates MSYS2 & GCC then compiles MysaIrrigationSystem
REM --------------------------------------------------

REM 1) Check for g++
where g++ >nul 2>nul
if %ERRORLEVEL%==0 goto precompile

REM 2) g++ missing – ensure MSYS2 present
if exist "C:\msys64\usr\bin\bash.exe" (
    echo MSYS2 found; skipping installation.
) else (
    echo MSYS2 not found. Attempting installation via winget...
    winget install -e --id MSYS2.MSYS2 --accept-package-agreements --accept-source-agreements
    if %ERRORLEVEL% neq 0 (
        echo WARNING: winget install failed. If you have MSYS2 installed manually, this is OK.
    )
    if not exist "C:\msys64\usr\bin\bash.exe" (
        echo.
        echo ERROR: MSYS2 still not detected.
        echo Please install it manually from https://www.msys2.org and re-run this script.
        pause
        exit /b 1
    )
)

REM 3) Update & install GCC in MSYS2
echo Installing / updating GCC toolchain in MSYS2...
"C:\msys64\usr\bin\bash.exe" -lc "pacman -Syu --noconfirm"
"C:\msys64\usr\bin\bash.exe" -lc "pacman -Sy --noconfirm mingw-w64-x86_64-gcc"

REM 4) Add MinGW‑w64 bin to PATH for this session
set "PATH=%PATH%;C:\msys64\mingw64\bin"

REM 5) Persist PATH to your user environment
setx PATH "%PATH%" >nul

echo.
echo g++ should now be available. You can re-open this terminal if needed.
echo.
goto precompile

:precompile
REM Try to delete mysa_irrigation.exe if it exists (to avoid permission denied)
if exist mysa_irrigation.exe (
    del /f mysa_irrigation.exe
    if exist mysa_irrigation.exe (
        echo.
        echo WARNING: Could not delete mysa_irrigation.exe. Please close any running instance and try again.
        pause
        exit /b 1
    )
)

goto compile

:compile
echo Compiling Mysa Irrigation System...
g++ -std=c++11 ^
    -IMysaIrrigationSystem/include ^
    MysaIrrigationSystem/main.cpp ^
    MysaIrrigationSystem/src/GardenZone.cpp ^
    MysaIrrigationSystem/src/IrrigationController.cpp ^
    MysaIrrigationSystem/src/Logger.cpp ^
    MysaIrrigationSystem/src/Plant.cpp ^
    MysaIrrigationSystem/src/Soil.cpp ^
    MysaIrrigationSystem/src/WaterPump.cpp ^
    MysaIrrigationSystem/src/WeatherSensor.cpp ^
    -o mysa_irrigation.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful!
    echo Running program...
    mysa_irrigation.exe
) else (
    echo.
    echo Compilation failed!
)

pause
exit /b 