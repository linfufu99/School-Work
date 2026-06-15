@echo off
chcp 65001 >nul
echo ========================================
echo   银行账户管理系统 - 自动编译运行
echo ========================================
echo.

REM 检查是否在正确的目录
if not exist CMakeLists.txt (
    echo [错误] 请在项目根目录运行此脚本
    pause
    exit /b 1
)

echo [步骤1] 正在查找Qt安装路径...
echo.

REM 常见的Qt安装路径
set QT_PATHS=C:\Qt D:\Qt E:\Qt C:\Program Files\Qt D:\Program Files\Qt

set QT_FOUND=0
for %%P in (%QT_PATHS%) do (
    if exist "%%P" (
        echo 检查: %%P
        for /d %%V in ("%%P\6.*" "%%P\5.*") do (
            if exist "%%V\mingw*\bin\qmake.exe" (
                for /d %%M in ("%%V\mingw*") do (
                    set "QT_DIR=%%M"
                    set QT_FOUND=1
                    goto :found_qt
                )
            )
        )
        REM 检查Tools目录下的MinGW
        if exist "%%P\Tools" (
            for /d %%T in ("%%P\Tools\mingw*") do (
                if exist "%%T\bin\g++.exe" (
                    set "MINGW_DIR=%%T"
                )
            )
        )
    )
)

:found_qt
if %QT_FOUND%==0 (
    echo.
    echo [错误] 未找到Qt安装！
    echo.
    echo 请手动设置Qt路径：
    echo 1. 找到你的Qt安装目录（例如 C:\Qt\6.10.1\mingw_64）
    echo 2. 编辑此脚本，在开头添加：
    echo    set "QT_DIR=你的Qt路径"
    echo    set "MINGW_DIR=你的MinGW路径"
    echo.
    pause
    exit /b 1
)

echo.
echo [找到Qt] %QT_DIR%
if defined MINGW_DIR echo [找到MinGW] %MINGW_DIR%

REM 设置环境变量
set "PATH=%QT_DIR%\bin;%PATH%"
if defined MINGW_DIR set "PATH=%MINGW_DIR%\bin;%PATH%"

REM 查找CMake
set CMAKE_FOUND=0
where cmake >nul 2>&1
if %errorlevel%==0 (
    set CMAKE_FOUND=1
    echo [找到CMake] 
    cmake --version | findstr /C:"version"
) else (
    REM 尝试在Qt目录下查找CMake
    for %%P in (%QT_PATHS%) do (
        if exist "%%P\Tools\CMake*\bin\cmake.exe" (
            for /d %%C in ("%%P\Tools\CMake*") do (
                set "PATH=%%C\bin;%PATH%"
                set CMAKE_FOUND=1
                echo [找到CMake] %%C
                goto :found_cmake
            )
        )
    )
)

:found_cmake
if %CMAKE_FOUND%==0 (
    echo.
    echo [错误] 未找到CMake！
    echo 请安装CMake或确保Qt安装包含CMake工具
    pause
    exit /b 1
)

echo.
echo ========================================
echo [步骤2] 开始编译...
echo ========================================
echo.

REM 清理并创建构建目录
if exist build rmdir /s /q build
mkdir build
cd build

echo 配置项目...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_DIR%"

if errorlevel 1 (
    echo.
    echo [错误] CMake配置失败！
    cd ..
    pause
    exit /b 1
)

echo.
echo 编译项目...
cmake --build . --config Release -j4

if errorlevel 1 (
    echo.
    echo [错误] 编译失败！
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo [步骤3] 启动程序...
echo ========================================
echo.

REM 复制Qt DLL到可执行文件目录
if exist appyhyhglxt.exe (
    echo 程序位置: %CD%\appyhyhglxt.exe
    start appyhyhglxt.exe
) else if exist Release\appyhyhglxt.exe (
    echo 程序位置: %CD%\Release\appyhyhglxt.exe
    start Release\appyhyhglxt.exe
) else (
    echo [错误] 找不到可执行文件
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo 编译完成！程序已启动
echo ========================================
echo.
echo 数据库文件位置: %CD%\build\bank_system.db
echo.
echo 按任意键关闭此窗口...
pause >nul
