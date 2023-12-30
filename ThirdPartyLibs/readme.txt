In this "ThirdParty" directory, we can see the following folders containing various third party libraries.
Each have been downloaded from various sources on the internet, mainly github and have had Visual studio project files created for them
which compile each into a static library (If they're not a header only codebase) which are then linked or included into the DavesCodeLib static library.

DearImGUI
	Download: https://github.com/ocornut/imgui
	Build type: Static library.
	Description: Dear ImGui is a bloat-free graphical user interface library for C++. It outputs optimized vertex buffers that you can render anytime in your 3D-pipeline-enabled application.
	It is fast, portable, renderer agnostic, and self-contained (no external dependencies).

glm
	Download: https://github.com/g-truc/glm
	Build type: Header include, no static library.
	Description: OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.

SDL2-devel-????-VC
	Download: https://github.com/libsdl-org/SDL/releases/tag/release-2.28.5
	Build type: Header include, static library precompiled.
	Description: Simple DirectMedia Layer is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via Direct3D, OpenGL and Vulkan.
				This is the unzipped .zip file of the SDL library downloaded from the development version of the SDK which already contains pre-compiled files for Visual Studio.

stb
	Download: https://github.com/nothings/stb
	Build type: Header include, no static library.
	Description: Used by the "Image" section of code within DavesCodeLib to enable loading of JPGs/PNGs etc.

TinyObjLoader
	Download: https://github.com/tinyobjloader/tinyobjloader
	Build type: Static library.
	Description: Tiny but powerful single file wavefront obj loader written in C++03. No dependency except for C++ STL. It can parse over 10M polygons with moderate memory and time.

vk-bootstrap
	Download: https://github.com/charles-lunarg/vk-bootstrap
	Build type: Static library.
	Description: A utility library that jump starts initialization of Vulkan.
				 This library simplifies the tedious process of:
				    Instance creation
					Physical Device selection
					Device creation
					Getting queues
					Swapchain creation
				 It also adds several conveniences for:
				    Enabling validation layers
					Adding a debug callback messenger
					Enabling extensions on a physical device
					Select a gpu based on a set of criteria like features, extensions, memory, etc

VulkanMemoryAllocator
	Download: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
	Build type: Header include, no static library.
	Description: Easy to integrate Vulkan memory allocation library.

VulkanSDK
	Download: https://vulkan.lunarg.com/sdk/home#windows
	Description: The Vulkan graphics API SDK.
	This folder is a copy of the VulkanSDK installation directory after it has been installed with the SDK installer.
