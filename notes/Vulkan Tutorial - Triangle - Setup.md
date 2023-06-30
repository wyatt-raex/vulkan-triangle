Links [[Vulkan Tutorial]]
Tags: #Computer_Science #Graphics #Programming 

# Local TOC
- [[#Base Code]]
- [[#Instance]]
	- [[#General Pattern to Vulkan Object Creation]]
- [[#Validation Layers]]
- [[#Physical Devices and Queue Families]]
- [[#Logical Device and Queues]]


## Base Code
- [[#Local TOC|Back to Top]]
- [[#Headers]]
- [[#Hello Triangle Application Class]]
- [[#Main]]
- [[#Resource Management]]

### Headers
```C++
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
```

- `<vulkan/vulkan.h>` - <i>provides functions, structures and enumerations</i>
- `<iostream>` & `<stdexcept>` - <i>included for reporting/propagating errors</i>
- `<cstdlib>`- <i>provides </i>`EXIT_SUCCESS` <i>and</i> `EXIT_FAILURE` <i>macros</i>

### Hello Triangle Application Class
```C++
class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initWindow() {}
	void initVulkan() {}
	void mainLoop() {}
	void cleanup() {}
};
```

The program itself and it's functions is located in `HelloTriangleApplication` class. This is where we'll store Vulkan Objects as private class members, and add functions to initiate each of them.
- `initWindow()` - <i>self explanatory. Init the window using GLFW.</i>
- `initVulkan()` - <i>where functions will be called to initialize Vulkan Objects.</i>
- `mainLoop()` - <i>loops until the window is closed and renders frames needed.</i>
- `cleanup()` - <i>where we'll deallocate any resources we've used.</i>

If any fatal error occurs during runtime, a `std::runtime_error` exception will be thrown with a descriptive message and will be propagated back to `main()` where the error will then be displayed in the command prompt.

### Main
```C++
int main(int argc, char** argv) {
	HelloTriangleApplication app;

	try {
		app.run();	
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;	
	}

	return EXIT_SUCCESS;
}
```

To be able to handle a wider variety of exception types, we catch the more general `std::exception`.

### Resource Management
Just like how every chunk of memory allocated with `malloc` needs to be `free` after use, every Vulkan object also needs to be explicitly destroyed when it's no longer needed.

- Vulkan objects are either created:
	- Directly with functions like `vkCreateXXX()`
	- or Allocated through another object with functions like `vkAllocateXXX()`
- To Free/Deallocate Vulkan objects you would use:
	- `vkDestroyXXX()` for created objects
	- `vkFreeXXX()` for allocated objects

Every Vulkan object shares the parameter:
- `pAllocator` - <i>optional parameter that allows you to specify callbacks for custom memory allocator.</i>
	- <i>this parameter will be ignored in the tutorial and will always be passed </i> `nullptr`.

 It is possible to perform automatic resource management using RAII/smart pointers provided by `<memory>` header, but this tutorial will be explicit about memory allocation/deallocation, *because Vulkan's niche is to be explicit about every operation to avoid mistakes*, so it's good to be explicit about the lifetime of objects to learn how the API works.

### Integrating GLFW
#### initWindow()
```C++
private:
	GLFWwindow* window;


	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Not using OpenGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Don't allow resizing

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}
```

Need to begin by initializing GLFW with `glfwInit()`. Because GLFW was originally designed to be used with OpenGL, we tell it not to do so by default with the following call:
- `glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API)`

Right now we're not going to deal with re-sizable windows so we'll disable that for now with:
- `glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE)`

Then we create the window:
- `window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr)`

The first two parameters, `WIDTH` and `HEIGHT` are constants that specify the window width and height, and we've defined them above our `HelloTriangleApplication` class and below our included headers. The 3rd parameter `"Vulkan"` is our window title.

The 4th parameter allow us to optionally specify which monitor the window will open on. While the 5th parameter is only relevant to OpenGL.

#### mainLoop()
```C++
void mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();	
	}
}
```

Mostly self explanatory. The window will remain open until it's closed. `glfwPollEvents()` will look for events such as pressing the X button to close the window, etc.

We'll later call a function in this loop to render a single frame.

#### cleanup()
```C++
void cleanup() {
	glfwDestroyWindow(window);

	glfwTerminate();
}
```

Now we cleanup the resources we've allocated to memory. That being our private member `window` and GLFW itself.


## Instance
- [[#Local TOC|Back to Top]]
- [[#Creating an Instance]]
	- [[#General Pattern to Vulkan Object Creation]]
- [[#Checking for Extension Support]]
- [[#Cleaning Up]]

### Creating an Instance

#### General Pattern to Vulkan Object Creation
```C++
VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
```
General pattern is:
- Pointer to struct with creation info for the object
- Pointer to custom allocator callbacks, <i>which is always</i> `nullptr` <i>in this tutorial.</i>
- Pointer to the variable that stores the handle to the new object.

#### Initial Setup
```C++
private:
/* ... */
VkInstance instance;

/* ... */

void initVulkan() {
	createInstance();
}

void createInstance() {}
```
We'll create a new function to handle getting the information required to create a new Vulkan instance. This will be called from `initVulkan()` and the resulting instance that's created will be stored as a private member `instance` of type `VkInstance`.

The instance itself will require a struct `createInfo{}` in order to be created. `createInfo{}` can optionally be given a struct `appInfo` as one of it's members, while not required the `appInfo` struct is generally useful to have.

#### Application Info
```C++
void createInstance() {
	// Optional but useful struct
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
}
```
<a href="https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkApplicationInfo.html">VkApplicationInfo Documentation Page</a>

*Many structs in Vulkan will require you to specify the struct type in the* `sType` *member.* `appInfo` is also one of many structs that has the `pNext` member which can be used to extend the struct with more information. However, we're going to leave it as it's default value of `nullptr`.

Rest of `appInfo` members are self-explanatory.

#### Create Info
```C++
void createInstance() {
	/* ... */

	// Required struct. Needs required extension(s) info from GLFW
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo

	// Get required extension(s) info from GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;
}
```
<a href="https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkInstanceCreateInfo.html">VkInstanceCreateInfo Documentation Page</a>

First two parameters `sType` and `pApplicationInfo` are self-explanatory. `sType` is the type of the structure. `pApplicationInfo` is a reference to the `appInfo` struct we created earlier.

Because Vulkan is a platform agnostic API, you need an extension in order for Vulkan to be able to communicate with the window system (Official wording: <i>you need an extension to interface with the window system</i>). GLFW has a built-in function in order to get the required extension(s) for this. Now we can set the `enabledExtensionCount` and `ppEnabledExtensionNames` members.
- `enabledExtensionCount` - the number of extensions enabled.
- `ppEnabledExtensionNames` - the names of each extension enabled.

Lastly we've set `enabledLayerCount` which is the number of global layers to enable. Layers will be talked about more in depth here: [[#Validation Layers]]. 
- <i>Similar to </i> `ppEnabledExtensionNames` <i>there is also </i>`ppEnabledLayerNames` <i>as well</i>.

#### Create the Instance
```C++
private:
	/* ... */
	VkInstance instance;

	/* ... */

	void createInstance() {
		/* ... */

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");	
		}
	}
```
<a href="https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkCreateInstance.html">vkCreateInstance() Documentation Page</a>

Finally to create the instance we make a call to the `vkCreateInstance()` function. Passing in parameters for a struct with the instance's information, any allocator callbacks, and a reference to the variable the instance will be stored in. [[#General Pattern to Vulkan Object Creation|See General Pattern to Vulkan Object Creation Here]].

`vkCreateInstance()` has a return value of type `VkResult`, we use the if statement here to ensure that initializing the instance was a success throwing a `std::runtime_error` if it wasn't.

### Checking for Extension Support
```C++
void createInstance() {
	/* ... */

	// Check for extension support [Optional]
	uint32_t extensionCount = 0;
	// Get number of extensions and store into `extensionCount`
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Now get the names of extension then print it out to std::out
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(
		nullptr, 
		&extensionCount, 
		extensions.data()
	);

	std::cout << "Available extensions:" << std::endl;
	for (const auto& extension : extensions) {
		std::cout << '\t' << extension.extensionName << std::endl;	
	}
}
```
<a href="https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkEnumerateInstanceExtensionProperties.html">vkEnumerateInstanceExtensionProperties() Documentation Page</a>
<a href="https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkExtensionProperties.html">VkExtensionProperties Documentation Page</a>

One of the errors we can run into when creating an instance is `VK_ERROR_EXTENSION_NOT_PRESENT`. We could only specify the extensions we require and terminate the program if that error code comes back, which makes sense for essential extensions like the window system interface. But what if we wanted to check for optional functionality.

We can retrieve a list of supported extensions before creating the instance by using the `vkEnumerateInstanceExtensionProperties()` function.
- 1st parameter allows us to filter extensions by a specific validation layer, which we'll ignore for now.
- 2nd parameter is a pointer to a variable that stores the number of extensions.
- 3rd parameter is a pointer to an array of `VkExtensionProperties` which stores details on each extension.

In our code we first need to get the number of extensions, which we can do by calling `vkEnumerateInstanceExtensionProperties()` and passing `nullptr` to both the 1st and 3rd parameters.

Once we've stored the number of extensions in our variable we can then store the details of each extension in our vector (of `<VkExtensionProperties>` aptly called `extensions`) by once again calling `vkEnumerateInstanceExtensionProperties()` and passing in `extensions` as the 3rd parameter.

Finally we then simply output to std::out all the available Instance Extensions.

#### Challenge: Create GLFW Extensions Supported Function
As a challenge, Vulkan Tutorial asks us to create a function that checks if all the extensions that are returned by `glfwGetRequiredInstanceExtensions()` are included in the supported extensions list we acquired above.

```C++
void createInstance() {
	/* ... */

	// Get required extension(s) info from GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	/* ... */

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(
		nullptr, 
		&extensionCount, 
		extensions.data()
	);

	/* ... */

	if (isGLFWExtensionsSupported(
					&extensions, 
					glfwExtensions, 
					&glfwExtensionCount
				)
			) {
		std::cout << "All GLFW Extensions supported: true" << std::endl;	
	}
	else {	
		std::cout << "All GLFW Extensions supported: false" << std::endl;	
	}
}
```
<i>Above is all the code required to call my function</i> `isGLFWExtensionsSupported()`.

- We need the vector of `extensions` acquired from the section above [[#Checking for Extension Support]].
- We also need the list of `glfwExtensions` and how many there are `glfwExtensionCount`. 

```C++
bool isGLFWExtensionsSupported(
	std::vector<VkExtensionProperties>* vulkan_instance_extensions,
	const char* glfw_extensions[],
	uint32_t* glfwExtensionCount
) {
	/*
	* Iterate through every glfwExtension.
	*   For every glfwExtension, iterate through all Vulkan Instance Extensions and
	*   check if the name of one Vulkan Instance Extension matches the current
	*   glfwExtension.
	*
  * If `extension_found` is never changed to true, then the current
  * glfwExtension isn't supported. Otherwise we'll finish both loops and return
  * true.
	*/
	std::cout << "Supported GLFW extensions:" << std::endl;
	for (uint32_t i = 0; i < *glfwExtensionCount; i++) {
		bool extension_found = false; // Reset for every glfwExtension

		for (const auto& extension : *vulkan_instance_extensions) {
			if (strcmp(glfw_extensions[i], extension.extensionName) == 0) {
				std::cout << '\t' << glfw_extensions[i] << std::endl;
				is_current_ext_suppported = true;
				break;	
			}	
		}

		if (!extension_found) return false;
	}

	return true;
}
```
Moving into the function itself we can initially set a variable `extension_found`  (is the current glfw extension supported?) to false.

Then as the comment explains, we check if every glfwExtension exists in the list of Vulkan Instance Extensions. Returning true if all glfwExtensions are in the list of Vulkan Instance Extensions and false otherwise.

### Cleaning Up
```C++
private:
	/* ... */
	VkInstance instance;

	/* ... */

	void cleanup() {
		vkDestroyInstance(instance, nullptr);	
	}
```
<a href="https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkDestroyInstance.html">vkDestroyInstance() Documentation Page</a>

Finally for this section, because we created a new Vulkan Object we also need to be responsible and free the memory it allocated with `vkDestroyInstance()`. `vkDestroyInstance()` can take an optional 2nd parameter that's an allocator callback.

*Importantly, all other Vulkan resources that we end up creating should be cleaned up before the instance is destroyed!*


## Validation Layers
- [[#Local TOC|Back to Top]]
- [[#What are Validation Layers?]]
- [[#Using Validation Layers]]
- [[#Message Callback]]
- [[#Debugging Instance Creation and Destruction]]

### What are Validation Layers?
The Vulkan API is designed around the idea of minimal driver overhead, as such on of the consequences of that design choice is that there is extremely limited error checking in Vulkan by default. 

<i>Validation layers are the solution to this but are not installed with the Vulkan API, they are included with the LunarG Vulkan SDK. These validation layers check for the most common errors and are <a href="https://github.com/KhronosGroup/Vulkan-ValidationLayers">Open Source</a>.</i>

*Validation Layers is Vulkan's optional, togglable components that hook into Vulkan function calls to apply error checking/additional operations.* While they induce overhead, you can toggle different validation layers as you please to target certain debugging/error checking info. Multiple and/or single validation layers can be toggled on at the same time. 

Common "operations" in validation layers are:
- Checking the values of parameters against the specification to detect misuse
- Tracking creation and destruction of objects to find resource leaks
- Checking thread safety by tracking the threads that calls originate from
- Logging every call and its parameters to the standard output
- Tracing Vulkan calls for profiling and replaying

<i>Example implementation of a function in a diagnostics validation layer could look like:</i>
```C++
VkResult vkCreateInstance(
	const VkInstanceCreateInfo* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkInstance* instance
) {
	if (pCreateInfo == nullptr || instance == nullptr) {
		log("Null pointer passed to required parameter!");
		return VK_ERROR_INITIALIZATION_FAILED;	
	}

	return real_vkCreateInstance(pCreateInfo, pAllocator, instance);
}
```

There used to be two different types of validation layers:
- Instance - <i>would only check calls related to global Vulkan objects (e.g. instances)</i>
- Device Specific - <i>would only check calls related to a specific GPU</i>

*Device specific validation layers are now depreciated!* However, the Vulkan specification document still recommends that you enable validation layers at device level as well for compatibility. 
- We will specify the same layers as the instance at the logical device layer [[#Logical Device and Queues|later on]].

### Using Validation Layers
This section goes through how to enable the standard diagnostics layers provided by the Vulkan SDK. Just like extensions, validation layers need to be enabled by specifying their name.
- All of the useful standard validation layers is bundled into a layer included in the SDK that is known as `VK_LAYER_KHRONOS_validation`.

```C++
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
```
Near the top of our code, where our global variables and headers are, we've included a global vector that stores which validation layers we're using `validationLayers` as well as utilized the `NDEBUG` macro to determine whether to enable the validation layers or not.

```C++
bool checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLAyers) {
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
```
<a href="https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkEnumerateInstanceLayerProperties.html">vkEnumerateInstanceLayerProperties() Documentation Page</a>

Then we added a new function:
- `checkValidationLayerSupport()` - <i>checks if all of the requested validation layers are available.</i>

The same way we got, listed, and checked for [[#Checking for Extension Support|extensions]], we do the same with the validation layers this time using `vkEnumerateInstanceLayerProperties()`. Then almost exactly like we did in the [[#Challenge Create GLFW Extensions Supported Function|checking extensions challenge]] we check that the validation layers exist the exact same way.

```C++
void createInstance() {
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");	
	}

	/* ... */

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();	
	} else {
		createInfo.enabledLayerCount = 0;	
	}
}
```
Lastly we add a check to the beginning of `createInstance()` to ensure all validation layers we have requested to use are available. And then later change the `VkInstanceCreateInfo` struct instantiation to include the validation layer names if they are enabled.
- <i>If the check was successful then</i> `vkCreateInstance()` <i>should not ever return a</i> `VK_ERROR_LAYER_NOT_PRESENT` <i>error.</i>

### Message Callback
Validation layers will print debug messages to standard output by default, but we can also handle them ourselves by providing explicit callback in our program. This allows us to decide which kinds of messages we'd like to see, because not all messages are necessarily (fatal) errors.

```C++
std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions
		(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	
	return extensions;
}
```
First we'll separate out our code for getting required extensions and add a check for if `enableValidationLayers` is toggled; if so adding the `VK_EXT_debug_utils` extension which is required to set up callback message handling.

```C++
void createInstance() {
	/* ... */

	// Get required extension(s) info from GLFW
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
}
```
Then edit the `createInstance()` function to account for the `getRequiredExtensions()` function we just created.

```C++
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}
```
Then we'll add our initial debug callback function `debugCallback()` with the prototype `PFN_vkDebugUtilsMessengerCallbackEXT`. By using this prototype we can ensure that the function has the correct function signature expected by Vulkan for a debug messenger callback. With this we can now pass `debugCallback()` as a parameter to Vulkan functions that require a debug messenger callback. The `VKAPI_ATTR` and `VKAPI_CALL` ensure the function has the right signature for Vulkan to call it.

Parameters:
- `messageSeverity` - specifies the severity of the message; can be one of the following flags:
	- `VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT` - Diagnostic message.
	- `VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT` - Informational message like the creation of a resource.
	- `VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT` - Message about behavior that is not necessarily an error, but very likely a bug in your application.
	- `VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT` - Message about behavior that is invalid and may cause crashes.

```C++
if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
	// Message is important enough to show
}
```
With these values of this enumeration being set up this way, we can use a comparison operation to check if the message is equal or worse to some level of severity. Example above.

Parameters (continued):
- `messageType` - can be one of the following values:
	- `VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT` - Some event has happened that is unrelated to the specification or purpose.
	- `VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT` - Something has happened that violates the specification or indicates a possible mistake.
	- `VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT` - Potential non-optimal use of Vulkan.
- `pCallbackData` - refers to a `VkDebugUtilsMessengerCallbackDataEXT` struct containing the details of the message itself; the most important members of which being:
	- `pMessage` - The debug message as a null terminated string.
	- `pObjects` - Array of Vulkan object handles related to the message.
	- `objectCount` - Number of objects in the array.
- `pUserData` - is a pointer that was specified during the setup of the callback and allows you to pass your own data to it.

The callback function `debugCallback()` returns a boolean `VkBool32` that determines if the function that called `debugCallback()` should be aborted with the error `VK_ERROR_VALIDATION_FAILED_EXT`. However, this is normally only used to test validation layers themselves, so you should always return `VK_FALSE`.

 ```C++
private:
	/* ... */
	VkDebugUtilsMessengerEXT debugMessenger;

	/* ... */

	void initVulkan() {
		createInstance();
		setupDebugMessenger();	
	}

	/* ... */

	void setupDebugMessenger() {
		if (!enableValidationLayers) return;	

		VkDebugUtilsMessengerCreateInfo{};
		populateDebugMessengerCreateInfo(createInfo);
	}

	/* ... */

	void populateDebugMessengerCreateInfo(
		VkDebugUtilsMessengerCreateInfoEXT& createInfo
	) {
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional
	}
```
Finally all that remains now other than cleaning up everything we've created, is to tell Vulkan about the callback function. As per usual the debug function is managed with a handle that needs to be explicitly created and destroyed. The callback function we created `debugCallback()` is part of a *debug messenger* and we can have as many of them as we want.

First we'll add a class member for the debug messenger handle right under `instance`. Then we'll add a function `setupDebugMessenger()` to be called after our instance is created via `createInstance()`.

Inside the `setupDebugMessenger()` function, if validation layers are not enabled then there is no point in having a debug messenger as nothing will show, so we'll just return in that case. Otherwise we'll create a struct for the creation of the object and populate it with the information it requires in another function `populateDebugMessengerCreateInfo()`.
- `sType` - The type of the struct being created.
- `messageSeverity` - This struct parameter allows us to specify <u>all</u> the types of severities we would like our callbacks to be called for. (<i>We've specified all types except</i> `VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT` <i>to receive notifications about possible problems while leaving out verbose general debug information</i>)
- `messageType` - Lets us filter which types of messages your callback is notified about. (<i>We've enabled all types here</i>)
- `pfnUserCallback` - Specifies the pointer to the callback function.
- `pUserData` - Optional pointer that will be passed along to the callback function via `pUserData` parameter. (<i>As an example you could pass a pointer to the </i>`HelloTriangleApplication` <i>class</i>)

```C++
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
```
After creating a populating the struct with the information it requires we need to then pass this information to the function `vkCreateDebugUtilsMessengerEXT()`. However, this is a Vulkan extension function, meaning that it is not included in the core Vulkan header/library and is not essential for the base operation of Vulkan. As such we need to include it.

However, this is not as simple as a simple `#include`, to maintain compatibility between many versions of Vulkan as well the different platforms it can be running on *the Vulkan API includes optional functionality dynamically at runtime*.

All of the parameters passed to our `CreateDebugUtilsMessengerEXT()` function are the parameters that will get passed to Vulkan's `vkCreateDebugUtilsMessengerEXT()` function IF the function is found.
- `instance` - Our Vulkan instance we've instantiated.
- `pCreateInfo` - A pointer to the struct containing the information required for the `DebugUtilsMessenger` to be created.
- `pAllocator` - A pointer to a struct `VkAllocataionCallbacks`. Normally set to `nullptr` to set it to Vulkan's default specifications. <i>However, we can specify the information in this struct to specify how we want memory allocated and deallocated for the</i> `DebugUtilsMessenger`.
- `pDebugMessenger` - A pointer to the `debugMessenger` class member.

We then attempt to get a pointer to the `vkCreateDebugUtilsMessengerEXT()` function using `vkGetInstanceProcAddr()` and store it in the variable `func`. This function returns a pointer to the requested function if found and `nullptr` otherwise.

```C++
void setupDebugMessenger() {
	/* ... */

	if (CreateDebugUtilsMessengerEXT(
			instance,
			&createInfo,
			nullptr,
			&debugMessenger	
		) 
	!= VK_SUCCESS) {

		throw std::runtime_error("failed to set up debug messenger!");
	}
}
```
Now back inside our `setupDebugMessenger()` function we should be able to call `CreateDebugUtilsMessengerEXT()`.

```C++
void DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator
) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, debugMessenger, pAllocator);	
	}
}
```
Finally because we created yet another Vulkan object `debugMessenger` it also needs to be destroyed. Right below our proxy function `CreateDebugUtilsMessengerEXT` we create another proxy function to search for and dynamically include `vkDestroyDebugUtilsMessengerEXT()`.

Remember that the final parameter `pAllocator` is a pointer to a structure containing information on how we want Vulkan to handle memory allocation/deallocation. We will stick to the defaults by passing `nullptr` to this parameter.

```C++
void cleanup() {
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerExt(instance, debugMessenger, nullptr);	
	}

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}
```
Now we add the necessary calls in our `cleanup()` function to handle destroying our `debugMessenger` Vulkan object.

### Debugging Instance Creation and Destruction
At this point debugging with validation layers has been added to the program but we still don't have debugging enabled for instance creation and destroying. The `vkCreateDebugUtilsMessengerEXT()` requires a valid instance to have been created and destroyed, and the `vkDestroyDebugUtilsMessengerEXT()` must be called before the instance is destroyed.

```C++
void createInstance() {
	/* ... */

	VkInstanceCCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	/* ... */

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		popualteDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	/* ... */

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");	
	}
}
```
The way to handle this is create a separate debug utils messenger specific for those two function calls, and have that messenger be passed into the `VkInstanceCreateInfo{}` struct as the `pNext` member.

The `debugCreateInfo{}` variable is placed out the if statement to ensure that it is not destroyed before the `vkCreateInstance` call. By creating an additional debug messenger in this way it will automatically be used during `vkCreateInstance()` and `vkDestroyInstance()` and cleaned up after that.

## Physical Devices and Queue Families
- [[#Local TOC|Back to Top]]
- [[#Selecting a Physical Device]]
- [[#Base Device Suitability Checks]]
- [[#Queue Families]]

### Selecting a Physical Device
After initializing the Vulkan library through a `VkInstance` we need to look for and select a GPU that supports the features we need. <i>You can even select and use any number of graphics cards simultaneously, but for this tutorial we'll only use the first GPU that suits our needs</i>.

```C++
private:
	/* ... */
	VkPhsyicalDevice physicalDevice = VK_NULL_HANDLE;

	void initVulkan() {
		createInstance();
		setupDebugMessenger();
		pickPhysicalDevice();	
	}
```
The GPU we end up selecting will be stored in a class member variable `physicalDevice` that'll be of type `VKPhysicalDevice`. We'll handle finding and selecting the appropriate GPU in a new method `pickPhysicalDevice()`.

```C++
void pickPhysicalDevice() {
	// List available GPUs with Vulkan compatability
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");	
	}

	// Store allocated VkPhysicalDevice handles in a vector
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");	
	}
}

bool isDeviceSuitable(VkPhysicalDevice device) {
	return true;
}
```
Very similar to what we did for extensions, we query the number of graphics cards then after we've gotten the number of GPUs available we store the `VkPhysicalDevice` handles in a vector.

We'll then introduce a new method `isDeviceSuitable()` which will check if the GPU is suitable for all the operations we want to perform. We'll cover it's implementation in [[#Base Device Suitability Checks]] so for now it'll just return true.  

We can then loop through all the devices in the vector, calling `isDeviceSuitable()` on each till we find one that suits our needs. Lastly we'll have a check to ensure we've found a GPU to use, and if not we'll throw a runtime error.

### Base Device Suitability Checks
*The following code for this section doesn't get implemented into the main.cpp file. It's mostly some examples to show how we could go about selecting a GPU and viewing it's available properties/features*

```C++
VkPhysicalDeviceProperties deviceProperties;
vkGetPhysicalDeviceProperties(device, &deviceProperties);
```
You can view basic device properties (<i>like the name, type, and supported Vulkan version</i>) by calling `vkGetPhysicalDeviceProperties()`.

```C++
VkPhysicalDeviceFeatures deviceFeatures;
vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
```
For optional features (<i>like texture compression, 64-bit floats, and multi-viewport rendering (useful for VR)</i>) can by queried by calling `vkGetPhysicalDeviceFeatures()`.

Other details can also be queried from devices such as device memory and queue families ([[#Queue Families|see the next section]]).

```C++
bool isDeviceSuitable(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DESCRETE_GPU
					&& deviceFeatures.gemoetryShader;
}
```
If for example our application was only usable with dedicated GPUs that support geometry shaders then our function could look like the one above.

```C++
#include <map>

/* ... */

void pickPhysicalDevice() {
	/* ... */

	// Use an ordered map to autmoatically sort candidates by increasing score
	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices) {
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));	
	}

	// Check if the best candidate is suitable at all
	if (candidates.rbegin()-> first > 0) {
		physicalDevice = candidates.rbegin()->second;	
	} else {
		throw std::runtime_error("failed to find a suitable GPU!");	
	}
}


int rateDeviceSuitability(VkPhysicalDevice device) {
	/* ... */

	int score = 0;

	// Descrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DESCRETE_GPU) {
		score += 100;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without gemoetry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;	
	}

	return score;
}
```
It would also be possible to score each device in the `devices` vector based on how suitable they would be then picking the best one which has the highest score. Such an implementation could look like the above code.

However, in this tutorial we'll only be concerned with finding the first GPU that supports Vulkan and it's necessary features.

### Queue Families
Prior in the tutorial it was briefly mentioned *that almost every operation in Vulkan, anything from drawing to uploading textures, requires commands to be submitted to a queue.* There are also different types of queues that originate from different queue families and *each family of queues allows only a subset of commands*.

```C++
struct QueueFamilyIndices {
	uint32_t graphicsFamily;
}

/* ... */

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;
	// Logic to find queue family indices to populate struct with
	return indices;
}
```
To find and check which queue families are supported by a device, and ensure the device supports the commands we want to use, we'll create a new function `findQueueFamilies()` that looks for all the queue families we need. 

Although for now we're only looking for a queue that supports graphics commands, we'll future proof our program for when we want multiple queues by bundling all the queue indices into a struct `QueueFamilyIndices{}`.

```C++
#include <optional>

/* ... */

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
}

/* ... */

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;
	// Logic to find queue family indices to populate struct with
	return indices;
}
```
However a problem arises, what if a queue family is not available? We could throw an exception in `findQueueFmamilies()`, but that function isn't the right place to make decisions on device suitability. <i>For example, we may prefer devices with dedicated transfer queue family, but not require it</i>. So now we need a way to indicate whether a particular queue family was found.

Because any value of `uint32_t` could be a queue family index (<i>including</i> `0`), it's not really possible to indicate the nonexistence of a queue family using a `uint32_t`. Thankfully, C++17 introduces a data structure just for this case `<optional>`.
- `<optional>` - a wrapper data structure that contains no value until you assign something to it. Checking whether it contains a value or not can be done by calling `has_value()`.

```C++
bool isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.graphicsFamily.has_value();
}


QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	// Logic to find queue family indices to populate struct with
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, 
		queueFamiles.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;	
		}	

		i++;
	}
}
```
Now we can begin the implementation of `findQueueFamilies()`. First we'll do what we've become accustom to and retrieve the list of queue families using `vkGetPhysicalDeviceQueueFamilyProperties()`.

The `VkQueueFamilyProperties` struct contains some details about the queue family, including the type of operations that are supported and the number of queues that can be created based on that family. We need to find at least one queue family that supports `VK_QUEUE_GRAPHICS_BIT`. 
- We'll do so by looping through all the available queue families and checking if one of the struct contains the `VK_QUEUE_GRAPHICS_BIT` via a bit-wise AND operation.
- The variable `i` here keeps track of the index of each queueFamily.

After going through all the available queue families of the device, in `isDeviceSuitable()` we can call `has_value()` on the index we <i>hopefully</i> found and stored to check if the device is suitable.

```C++
/* ... */

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return grahpicsFamily.has_value();	
	}
}

/* ... */

bool isDeviceSuitable(VkPhysicaldevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.isComplete();
}


QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	/* ... */

	for (const auto& queueFamily : queueFamilies) {
		/* ... */

		if (indices.isComplete()) break;

		i++;
	}
}
```
To make this a little more convenient we'll put the call for `has_value()` inside our `QueueFamilyIndices` struct instead and change our code appropriately. Afterwords we can also add a check within our for loop to exit early if we've found the queue family we were looking for.

## Logical Device and Queues
- [[#Local TOC|Back to Top]]
- [[#Specifying the Queues to be Created]]
- [[#Specifying Used Device Features]]
- [[#Creating the Logical Device]]
- [[#Retrieving Queue Handles]]

```C++
private:
/* ... */
VkDevice device;

/* ... */

void initVulkan() {
	createInstance();
	setupDebugMessenger();
	pickPhysicalDevice();
	createLogicalDevice();
}

/* ... */
```
After we've selected a physical device to use, we need to set up a <i>logical device</i> in order to interface with it. The process to set up a logical device is similar to the instance creation process and describes what features we want to use. We also need to specify which queues to create now that we've queried which queue families are available.

We'll start by adding a new class member to store the logical device `device` and create a new method `createLogicalDevice()` to setup our logical device.

### Specifying the Queues to be Created
```C++
void createLogicalDevice() {
	// Begin setup to create our logical device to interface with the GPU.
	QueueFmilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	// Set the scheduling priority of our graphics queue family in the command 
	// buffer.
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;
}
```
The first piece of information we'll set up for our logical device is the queue families it will use and how many of each of them we want (<i>normally just 1</i>). As per usual, we define the creation of most things in Vulkan using structs. The `VkDeviceQueueCreateInfo{}` struct describes the number of queues we want for *a single queue family*.
- `sType` - Type of the struct.
- `queueFamilyIndex` - Index the queue family exists on in the GPU.
- `queueCount` - How many queues of the current queue family we want to create.
- `pQueuePriorities` - Pointer to a float telling Vulkan how to schedule commands coming from this queue into the command buffer. <i>This is required even if there is only a single queue</i>.

### Specifying Used Device Features
```C++
void createLogicalDevice() {
	/* ... */

	// Specify the set of device features we're using.
	VkPhysicalDeviceFeatures{};
}
```
The next piece of information for the logical device is the device features being used. These are queried with `vkGetPhysicalDeviceFeatures()` and an example of such features would be geometry shaders. However right now in just drawing a triangle we don't need any special features, so we'll leave the struct's properties initalization to `VK_FALSE`.

### Creating the Logical Device
```C++
void createLogicalDevice() {
	/* ... */

	// Create the device.
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;
}
```
Now we can fill in the `VkDeviceCreateInfo{}` structure to create our logical device. Firstly we'll pass pointers to the queue creation and device features structs.

```C++
void createLogicalDevice() {
	/* ... */

	// Create the device
	VkDeviceCreateInfo createInfo{};

	/* ... */

	createInfo.enabledExtensionCount = 0;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>
			(validationLayers.size());	
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;	
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != 
			VK_SUCCESS) {

		throw std::runtime_error("failed to create logical device!");
	}
}
```
The remainder of the `VkDeviceCreateInfo{}` struct is very similar to the `VkInstanceCreateInfo{}` struct in that it requires us to specify the extensions and validation layers we're going to use. The difference between the two is that these extensions and validation layers are device specific this time. For now we won't need any device extensions and we'll enable validation layers depending on `enableValidationLayers`.
- <i>An example of a device specific extension is</i> `VK_KHR_swapchain`<i>, which allows you to present rendered images from that device to windows</i>.

Lastly, we'll instantiate the logical device with a call to `vkCreateDevice()`. The parameters in order are:
- `physicalDevice` - The physical device to interface with.
- `&createInfo` - The queue and usage info we just specified.
- `nullptr` - The optional allocation callbacks pointer.
- `&device` - A pointer to a variable to store the logical device handle in.

```C++
void cleanup() {
	vkDestroyDevice(device, nullptr);

	/* ... */
}
```
Then all that's left is to make sure to clean up our logical device object when we're done with it.

### Retrieving Queue Handles
```C++
private:
/* ... */

VkQueue graphicsQueue;

/* ... */

	void createLogicalDevice() {
		/* ... */

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	}
```
Now while the queues are automatically created alongside the logical device, we don't have a handle to interface with them yet. So we first create a class member to store the handle for the graphics queue `graphicsQueue`.

Then after we've instantiated our logical device we'll make a call to `vkGetDeviceQueue()` to retrieve the queue handles for each queue family. The parameters are in order:
- `device` - The logical device to get queue handles from.
- `indices.graphicsFamily.value()` - The queue family to get a handle for.
- `0` - The index of the queue family itself? (<i>I'm a bit unsure on this.</i>)
- `&graphicsQueue` - A pointer to the variable to store the queue handle in.

*Device queues are implicitly cleaned up when the logical device is destroyed, so we don't have to do anything with* `graphicsQueue` *inside our* `cleanup()` *function.*

[[#Local TOC|Back to Top]]