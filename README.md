![X project logo. Two letters D and C, extruded slightly and placed against a quad within the 3D application Blender.](https://github.com/DavidCradock/DC/blob/51c30954811147e2ef6a3bb882f0f1131df434be/github_images/github_social_image.png)
# My current personal project.
## Written in C++ for Windows 11, using OpenGL/Vulkan for rendering.
### This project is intended to become a bunch of code which I can use to develop a game with.
#### Not only a game, but also applications too. I wish to implement an image editor to replace my aging Adobe Photoshop as it's user interface does not scale on high DPI displays making the interface incredibly difficult to use and I find the way which open source alternatives deal with the alpha channel awkward. I also refuse to pay a monthly fee for software to obtain access to the latest version of Photoshop.
** Build help **
- Download the source as a zip and extract somewhere.
- Download and install Visual studio
- Download and install the Vulkan SDK
- Load the X.sln file into Visual studio
- Hit compile, done.

# TODO
Implement renderer
Remove GLFW
Check String class
Add 2D and UI stuff from X project codebase.

# Notes...

# Changes to coding style

Do not use _m for members as it's bloody obvious that the member is a member.

Do not use pre-pended type (for example fSomeFloat) as the IDE shows the type of a member/variable)

Do not use _ underscore for private members as the IDE shows that a member is private.

Use a descriptive, non abbreviated name for a variable/member. Abbreviation makes things less clear and autocomplete makes adding names easy with less typing.

Do not add "C" to beginning of class declarations, it's a class, it's obvious.

Parameters are all lowercase for their first character and have PARAM at the end in the definition only and only if needed.

Camel style casing, someFileWhichDoesSomething. It's easy to read and quite sparse.

# Source code filenaming.

Prepend "DC" for the library's main include header (and possible .cpp), but for the rest of the files, do not add the "DC"

# Error handling

Do not use exceptions, there's some overhead and using macros for critical errors is fine.

# Singleton pattern.

Do not use unless appropriate.

Singletons are great for allowing quick access to a "global" object, but as their name implies, prevents creation of multiple instances.

Their initialisation order can be really messy.

Use globals instead, or pass pointers/references to objects.

# Strings.

Use wide strings as it's the default setting, they are recommended for many positive reasons (IE some user with a weird character is now supported)
and it's easier to work with than multibyte.

Sure, the L"bloody L!" is kinda annoying having to add Ls everywhere, but calm down, it's only a single character to add. Relax! :D

# Vulkan notes.
Vulkan main objects and their use.

VkInstance : The Vulkan context, used to access drivers.

VkPhysicalDevice : A GPU. Used to query physical GPU details, like features, capabilities, memory size, etc.

VkDevice : The “logical” GPU context that you actually execute things on.

VkBuffer : A chunk of GPU visible memory.

VkImage : A texture you can write to and read from.

VkPipeline : Holds the state of the gpu needed to draw. For example: shaders, rasterization options, depth settings.

VkRenderPass : Holds information about the images you are rendering into. All drawing commands have to be done inside a render pass.

VkFrameBuffer : Holds the target images for a render pass.

VkCommandBuffer : Encodes GPU commands. All execution that is performed on the GPU itself (not in the driver) has to be encoded in a VkCommandBuffer.

VkQueue : Execution “port” for commands. GPUs will have a set of queues with different properties. Some allow only graphics commands, others only allow memory commands, etc. Command buffers are executed by submitting them into a queue, which will copy the rendering commands onto the GPU for execution.

VkDescriptorSet : Holds the binding information that connects shader inputs to data such as VkBuffer resources and VkImage textures. Think of it as a set of gpu-side pointers that you bind once.

VkSwapchainKHR : Holds the images for the screen. It allows you to render things into a visible window. The KHR suffix shows that it comes from an extension, which in this case is VK_KHR_swapchain

VkSemaphore : Synchronizes GPU to GPU execution of commands. Used for syncing multiple command buffer submissions one after other.

VkFence : Synchronizes GPU to CPU execution of commands. Used to know if a command buffer has finished being executed on the GPU.

Engine initialization

First, everything is initialized. To initialize Vulkan, you start by creating a VkInstance. From the VkInstance, you query the list of VkPhysicalDevice handles available in the machine.
For example, if the computer had both a dedicated GPU and integrated graphics, there would be a VkPhysicalDevice for each. After querying the limits and features of the available
VkPhysicalDevice handles, you create a VkDevice from it. With a VkDevice, you then get VkQueue handles from it, allowing you to execute commands. Then you initialize the VkSwapchainKHR.
Alongside the VkQueue handles, you create VkCommandPool objects that enable you to allocate command buffers from them.


Asset initialization

Once the core structures are initialized, you initialize the resources you need for whatever you will be rendering. The materials are loaded, and you create a set of VkPipeline objects
for the shader combinations and parameters needed to render the materials. For the meshes, you upload their vertex data into VkBuffer resources, and you upload their textures into
VkImage resources, making sure that the images are in “readable” layout. You also create VkRenderPass objects for whatever your main rendering passes you have. For example, there may
be a VkRenderPass for the main rendering, and another for a shadow pass. On a real engine, much of this can be parallelized and done in background threads, especially since pipeline
creation can be quite expensive.


Render Loop

Now that everything is ready for rendering, you first ask the VkSwapchainKHR for an image to render to. Then you allocate a VkCommandBuffer from a VkCommandBufferPool or reuse an
already allocated command buffer that has finished execution, and “start” the command buffer, which allows you to write commands into it. Next, you begin rendering by starting a
VkRenderPass, which specifies that you are rendering to the image requested from swapchain, then create a loop where you bind a VkPipeline, bind some VkDescriptorSet resources
(for the shader parameters), bind the vertex buffers, and then execute a draw call. Once you are finished with the drawing for a pass, you end the VkRenderPass. If there is nothing
more to render, you end the VkCommandBuffer. Finally, you submit the command buffer into the queue for rendering. This will begin execution of the commands in the command buffer on
the gpu. If you want to display the result of the rendering, you “present” the image you have rendered to to the screen. Because the execution may not have finished yet, you use a
semaphore to make the presentation of the image to the screen wait until rendering is finished.
