//
// Created by Nikita on 25.03.2021.
//

#include <Environment/Vulkan.h>
#include <ResourceManager/ResourceManager.h>
#include <FileSystem/FileSystem.h>

namespace Framework::Graphics{
    bool Vulkan::PreInit(unsigned int smooth_samples, const std::string& appName, const std::string& engineName) {
        Helper::Debug::Graph("Vulkan::PreInit() : init vulkan application...");

        VkApplicationInfo applicationInfo   = {};
        applicationInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName    = appName.c_str();
        applicationInfo.pEngineName         = engineName.c_str();
        applicationInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion          = VK_API_VERSION_1_2;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo     = &applicationInfo;

        bool validationQueried = false;

        if (!m_extRequired.empty()) {
            if (!VulkanTools::CheckInstanceExtensionSupported(m_extRequired)){
                Helper::Debug::Error("Vulkan::PreInit() : not supported required extensions!");
                return false;
            }

            instanceCreateInfo.enabledExtensionCount = (unsigned __int32)m_extRequired.size();
            instanceCreateInfo.ppEnabledExtensionNames = m_extRequired.data();

            validationQueried = true;
            Helper::Debug::Graph("Vulkan::PreInit() : vulkan validation enabled.");
        }

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &(m_vkInstance)) != VK_SUCCESS) {
            Helper::Debug::Error("Vulkan::PreInit() : failed create vulkan instance!");
            return false;
        } else
            Helper::Debug::Graph("Vulkan::PreInit() : vulkan instance successfully created!");

        if (validationQueried) {
            VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfoExt = {};
            debugReportCallbackCreateInfoExt.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            debugReportCallbackCreateInfoExt.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            debugReportCallbackCreateInfoExt.pfnCallback = DebugVulkanCallback;

            auto vkCreateDebugReportCallbackEXT =
                    (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
                            m_vkInstance, "vkCreateDebugReportCallbackEXT");

            if (vkCreateDebugReportCallbackEXT(
                    m_vkInstance,
                    &debugReportCallbackCreateInfoExt,
                    nullptr, &(this->m_validationReportCallBack)) != VK_SUCCESS) {
                Helper::Debug::Error("Vulkan::PreInit() : failed create vulkan debug report callback info!");
                return false;
            } else
                Helper::Debug::Graph("Vulkan::PreInit() : successfully create vulkan debug report callback!");
        }

        return true;
    }

    bool Vulkan::MakeWindow(const char *winName, bool fullScreen, bool resizable) {
        Helper::Debug::Graph("Vulkan::MakeWindow() : creating window...");

        if (!this->m_winFormat) {
            Helper::Debug::Error("Vulkan::MakeWindow() : format isn't initialized!");
            return false;
        }

        this->m_basicWindow = new Win32Window();
        if (!this->m_basicWindow->Create(winName, 0, 0, m_winFormat->Width(), m_winFormat->Height(), fullScreen, resizable)) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to create window!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : initializing window surface...");
        this->m_vkSurface = VulkanTools::InitWindowSurface(m_vkInstance, m_basicWindow);
        if (!m_vkSurface) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to initialize window surface!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : creating device...");
        this->m_device = VulkanTools::InitDevice(
                this->m_vkInstance, this->m_vkSurface,
                { VK_KHR_SWAPCHAIN_EXTENSION_NAME }, //VK_KHR_SURFACE_EXTENSION_NAME
                { "VK_LAYER_KHRONOS_validation" }, //  VK_LAYER_LUNARG_standard_validation
                false);
        if (!m_device.IsReady()) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to initialize device!");
            Helper::Debug::Error("Vulkan::MakeWindow() : you need install Vulkan drivers on your PC!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : creating swapchain...");
        this->m_swapchain = VulkanTools::InitSwapchain(m_device, m_vkSurface);
        if (!m_swapchain.m_ready) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to initialize swapchain!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : create image views...");
        if (!VulkanTools::CreateImageViews(m_device, &m_swapchain)) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to create image views!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : create command pool...");
        if (!VulkanTools::CreateCommandPool(&m_device, &m_swapchain)) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to create command pool!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : create semaphore...");
        if (!VulkanTools::CreateVulkanSemaphore(m_device, &m_swapchain)) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to create semaphore!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : initialize render pass...");
        this->m_vkRenderPass = VulkanTools::InitRenderPass(m_device, m_swapchain);
        if (m_vkRenderPass == VK_NULL_HANDLE) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to init render pass!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : initializing synchronization...");
        this->m_swapchainImageAvailable = VulkanTools::InitSynchronizations(m_device);
        if (this->m_swapchainImageAvailable == VK_NULL_HANDLE) {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to initialize synchronization!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : create depth stencil image...");
        if (!VulkanTools::CreateDepthStencilImage(
                &m_device,
                &m_swapchain,
                Helper::Math::Vector2(m_basicWindow->GetWidth(), m_basicWindow->GetHeight())))
        {
            Helper::Debug::Error("Vulkan::MakeWindow() : failed to create depth stencil image!");
            return false;
        }

        Helper::Debug::Graph("Vulkan::MakeWindow() : creating frame buffers...");
        this->m_vkFrameBuffers.resize(m_swapchain.m_swapchainImages.size());
        for (unsigned __int32 i = 0; i < m_swapchain.m_swapchainImages.size(); ++i) {
            m_vkFrameBuffers[i] = VulkanTools::CreateFrameBuffer(
                    m_vkRenderPass,
                    m_device,
                    { m_swapchain.m_depthStencilImageView, m_swapchain.m_swapchainImageViews[i] },
                    Helper::Math::Vector2(m_basicWindow->GetWidth(), m_basicWindow->GetHeight()));

            if (m_vkFrameBuffers[i] == VK_NULL_HANDLE) {
                Helper::Debug::Error("Vulkan::MakeWindow() : failed to create frame buffer!");
                return false;
            }
        }

        return true;
    }

    bool Vulkan::CloseWindow() {
        Helper::Debug::Graph("Vulkan::CloseWindow() : close window...");

        if (m_validationReportCallBack == VK_NULL_HANDLE)
            return false;
        else {
            auto vkDestroyDebugReportCallbackEXT =
                    (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(
                            m_vkInstance, "vkDestroyDebugReportCallbackEXT");

            vkDestroyDebugReportCallbackEXT(
                    m_vkInstance,
                    this->m_validationReportCallBack,
                    nullptr);

            this->m_validationReportCallBack = VK_NULL_HANDLE;
            Helper::Debug::Graph("Vulkan::CloseWindow() : vulkan validation report callback successfully destroyed!");
        }

        //!=============================================================================================================

        VulkanTools::DeInitSynchronizations(m_device, &m_swapchainImageAvailable);
        Helper::Debug::Graph("Vulkan::CloseWindow() : vulkan synchronization successfully destroyed!");

        VulkanTools::DeInitSwapchain(this->m_device, &this->m_swapchain);
        Helper::Debug::Graph("Vulkan::CloseWindow() : vulkan swapchain successfully destroyed!");

        VulkanTools::DeInitDevice(&this->m_device);
        Helper::Debug::Graph("Vulkan::CloseWindow() : vulkan device successfully destroyed!");

        if (m_vkSurface == VK_NULL_HANDLE)
            return false;
        else {
            VulkanTools::DeInitWindowSurface(m_vkInstance, &m_vkSurface);
            Helper::Debug::Graph("Vulkan::CloseWindow() : vulkan surface successfully destroyed!");
        }

        if (m_vkInstance == VK_NULL_HANDLE)
            return false;
        else {
            vkDestroyInstance(m_vkInstance, nullptr);
            m_vkInstance = VK_NULL_HANDLE;

            Helper::Debug::Graph("Vulkan::CloseWindow() : vulkan instance successfully destroyed!");
        }

        return true;
    }

    bool Vulkan::Init(int swapInterval) {
        Helper::Debug::Graph("Vulkan::Init() : initializing vulkan...");

        m_basicWindow->SetCallbackResize([this](BasicWindow* win, int w, int h) {

            g_callback(WinEvents::Resize, win, &w, &h);
        });

        return true;
    }

    VkBool32 DebugVulkanCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj,
                                         size_t location, int32_t code, const char *layerPrefix, const char *msg,
                                         void *userData)
    {
        Helper::Debug::Error("DebugVulkanCallback() : " + std::string(msg));
        return VK_FALSE;
    }


    bool Vulkan::CompileShader(const std::string &name, IShaderProgram *shaderProgram) const noexcept {
        if (!shaderProgram)
            return false;

        std::string vertex_path   = Helper::ResourceManager::GetResourcesFolder() + "\\Shaders\\" + GetPipeLineName() + "\\" + name + "_vertex.glsl";
        std::string fragment_path = Helper::ResourceManager::GetResourcesFolder() + "\\Shaders\\" + GetPipeLineName() + "\\" + name + "_fragment.glsl";;

        std::string vertCode = Helper::FileSystem::ReadAllText(vertex_path);
        std::string fragCode = Helper::FileSystem::ReadAllText(fragment_path);

        auto* vkShader = reinterpret_cast<VulkanShader*>(shaderProgram);

        vkShader->m_vertShaderModule = VulkanTools::CreateShader(m_device, vertCode);
        vkShader->m_fragShaderModule = VulkanTools::CreateShader(m_device, fragCode);

        if (!vkShader->IsReady()) {
            Helper::Debug::Error("Vulkan::CompileShader() : failed to compile \""+name + "\" shader!");
            return false;
        }

        return true;
    }

    void Vulkan::SetWindowSize(unsigned int w, unsigned int h) {
        if (m_winFormat->GetValue() != Types::WindowFormat::Free) {
            w = m_winFormat->Width();
            h = m_winFormat->Height();
        } else
            m_winFormat->SetFreeValue(w, h);

        if (Helper::Debug::GetLevel() >= Helper::Debug::Level::High)
            Helper::Debug::Log("Vulkan::SetWindowSize() : width = " + std::to_string(w) + "; height = "+ std::to_string(h));


    }
}