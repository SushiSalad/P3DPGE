#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "olcPixelGameEngine.h"
#include "../components/Model.h"

#include <boost/optional.hpp>
#include <set>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

//requires C:\src\boost_1_74_0; C:\src\stb-master; C:\VulkanSDK\1.2.162.1\Include; C:\src\glm; C:\src\glfw-3.3.2.bin.WIN64\include;

////////////////////////////////////
//// vulkan-p3dpge interactions ////
////////////////////////////////////

namespace VkP3DPGE{
	struct Vertex{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
	
		static VkVertexInputBindingDescription getBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

	struct UniformBufferObject{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct QueueFamilyIndices {
		boost::optional<uint32> graphicsFamily;
		boost::optional<uint32> presentFamily;
		bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
}

namespace std {
	template<> struct hash<VkP3DPGE::Vertex> {
		size_t operator()(VkP3DPGE::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct VulkanRenderAPI : p3dpge_vk::RenderAPI {
	//////////////////////////////
	//// vulkan api variables ////
	//////////////////////////////
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
	std::vector<VkP3DPGE::Vertex> vertices = {
		{{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	std::vector<uint32> indices = {
		0, 1, 2, 2, 3, 0
	};
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkCommandBuffer> commandBuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
	
	///////////////////////////////
	//// user config variables ////
	///////////////////////////////

	//TODO(r,delle) INSERT VIDEO SETTINGS HERE
	//rendering parameters
	uint32 windowWidth = 800;
	uint32 windowHeight = 600;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	uint32 mipLevels;
	int MAX_FRAMES_IN_FLIGHT = 2;

	/////////////////////////////////
	//// pge interface functions ////
	/////////////////////////////////

	void Init() override { initVulkan(); }
	void Draw() override { drawFrame(); }
	void Cleanup() override { cleanupVulkan(); }

	//////////////////////////
	//// vulkan functions ////
	//////////////////////////

	void initVulkan();
	void cleanupVulkan();
	void cleanupSwapChain();

	//cleans up prev swap chain, recreates it and things that depend on it
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Swap_chain_recreation
	void recreateSwapChain();

	//TODO(r,delle) INSERT VIDEO SETTINGS HERE
	void createInstance();

	void setupDebugMessenger();

	//https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface
	void createSurface();

	//https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
	void pickPhysicalDevice();

	//creates an interface between the actual GPU device and a virtual device for interaction
	//https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Logical_device_and_queues
	void createLogicalDevice();

	//creates a set of images that can be drawn to and presented to the window
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Swap_chain
	void createSwapChain();

	//creates an image view for each swap chain image to describe how to use/write to that image
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Image_views
	void createImageViews();

	//TODO(r,delle) INSERT VIDEO SETTINGS HERE
	//how many color/depth buffers, their sample amounts, how they should be rendered
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes
	void createRenderPass();

	//creates (storage) buffers or images to be used by shaders
	//https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer
	void createDescriptorSetLayout();

	//TODO(r,delle) INSERT VIDEO SETTINGS HERE
	//almost all the parameters for doing graphics/rendering
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Introduction
	void createGraphicsPipeline();

	//creates a pool for commands to be executed on a device
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Command_buffers
	void createCommandPool();

	//creates colorImage with selected msaaSamples
	//https://vulkan-tutorial.com/en/Multisampling
	void createColorResources();

	//creates depthImage, a depth buffer image after finding optimal settings
	//https://vulkan-tutorial.com/en/Depth_buffering
	void createDepthResources();

	//creates a framebuffer per image view using the renderpass
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Framebuffers
	void createFramebuffers();

	//creates staging image on CPU, copies texture pixels to it, creates vulkan image on GPU from the staging image
	//https://vulkan-tutorial.com/en/Texture_mapping/Images
	void createTextureImage();

	//creates textureImageView that specifies the attributes of the textureImage already created
	//https://vulkan-tutorial.com/en/Texture_mapping/Image_view_and_sampler
	void createTextureImageView();
	
	//TODO(r,delle) INSERT VIDEO SETTINGS HERE
	//used for filtering/anti-aliasing, this uses Repeat sampling
	//https://vulkan-tutorial.com/en/Texture_mapping/Image_view_and_sampler#page_Samplers
	void createTextureSampler();
	
	//creates a vertex buffer on CPU and GPU, fills a CPU staging buffer, copies it to the GPU
	//https://vulkan-tutorial.com/en/Vertex_buffers/Vertex_buffer_creation
	void createVertexBuffer();
	
	//creates an index buffer on CPU and GPU, fills a CPU staging buffer, copies it to the GPU
	//https://vulkan-tutorial.com/en/Vertex_buffers/Index_buffer
	void createIndexBuffer();
	
	//creates and allocates a uniform buffer per swap chain image
	//https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer
	void createUniformBuffers();

	//creates a pool of descriptors for a buffer (per image) to be sent to shaders
	//https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_pool_and_sets
	void createDescriptorPool();

	//creates the actual sets of info to be sent to the GPU
	//https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_pool_and_sets
	void createDescriptorSets();
	
	//TODO(r,delle) INSERT VIDEO SETTINGS HERE
	//creates a command buffer per frame buffer
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Command_buffers
	void createCommandBuffers();
	
	//creates semaphores and fences indicating: images are ready, rendering is finished
	//[GPU-GPU sync] semaphores coordinate operations across command buffers so that they execute in a specified order	(pause threads)
	//[CPU-GPU sync] fences are similar but are waited for in the code itself rather than threads						(pause code)
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Rendering_and_presentation
	void createSyncObjects();

	//grabs an image from swap chain, submits the command buffer to the command queue, adds the image to the presentation queue
	//https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Rendering_and_presentation
	void drawFrame();

	//TODO(r,delle) INSERT VIDEO SETTINGS HERE
	//updates the uniform buffers used by shaders
	//https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer
	void updateUniformBuffer(uint32 currentImage);

	///////////////////////////
	//// utility functions ////
	///////////////////////////

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	
	//device requires: anistrophy
	bool isDeviceSuitable(VkPhysicalDevice device);

	VkP3DPGE::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	
	VkP3DPGE::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	
	//https://vulkan-tutorial.com/en/Multisampling
	VkSampleCountFlagBits getMaxUsableSampleCount();

	//this controls color formats
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	
	//this controls vsync/triple buffering
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	
	//returns the drawable dimensions of the window
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	
	//creates an image view specifying how to use an image
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32 mipLevels);

	//searches the device a format matching the arguments
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	
	//tries to find a viable depth format
	VkFormat findDepthFormat();
	//creates a shader module from the shader bytecode
	VkShaderModule createShaderModule(const std::vector<char>& code);

	//finds which memory types the graphics card offers
	uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);
	
	//creates and binds a vulkan image to the GPU
	void createImage(uint32 width, uint32 height, uint32 mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, 
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	//creates a buffer of defined usage and size on the device
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	//returns a command buffer that will only execute once
	VkCommandBuffer beginSingleTimeCommands();
	
	//ends a command buffer and frees that buffers memory
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	//converts a VkImage from one layout to another using an image memory barrier
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32 mipLevels);

	//uses commands to copy a buffer to an image
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);

	//scans an image for max possible mipmaps and generates them
	//https://vulkan-tutorial.com/en/Generating_Mipmaps
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32 texWidth, int32 texHeight, uint32 mipLevels);

	//copies a buffer, we use this to copy from CPU to GPU
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
};