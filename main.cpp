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
	const bool enableValidationLayers = false;
#else 
	const bool enableValidationLayers = true;
#endif

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


	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Not using OpenGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Don't allow resizing

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}


	void initVulkan() {
		createInstance();
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
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		// Check for extension support [OPTIONAL]
		uint32_t extensionCount = 0;
		// Get number of extensions and store into `extensionCount`
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr); 

		// Now get the names of each extension then print it out to std::out
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "Available extensions:" << std::endl;
		for (const auto& extension : extensions) {
			std::cout << '\t' << extension.extensionName << std::endl;
		}

		// Check if all GLFW extensions are supported by our current list of Vulkan Instance Extensions
		if (isGLFWExtensionsSupported(&extensions, glfwExtensions, &glfwExtensionCount)) {
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


	bool isGLFWExtensionsSupported(
		std::vector<VkExtensionProperties>* vulkan_instance_extensions, 
		const char* glfw_extensions[],
		uint32_t* glfwExtensionCount
	) {
		/*
		 * Iterate through every glfwExtension.
		 *	For every glfwExtension, iterate through all Vulkan Instance Extensions and check if the
		 *	name of one Vulkan Instance Extension matches the current glfwExtension.
		 *
		 * If `extension_found` is never changed to true, then the current glfwExtension isn't
		 * supported. Otherwise we'll finish both loops and return true;
		 */
		std::cout << "Supported GLFW extensions:" << std::endl;
		for (uint32_t i = 0; i < *glfwExtensionCount; i++) {
			bool extension_found = false; // Reset for every glfwExtension
			
			for (const auto& extension : *vulkan_instance_extensions) {
				if (strcmp(glfw_extensions[i], extension.extensionName) == 0) {
					std::cout << '\t' << glfw_extensions[i] << std::endl;
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
