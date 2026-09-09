:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
@echo off
call %~dp0../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set "projectdir=%~dp0"
set its_blob_log="%projectdir%its_blob_perso.log"
set current_log_file=%its_blob_log%
echo. > %current_log_file%

:start
goto exe:
goto py:
:exe
:: called if we want to use ITSbuilder executable
set "itsbuilder=%cube_fw_path%\Utilities\PC_Software\ITSbuilder\dist\ITSbuilder\ITSbuilder.exe"
set "python="
if exist %itsbuilder% (
echo run ITSbuilder with Windows executable
goto update
)
:py
:: called if we just want to use ITSbuilder python (think to comment "goto exe:")
set "itsbuilder=%cube_fw_path%\Utilities\PC_Software\ITSbuilder\ITSbuilder.py"
set "python=python "
echo run ITSbuilder with python

:update
set "ItsBuilder=%python%%itsbuilder%"

:: ITS blob file
set blob=%projectdir%Binary\ITS_Factory_Blob.bin

:: Owner of the data (NonSecure: 0xffffffff, Secure module: ASID of the module)
set owner=0xffffffff

:: ============================================= ITS blob preparation (example) ===================================================
:: ID of the key  (0x23 is the data ID used by application)
set id_key=0x23

:: Create empty blob
%ItsBuilder% createblob -v 1 %blob% 2>> %current_log_file%
if !errorlevel! neq 0 goto :error

:: Add GCM key to blob
%ItsBuilder% addkey2blob %blob% %blob% -i %id_key% -o 0xffffffff --keytype=AES -b 256 -a CBC -k Keys\AES_256bits.bin -u ENCRYPT --format=RAW 2>> %current_log_file%
if !errorlevel! neq 0 goto :error

:: Parse blob
%ItsBuilder% infoblob %blob% - >> %current_log_file%
if !errorlevel! neq 0 goto :error

:: ============================================================= End functions ===================================================
:: All the steps were executed correctly
echo =====
echo ===== ITS blob generation successful
echo =====
if [%1] neq [AUTO] cmd /k
exit /b 0

:: Error
:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
if [%1] neq [AUTO] cmd /k
exit /b 1
