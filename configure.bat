@echo off
set CMAKE="D:\Program Files\JetBrains\CLion 2025.2.3\bin\cmake\win\x64\bin\cmake.exe"
set NINJA="D:\Program Files\JetBrains\CLion 2025.2.3\bin\ninja\win\x64\ninja.exe"

%CMAKE% -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=%NINJA% -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -G Ninja -S . -B build
