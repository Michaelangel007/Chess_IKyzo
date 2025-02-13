@REM Compile MSVC
@REM vcvars64.bat can be found in 
@REM "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist bin mkdir bin
cl.exe source\chess.c /Fe:bin/chess.exe
