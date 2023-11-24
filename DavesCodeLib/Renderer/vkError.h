#pragma once
#include "../Common/error.h"

namespace DC
{
#ifndef vkError
	// Checks return code from Vulkan and if not 0, creates an error.
#define vkError(x)\
		{\
		if(x){\
		if (VK_NOT_READY == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_NOT_READY")\
		else if (VK_TIMEOUT == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_TIMEOUT")\
		else if (VK_EVENT_SET == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_EVENT_SET")\
		else if (VK_EVENT_RESET == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_EVENT_RESET")\
		else if (VK_INCOMPLETE == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_INCOMPLETE")\
		else if (VK_ERROR_OUT_OF_HOST_MEMORY == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_OUT_OF_HOST_MEMORY")\
		else if (VK_ERROR_OUT_OF_DEVICE_MEMORY == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_OUT_OF_DEVICE_MEMORY")\
		else if (VK_ERROR_INITIALIZATION_FAILED == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_INITIALIZATION_FAILED")\
		else if (VK_ERROR_DEVICE_LOST == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_DEVICE_LOST")\
		else if (VK_ERROR_MEMORY_MAP_FAILED == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_MEMORY_MAP_FAILED")\
		else if (VK_ERROR_LAYER_NOT_PRESENT == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_LAYER_NOT_PRESENT")\
		else if (VK_ERROR_EXTENSION_NOT_PRESENT == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_EXTENSION_NOT_PRESENT")\
		else if (VK_ERROR_FEATURE_NOT_PRESENT == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_FEATURE_NOT_PRESENT")\
		else if (VK_ERROR_INCOMPATIBLE_DRIVER == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_INCOMPATIBLE_DRIVER")\
		else if (VK_ERROR_TOO_MANY_OBJECTS == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_TOO_MANY_OBJECTS")\
		else if (VK_ERROR_FORMAT_NOT_SUPPORTED == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_FORMAT_NOT_SUPPORTED")\
		else if (VK_ERROR_FRAGMENTED_POOL == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_FRAGMENTED_POOL")\
		else if (VK_ERROR_UNKNOWN == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_UNKNOWN")\
		else if (VK_ERROR_OUT_OF_POOL_MEMORY == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_OUT_OF_POOL_MEMORY")\
		else if (VK_ERROR_INVALID_EXTERNAL_HANDLE == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_INVALID_EXTERNAL_HANDLE")\
		else if (VK_ERROR_FRAGMENTATION == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_FRAGMENTATION")\
		else if (VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS")\
		else if (VK_PIPELINE_COMPILE_REQUIRED == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_PIPELINE_COMPILE_REQUIRED")\
		else if (VK_ERROR_SURFACE_LOST_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_SURFACE_LOST_KHR")\
		else if (VK_ERROR_NATIVE_WINDOW_IN_USE_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_NATIVE_WINDOW_IN_USE_KHR")\
		else if (VK_SUBOPTIMAL_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_SUBOPTIMAL_KHR")\
		else if (VK_ERROR_OUT_OF_DATE_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_OUT_OF_DATE_KHR")\
		else if (VK_ERROR_INCOMPATIBLE_DISPLAY_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_INCOMPATIBLE_DISPLAY_KHR")\
		else if (VK_ERROR_VALIDATION_FAILED_EXT == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_VALIDATION_FAILED_EXT")\
		else if (VK_ERROR_INVALID_SHADER_NV == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_INVALID_SHADER_NV")\
		else if (VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR")\
		else if (VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR")\
		else if (VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR")\
		else if (VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR")\
		else if (VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR")\
		else if (VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR")\
		else if (VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT")\
		else if (VK_ERROR_NOT_PERMITTED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_NOT_PERMITTED_KHR")\
		else if (VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT")\
		else if (VK_THREAD_IDLE_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_THREAD_IDLE_KHR")\
		else if (VK_THREAD_DONE_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_THREAD_DONE_KHR")\
		else if (VK_OPERATION_DEFERRED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_OPERATION_DEFERRED_KHR")\
		else if (VK_OPERATION_NOT_DEFERRED_KHR == x)\
			ErrorIfTrue(1, L"Vulkan error: VK_OPERATION_NOT_DEFERRED_KHR")\
		else\
			ErrorIfTrue(1, L"Vulkan error UNKNOWN")\
		}\
		}
#endif
}