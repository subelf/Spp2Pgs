@ECHO OFF

SETLOCAL
SET "FILE_DIR=%~dp0"
PUSHD "%FILE_DIR%"

SET SLNMAKE_EXIT=-1

IF "%SLNMAKE_TARGETNAME%"=="" GOTO :ShowHelp
IF "%SLNMAKE_TARGETEXT%"=="" GOTO :ShowHelp
IF "%SLNMAKE_OUTDIR%"=="" GOTO :ShowHelp
IF "%SLNMAKE_SLNOUTDIR%"=="" GOTO :ShowHelp
IF "%SLNMAKE_SLN%"=="" GOTO :ShowHelp

IF "%SLNMAKE_SLNTARGETNAME%"=="" SET "SLNMAKE_SLNTARGETNAME=%SLNMAKE_TARGETNAME%"

SET ARG=/%*
SET ARG=%ARG:/=%
SET ARG=%ARG:-=%
SET BUILDTYPE=Build

IF /I "%ARG%" == "?"   GOTO :ShowHelp

FOR %%G IN (%ARG%) DO (
  IF /I "%%G" == "Build"    SET "BUILDTYPE=Build"
  IF /I "%%G" == "Clean"    SET "BUILDTYPE=Clean"
  IF /I "%%G" == "Rebuild"  SET "BUILDTYPE=Rebuild"
)

SET SLNMAKE_EXIT=0

set BUILDCMD=MSBuild.exe "%SLNMAKE_SLN%" /nologo /consoleloggerparameters:Verbosity=minimal /nodeReuse:true /m /t:%BUILDTYPE% /property:Configuration=%SLNMAKE_CFG%;Platform=%SLNMAKE_ARCH%
%BUILDCMD%

IF %ERRORLEVEL% NEQ 0 (
  SET SLNMAKE_EXIT=%ERRORLEVEL%
  ECHO %BUILDCMD% failed.
)

IF /I NOT "%BUILDTYPE%" == "Build" CALL :PostClean
IF /I NOT "%BUILDTYPE%" == "Clean" (
  CALL :PostBuild
  IF NOT EXIST "%SLNMAKE_OUTDIR%%SLNMAKE_TARGETNAME%%SLNMAKE_TARGETEXT%" SET SLNMAKE_EXIT=-1
)

GOTO :ScriptEnd

:ShowHelp
TITLE %~nx0 Help
ECHO.
ECHO Usage:
ECHO %~nx0 [Clean^|Build^|Rebuild]
ECHO.
ECHO Notes: SLNMAKE_* environment variables should 
ECHO        be prepared before calling this script.
ECHO. & ECHO.
ECHO.

:ScriptEnd

POPD
ENDLOCAL

EXIT /B %SLNMAKE_EXIT%

REM ===============================

:PostClean
PUSHD %SLNMAKE_OUTDIR%
DEL /F /Q "%SLNMAKE_TARGETNAME%.*"
POPD

GOTO :EOF

:PostBuild
FOR %%I in (%SLNMAKE_SLNOUTDIR%%SLNMAKE_SLNTARGETNAME%*) DO (
	ECHO F | XCOPY /D /V /Y "%%I" "%SLNMAKE_OUTDIR%%SLNMAKE_TARGETNAME%%%~xI" >NUL 2>NUL
)

GOTO :EOF
