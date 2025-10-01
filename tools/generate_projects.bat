@echo on
setlocal
rem chcp 65001 >nul  && rem 日本語ログをUTF-8で見たい場合

rem --- このbatは tools\ に置く想定 ---
set "SCRIPT_DIR=%~dp0"
set "YAML=%SCRIPT_DIR%\theatria.build.yml"
set "GEN=%SCRIPT_DIR%\gen_build.py"

echo [INFO] YAML = %YAML%
echo [INFO] GEN  = %GEN%

if not exist "%YAML%"  ( echo [ERROR] YAML not found & exit /b 1 )
if not exist "%GEN%"   ( echo [ERROR] gen_build.py not found & exit /b 1 )

rem --- Python 検出 ---
set "PYTHON_EXE="
where py      >nul 2>nul && set "PYTHON_EXE=py"
if not defined PYTHON_EXE where python >nul 2>nul && set "PYTHON_EXE=python"
if not defined PYTHON_EXE ( echo [ERROR] Python not found & exit /b 1 )
%PYTHON_EXE% -V

rem --- PyYAML 確認 ---
%PYTHON_EXE% -c "import yaml" || (
  echo [INFO] Installing PyYAML...
  %PYTHON_EXE% -m pip install --upgrade pip pyyaml || exit /b 1
)

rem === ここが重要：tools をカレントにして実行 ===
pushd "%SCRIPT_DIR%"
"%PYTHON_EXE%" "%GEN%" "%YAML%" || ( popd & exit /b 1 )
popd

if not exist "%SCRIPT_DIR%\CMakeLists.txt" ( echo [ERROR] CMakeLists.txt not generated in tools & exit /b 1 )
if not exist "%SCRIPT_DIR%\premake5.lua"   ( echo [ERROR] premake5.lua not generated in tools   & exit /b 1 )

echo [OK] Generated:
echo   %SCRIPT_DIR%\CMakeLists.txt
echo   %SCRIPT_DIR%\premake5.lua
exit /b 0
