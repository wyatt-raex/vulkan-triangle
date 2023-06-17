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


	void createInstance() {
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
		createInfo.enabledLayerCount = 0;

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
		bool is_current_ext_supported = false;

		/*
		 * Iterate through every glfwExtension.
		 *	For every glfwExtension, iterate through all Vulkan Instance Extensions and check if the
		 *	name of one Vulkan Instance Extension matches the current glfwExtension.
		 *
		 * If `is_current_ext_supported` is never changed to true, then the current glfwExtension isn't
		 * supported. Otherwise we'll finish both loops and return true;
		 */
		std::cout << "Supported GLFW extensions:" << std::endl;
		for (uint32_t i = 0; i < *glfwExtensionCount; i++) {
			is_current_ext_supported = false; // Reset for every glfwExtension
			
			for (const auto& extension : *vulkan_instance_extensions) {
				if (strcmp(glfw_extensions[i], extension.extensionName) == 0) {
					std::cout << '\t' << glfw_extensions[i] << std::endl;
					is_current_ext_supported = true;
					break;
				}
			}

			if (!is_current_ext_supported) return false;
		}

		return true;
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
};


int main(int argc, char* argv[]) {
	HelloTriangleApplication app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
