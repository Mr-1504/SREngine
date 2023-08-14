# SpaRcle Engine

<img width="1920" alt="image_2023-05-31_21-40-59" src="https://github.com/Monika0000/SREngine/assets/47291246/6bf73c99-81a2-4a3e-ad28-4c788884eb38">
<img width="1920" alt="image_2023-05-31_21-40-59" src="https://github.com/Monika0000/SREngine/assets/47291246/32cfdbcb-5a0d-4a74-9501-539da40379e2">

## Supported compilers
- [x] MSVC (Stable)
- [x] MinGW
- [ ] Cygwin
- [x] Clang
- [ ] GCC

## Supported platforms
- [x] Windows 10
- [ ] Ubuntu
- [ ] Arch
- [ ] Redhat
- [ ] Android

## Branches 
 * master - branch for releases
 * dev - active development branch 
 * features/* - branches for new features
 * tmp/* - temporary branches for dangerous experiments

## Building
 0. Requirements:
    1. Install Clion 2020.1 / Visual Studio 2019
    2. Install [Vulkan SDK/Runtime](https://vulkan.lunarg.com/sdk/home) 1.3.211.0
    3. Install CMake 3.16.0
    4. Install [MS Build Tools](https://download.visualstudio.microsoft.com/download/pr/996d318f-4bd0-4f73-8554-ea3ed556ce9b/9610a60ad452c33dd3f9e8d3b4ce5d88d278f8b063d88717e08e1c0c13c29233/vs_BuildTools.exe) for Visual Studio 16.11.29 (if you are using CLion)
 2. Clone repository via the command "git clone https://github.com/Monika0000/SREngine"
 3. Switch to the "dev" (or another one) branch via the command "git checkout branch_name"
 4. Run command "git submodule update --init --recursive" in repository folder
 5. Working with CMake (if you're using Visual Studio): 
    1. Choose the path to the SREngine folder (where the source code is located).
    2. Choose the path where to build the binaries (SREngine/Build).
    3. Press "Configure" and choose your VS version.
    4. Press "Generate"
    5. Build and run Engine.exe with command line argument "-resources path-to-your-repository/Resources"
 6. Working with CLion: 
    1. Press Ctrl + Alt + S to open Settings window
    2. Select "Build, Execution, Deployment" to the left, then "Toolchains"
    3. Add (Alt + Insert) Visual Studio compiler
    4. Select amd64 in "Architecture:"
    5. In Project window right-click SREngine folder and click "Reload CMake Project"
    6. Click "Run 'SREngine'" (Shift + F10)

## Used libraries
  * glm
  * OpenGL / GLFW / GLEW / glad
  * Bullet3
  * PhysX
  * Box2D
  * [ImGUI](https://github.com/ocornut/imgui)
  * [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
  * [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)
  * [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)
  * stbi
  * json
  * [EvoScript](https://github.com/Monika0000/EvoScript)
  * [EvoVulkan](https://github.com/Monika0000/EvoVulkan)
  * Assimp
  
  ## Features
  * SpaRcle Shader Language (SRSL)
  * Chunk system 
  * Render passes
  * Custom framebuffers
  * C++ scripting system
  * Animations 
  * Physics
  * Font renderer
  * Vulkan pipeline
  * Multi-cameras renderer 
  * Geometry loader
  * Texture loader
