@echo off
echo Temp shader compilation batch file
@echo on
C:/VulkanSDK/1.3.261.1/Bin/glslc.exe default.vert -o default.vert.spv
C:/VulkanSDK/1.3.261.1/Bin/glslc.exe default.frag -o default.frag.spv
@echo off
pause
