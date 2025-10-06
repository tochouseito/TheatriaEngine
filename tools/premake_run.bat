@echo off
setlocal

rem === 設定（必要なら上書き） ===
set "GEN=vs2022"   rem 既定のジェネレータ。引数で上書き可: premake_run.bat vs2019
if not "%~1"=="" set "GEN=%~1"
rem ==============================

set "SCRIPT_DIR=%~dp0"
set "PREMAKE_LUA=%SCRIPT_DIR%premake5.lua"

echo [INFO] premake lua : %PREMAKE_LUA%
echo [INFO] generator   : %GEN%

if not exist "%PREMAKE_LUA%" (
  echo [ERROR] premake5.lua が見つかりません（tools に置いてください）
  exit /b 1
)

rem --- premake5 の検出（tools に置いた exe を優先） ---
set "PREMAKE_EXE=premake5"
if exist "%SCRIPT_DIR%premake5.exe" set "PREMAKE_EXE=%SCRIPT_DIR%premake5.exe"

where "%PREMAKE_EXE%" >nul 2>nul
if errorlevel 1 (
  echo [ERROR] premake5 が見つかりません。PATH を通すか tools に premake5.exe を置いてください。
  echo         例: winget install premake
  exit /b 1
)

pushd "%SCRIPT_DIR%"
"%PREMAKE_EXE%" %GEN%
set "RET=%ERRORLEVEL%"
popd

if not "%RET%"=="0" (
  echo [ERROR] premake 失敗（exit code=%RET%）
  exit /b %RET%
)

echo [OK] premake 生成完了
echo     出力先は premake5.lua の location 設定に従います（例: tools\build_vs\）
exit /b 0
