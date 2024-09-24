@echo off
REM Define variables
SET EXTENSION_NAME=bezier
SET SOURCE_FILE=bezier.c
SET SETUP_FILE=setup.py

REM Check if Python is installed
python --version
IF %ERRORLEVEL% NEQ 0 (
    echo Python is not installed. Please install Python and ensure it's added to your PATH.
    exit /b 1
)

REM Check if setuptools is installed
echo Checking for setuptools...
python -c "import setuptools"
IF %ERRORLEVEL% NEQ 0 (
    echo setuptools is not installed. Attempting to install setuptools...
    pip install setuptools
    IF %ERRORLEVEL% NEQ 0 (
        echo Failed to install setuptools. Please install it manually.
        exit /b 1
    )
)

REM Build the bezier C extension
echo Building bezier C extension...
python setup.py build
IF %ERRORLEVEL% NEQ 0 (
    echo Failed to build the C extension.
    exit /b 1
)
echo Build was successful!

REM Install the bezier extension
echo Installing the C extension...
python setup.py install
IF %ERRORLEVEL% NEQ 0 (
    echo Failed to install the C extension.
    exit /b 1
)
echo Installation was successful!

pause

@echo on
