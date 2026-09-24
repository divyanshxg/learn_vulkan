#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <vector>

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<char const *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
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
  GLFWwindow *window = nullptr;

  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;

  // Our debug messenger
  vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  std::vector<const char *> getRequiredInstanceExtensions() {

    // Extensions required by GLFW
    uint32_t glfwExtensionCount = 0;

    auto glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    // Required by MoltenVK for portability enumeration
#ifdef __APPLE__
    extensions.push_back(vk::KHRPortabilityEnumerationExtensionName);
#endif

    // Required for VK_EXT_debug_utils
    if (enableValidationLayers) {
      extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
  }

  // ------------------------------------------------------------
  // DEBUG CALLBACK
  // ------------------------------------------------------------

  static VKAPI_ATTR vk::Bool32 VKAPI_CALL
  debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                vk::DebugUtilsMessageTypeFlagsEXT type,
                const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {

    std::cerr << "[Vulkan] " << vk::to_string(severity) << " | "
              << vk::to_string(type) << "\n"
              << pCallbackData->pMessage << "\n\n";

    return vk::False;
  }

  // ------------------------------------------------------------
  // DEBUG MESSENGER
  // ------------------------------------------------------------

  void setupDebugMessenger() {

    if (!enableValidationLayers) {
      return;
    }

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &debugCallback};

    debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo);
  }

  // ------------------------------------------------------------
  // INSTANCE
  // ------------------------------------------------------------

  void createInstance() {

    constexpr vk::ApplicationInfo appInfo{
        .pApplicationName = "Hello Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = vk::ApiVersion14};

    auto requiredExtensions = getRequiredInstanceExtensions();

    auto extensionProperties = context.enumerateInstanceExtensionProperties();

    auto unsupportedPropertyIt = std::ranges::find_if(
        requiredExtensions,
        [&extensionProperties](auto const &requiredExtension) {
          return std::ranges::none_of(
              extensionProperties,
              [requiredExtension](auto const &extensionProperty) {
                return strcmp(extensionProperty.extensionName,
                              requiredExtension) == 0;
              });
        });

    if (unsupportedPropertyIt != requiredExtensions.end()) {
      throw std::runtime_error("Required extension not supported: " +
                               std::string(*unsupportedPropertyIt));
    }

    // --------------------------------------------------------
    // VALIDATION LAYERS
    // --------------------------------------------------------

    std::vector<const char *> requiredLayers;

    if (enableValidationLayers) {
      requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    auto layerProperties = context.enumerateInstanceLayerProperties();

    auto unsupportedLayerIt = std::ranges::find_if(
        requiredLayers, [&layerProperties](auto const &requiredLayer) {
          return std::ranges::none_of(
              layerProperties, [requiredLayer](auto const &layerProperty) {
                return strcmp(layerProperty.layerName, requiredLayer) == 0;
              });
        });

    if (unsupportedLayerIt != requiredLayers.end()) {
      throw std::runtime_error("Required layer not supported: " +
                               std::string(*unsupportedLayerIt));
    }

    // --------------------------------------------------------
    // CREATE INSTANCE
    // --------------------------------------------------------

    vk::InstanceCreateInfo createInfo{

#ifdef __APPLE__
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
#endif

        .pApplicationInfo = &appInfo,

        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),

        .ppEnabledLayerNames = requiredLayers.data(),

        .enabledExtensionCount =
            static_cast<uint32_t>(requiredExtensions.size()),

        .ppEnabledExtensionNames = requiredExtensions.data()};

    instance = vk::raii::Instance(context, createInfo);
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

    glfwDestroyWindow(window);
    glfwTerminate();
  }
};

int main() {

  try {

    HelloTriangleApplication app;
    app.run();

  } catch (const std::exception &e) {

    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
