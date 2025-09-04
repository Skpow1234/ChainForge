@echo off
REM ChainForge Build Script for Windows
REM This script builds the project using CMake and Conan

setlocal enabledelayedexpansion

REM Set error handling
set "EXIT_CODE=0"

REM Colors for output (Windows 10+)
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM Function to print colored output
:print_status
echo %BLUE%[INFO]%NC% %~1
goto :eof

:print_success
echo %GREEN%[SUCCESS]%NC% %~1
goto :eof

:print_warning
echo %YELLOW%[WARNING]%NC% %~1
goto :eof

:print_error
echo %RED%[ERROR]%NC% %~1
goto :eof

REM Check if required tools are installed
:check_requirements
call :print_status "Checking build requirements..."

where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    call :print_error "CMake is not installed. Please install CMake 3.20 or later."
    set "EXIT_CODE=1"
    goto :end
)

where conan >nul 2>&1
if %ERRORLEVEL% neq 0 (
    call :print_error "Conan is not installed. Please install Conan 2.0 or later."
    set "EXIT_CODE=1"
    goto :end
)

where ninja >nul 2>&1
if %ERRORLEVEL% neq 0 (
    call :print_warning "Ninja is not installed. Using Visual Studio generator instead."
    set "USE_NINJA=false"
) else (
    set "USE_NINJA=true"
)

call :print_success "Build requirements satisfied"
goto :eof

REM Install Conan dependencies
:install_dependencies
call :print_status "Installing Conan dependencies..."

REM Create build directory
if not exist "build" mkdir build

REM Install dependencies
cd build
conan install .. --output-folder=. --build=missing
if %ERRORLEVEL% neq 0 (
    call :print_error "Failed to install dependencies"
    set "EXIT_CODE=1"
    goto :end
)
cd ..

call :print_success "Dependencies installed successfully"
goto :eof

REM Configure the project
:configure_project
set "build_type=%~1"
if "%build_type%"=="" set "build_type=Release"
set "build_dir=%~2"
if "%build_dir%"=="" set "build_dir=build"

call :print_status "Configuring project (%build_type%) in %build_dir%..."

cd %build_dir%

if "%USE_NINJA%"=="true" (
    cmake .. -DCMAKE_BUILD_TYPE=%build_type% -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G Ninja
) else (
    cmake .. -DCMAKE_BUILD_TYPE=%build_type% -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
)

if %ERRORLEVEL% neq 0 (
    call :print_error "Failed to configure project"
    set "EXIT_CODE=1"
    goto :end
)

cd ..
call :print_success "Project configured successfully"
goto :eof

REM Build the project
:build_project
set "build_dir=%~1"
if "%build_dir%"=="" set "build_dir=build"

call :print_status "Building project in %build_dir%..."

cd %build_dir%

if "%USE_NINJA%"=="true" (
    ninja
) else (
    cmake --build . --config %build_type% --parallel
)

if %ERRORLEVEL% neq 0 (
    call :print_error "Failed to build project"
    set "EXIT_CODE=1"
    goto :end
)

cd ..
call :print_success "Project built successfully"
goto :eof

REM Run tests
:run_tests
set "build_dir=%~1"
if "%build_dir%"=="" set "build_dir=build"

call :print_status "Running tests in %build_dir%..."

cd %build_dir%
ctest --output-on-failure --parallel
if %ERRORLEVEL% neq 0 (
    call :print_warning "Some tests failed"
)
cd ..

call :print_success "Tests completed"
goto :eof

REM Install the project
:install_project
set "build_dir=%~1"
if "%build_dir%"=="" set "build_dir=build"
set "install_prefix=%~2"
if "%install_prefix%"=="" set "install_prefix=C:\Program Files\ChainForge"

call :print_status "Installing project to %install_prefix%..."

cd %build_dir%

if "%USE_NINJA%"=="true" (
    ninja install
) else (
    cmake --install . --prefix "%install_prefix%"
)

if %ERRORLEVEL% neq 0 (
    call :print_error "Failed to install project"
    set "EXIT_CODE=1"
    goto :end
)

cd ..
call :print_success "Project installed successfully"
goto :eof

REM Clean build directory
:clean_build
set "build_dir=%~1"
if "%build_dir%"=="" set "build_dir=build"

call :print_status "Cleaning build directory %build_dir%..."
if exist "%build_dir%" rmdir /s /q "%build_dir%"
call :print_success "Build directory cleaned"
goto :eof

REM Main execution
:main
set "build_type=Release"
set "build_dir=build"
set "install_prefix=C:\Program Files\ChainForge"
set "run_tests_flag=false"
set "install_flag=false"
set "clean_flag=false"

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :start_build
if "%~1"=="--debug" (
    set "build_type=Debug"
    shift
    goto :parse_args
)
if "%~1"=="--release" (
    set "build_type=Release"
    shift
    goto :parse_args
)
if "%~1"=="--build-dir" (
    set "build_dir=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="--install-prefix" (
    set "install_prefix=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="--test" (
    set "run_tests_flag=true"
    shift
    goto :parse_args
)
if "%~1"=="--install" (
    set "install_flag=true"
    shift
    goto :parse_args
)
if "%~1"=="--clean" (
    set "clean_flag=true"
    shift
    goto :parse_args
)
if "%~1"=="--help" (
    echo Usage: %~nx0 [OPTIONS]
    echo Options:
    echo   --debug           Build in Debug mode
    echo   --release         Build in Release mode ^(default^)
    echo   --build-dir DIR   Specify build directory ^(default: build^)
    echo   --install-prefix  Specify install prefix ^(default: C:\Program Files\ChainForge^)
    echo   --test            Run tests after building
    echo   --install         Install after building
    echo   --clean           Clean build directory before building
    echo   --help            Show this help message
    exit /b 0
)
shift
goto :parse_args

:start_build
call :print_status "Starting ChainForge build process..."
call :print_status "Build type: %build_type%"
call :print_status "Build directory: %build_dir%"

REM Check requirements
call :check_requirements
if %EXIT_CODE% neq 0 goto :end

REM Clean if requested
if "%clean_flag%"=="true" (
    call :clean_build "%build_dir%"
)

REM Install dependencies
call :install_dependencies
if %EXIT_CODE% neq 0 goto :end

REM Configure project
call :configure_project "%build_type%" "%build_dir%"
if %EXIT_CODE% neq 0 goto :end

REM Build project
call :build_project "%build_dir%"
if %EXIT_CODE% neq 0 goto :end

REM Run tests if requested
if "%run_tests_flag%"=="true" (
    call :run_tests "%build_dir%"
)

REM Install if requested
if "%install_flag%"=="true" (
    call :install_project "%build_dir%" "%install_prefix%"
    if %EXIT_CODE% neq 0 goto :end
)

call :print_success "Build process completed successfully!"
goto :end

:end
exit /b %EXIT_CODE%
