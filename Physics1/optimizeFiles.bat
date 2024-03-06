@echo off


cd Graphics 1\assets\textures




set "OutputFolder=CompressedLossy"
set "OutputFolderL=CompressedLossless"
set "OutputFolderO=PVROptimized"
echo %OutputFolder%

:: Lossy Compression
for %%f in (*.png) do (
	if exist Compressed/%%~nxf (
	echo already Compressed Lossy
	) else (
	"../../../CompressionProgs/pngquant.exe" --verbose --quality=45-85 --output %OutputFolder%\%%f "%%f"
	)
)

:: Lossless Compression
for %%f in (*.png) do (
	if exist CompressedL/%%~nxf (
	echo already Compressed Lossless
	) else (
	"../../../CompressionProgs/optipng.exe" %%f -dir %OutputFolderL%
	)
)

:: Optimized Compression (PVR)
for %%f in (*.png) do (
	if exist Optimized/%%~nxf (
	echo already Compressed Lossy
	) else (
	"../../../CompressionProgs/PVRTexToolCLI.exe" -m -f a1r5g5b5 -i %%f -o %OutputFolderO%\%%~nf.pvr
	)
)

pause

