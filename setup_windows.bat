@echo off
echo 3D Model Viewer

if not exist "CMakeLists.txt" (
    echo ERROR: CMakeLists.txt not found
    echo Please run this script from the model-viewer root directory.
    pause
    exit /b 1
)

if not exist "dependencies" (
    echo ERROR: dependencies folder not found
    echo Please ensure the dependencies folder is present with all required libraries.
    pause
    exit /b 1
)

where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake from: https://cmake.org/download/
    echo Make sure to add it to your system PATH during installation.
    pause
    exit /b 1
)

echo CMake found
echo Dependencies folder present

REM Create build directory
echo.
echo [1/3] Creating build directory...
if not exist "build" mkdir build
cd build

REM Configure project
echo.
echo [2/3] Configuring project...
cmake .. -A x64
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    echo Check that all dependencies are present in the dependencies folder.
    pause
    exit /b 1
)

echo Project configured successfully

REM Build project  
echo.
echo [3/3] Building project...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    echo Check the error messages above for details.
    pause
    exit /b 1
)

echo Build completed successfully
echo Location: %cd%\Release\ModelViewer.exe

REM Open the output directory
explorer Release

echo Press any key to exit...
pause >nul