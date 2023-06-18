/*
* Explinations on what things do what and what I understand about them will be
* located in my Obsidian notes. 
* - Reminder, try to keep comments in code brief, explicit, and clear. Add note 
*   to which note file further explination is located.
*/

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

//#define NDEBUG
#ifdef NDEBUG
	const bool log_extensions = false;
	const bool enableValidationLayers = false;
#else 
	const bool log_extensions = true;
	const bool enableValidationLayers = true;
#endif


VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
) {

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}


private:
	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;


	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Not using OpenGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Don't allow resizing

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}


	void initVulkan() {
		createInstance();
		setupDebugMessenger();
	}


	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}


	void cleanup() {
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void createInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// Optional struct but helpful
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Required struct. Needs required extension(s) info from GLFW
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Get required extension(s) info from GLFW
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		// Check for extension support [OPTIONAL]
		// Check if all GLFW extensions are supported by our current list of Vulkan Instance Extensions
		if (isGLFWExtensionsSupported(&extensions)) {
			std::cout << "All GLFW Extensions supported: true" << std::endl;
		}
		else {
			std::cout << "All GLFW Extensions supported: false" << std::endl;
		}

		// Create the instance.
		/* VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); */
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}


	void setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			throw std::runtime_error("failed to set up debug messenger!");
	}

	
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {	
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional
	}


	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}


	bool isGLFWExtensionsSupported(
		std::vector<const char*>* glfw_extensions
	) {
		/*
		 * Iterate through every glfwExtension.
		 *	For every glfwExtension, iterate through all Vulkan Instance Extensions and check if the
		 *	name of one Vulkan Instance Extension matches the current glfwExtension.
		 *
		 * If `extension_found` is never changed to true, then the current glfwExtension isn't
		 * supported. Otherwise we'll finish both loops and return true;
		 */
		
		uint32_t vk_extension_count = 0;
		// Get number of extensions and store into `vk_extension_count
		vkEnumerateInstanceExtensionProperties(nullptr, &vk_extension_count, nullptr); 
		std::vector<VkExtensionProperties> vk_extensions(vk_extension_count);
	
		// Now get the names of each extension then print it out to std::out
		vkEnumerateInstanceExtensionProperties(nullptr, &vk_extension_count, vk_extensions.data());

		if (log_extensions) {
			std::cout << "Available Vulkan Extensions:" << std::endl;

			for (const auto& extension : vk_extensions) {
				std::cout << '\t' << extension.extensionName << std::endl;
			}
		}	

		std::cout << "Supported GLFW extensions:" << std::endl;
		for (const auto& glfw_extension : *glfw_extensions) {
			bool extension_found = false; //Reset for every glfw_extension
			
			for (const auto& vk_extension : vk_extensions) {

				if (strcmp(glfw_extension, vk_extension.extensionName) == 0) {
					if (log_extensions)
						std::cout << '\t' << glfw_extension << std::endl;
					
					extension_found = true;
					break;
				}
			}

			if (!extension_found) return false;
		}

		return true;
	}


	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) return false;
		}

		return true;
	}


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}


};


int main(int argc, char* argv[]) {
	if (enableValidationLayers) {
		std::cout << "VALIDATION LAYERS ENABLED!" << std::endl;
	}
	else {	
		std::cout << "VALIDATION LAYERS DISABLED!" << std::endl;
	}

	HelloTriangleApplication app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
