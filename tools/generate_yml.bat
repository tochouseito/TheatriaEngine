@echo on
setlocal
chcp 65001 >nul

rem ===== 設定 =====
set "WORKSPACE=Theatria"
set "STARTUP=ChoEditor"   rem ← 未設定なら空に: set "STARTUP="
rem =================

rem この bat は tools\ にある想定
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%\.."  1>nul 2>nul
set "REPO_ROOT=%CD%"
popd 1>nul 2>nul

set "SLN=%REPO_ROOT%\project\Theatria.sln"
set "TOOL=%REPO_ROOT%\tools\theatria_buildgen.py"

rem ★ yml は bat と同じディレクトリ（tools）に出力
set "YAML_OUT=%SCRIPT_DIR%\theatria.build.yml"

rem （.editorconfig は従来どおりルート。ここも tools にしたいなら下を %SCRIPT_DIR% に変更）
set "EDITORCONFIG_OUT=%REPO_ROOT%\.editorconfig"

echo [INFO] REPO_ROOT   = %REPO_ROOT%
echo [INFO] SLN         = %SLN%
echo [INFO] TOOL        = %TOOL%
echo [INFO] YAML_OUT    = %YAML_OUT%
echo [INFO] EDITORCONFIG= %EDITORCONFIG_OUT%

if not exist "%SLN%" (
  echo [ERROR] Not found: "%SLN%"
  goto :fail
)
if not exist "%TOOL%" (
  echo [ERROR] Not found: "%TOOL%"
  goto :fail
)

rem ---- Python 検出 ----
set "PYTHON_EXE="
where py      && set "PYTHON_EXE=py"
if not defined PYTHON_EXE where python && set "PYTHON_EXE=python"
if not defined PYTHON_EXE (
  echo [ERROR] Python not found. Install: winget install --id Python.Python.3.12 -e
  goto :fail
)
%PYTHON_EXE% -V

rem ---- PyYAML 確認（必要なら導入）----
%PYTHON_EXE% -c "import yaml" || (
  echo [INFO] Installing PyYAML...
  %PYTHON_EXE% -m pip install --upgrade pip pyyaml || goto :fail
)

rem ---- 実行 ----
if defined STARTUP (
  "%PYTHON_EXE%" "%TOOL%" --sln "%SLN%" -o "%YAML_OUT%" --editorconfig "%EDITORCONFIG_OUT%" --workspace "%WORKSPACE%" --startup "%STARTUP%"
) else (
  "%PYTHON_EXE%" "%TOOL%" --sln "%SLN%" -o "%YAML_OUT%" --editorconfig "%EDITORCONFIG_OUT%" --workspace "%WORKSPACE%"
)
echo [INFO] theatria_buildgen exit code: %ERRORLEVEL%

if not exist "%YAML_OUT%" (
  echo [ERROR] YAML not generated: "%YAML_OUT%"
  goto :fail
)

echo [OK] Generated:
echo   %YAML_OUT%
if exist "%EDITORCONFIG_OUT%" echo   %EDITORCONFIG_OUT%
echo.
echo 次:  CMake/Premake 生成（例）
echo   python tools\gen_build.py tools\theatria.build.yml
goto :end

:fail
echo.
echo *** 失敗しました。上のログを貼ってください。***
echo.
pause
exit /b 1

:end
echo.
pause
exit /b 0
