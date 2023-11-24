@echo off
echo Temp shader compilation batch file
@echo on
C:/VulkanSDK/1.3.261.1/Bin/glslc.exe old_default.vert -o old_default.vert.spv
C:/VulkanSDK/1.3.261.1/Bin/glslc.exe old_default.frag -o old_default.frag.spv
C:/VulkanSDK/1.3.261.1/Bin/glslc.exe old_coloured_triangle.vert -o old_coloured_triangle.vert.spv
C:/VulkanSDK/1.3.261.1/Bin/glslc.exe old_coloured_triangle.frag -o old_coloured_triangle.frag.spv
@echo off
pause
