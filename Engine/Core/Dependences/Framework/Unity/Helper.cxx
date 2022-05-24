#include <macros.h>

#ifdef SR_WIN32
    #include "../Helper/src/Platform/PlatformWindows.cpp"
#endif

#include "../Helper/src/ECS/Component.cpp"
#include "../Helper/src/ECS/GameObject.cpp"
#include "../Helper/src/ECS/ISavable.cpp"
#include "../Helper/src/ECS/Transform.cpp"
#include "../Helper/src/ECS/EntityManager.cpp"
#include "../Helper/src/ECS/Transform3D.cpp"

#include "../Helper/src/Events/EventManager.cpp"
#include "../Helper/src/Events/Event.cpp"
#include "../Helper/src/Events/EventDispatcher.cpp"

#include "../Helper/src/FileSystem/FileSystem.cpp"
#include "../Helper/src/FileSystem/Path.cpp"

#include "../Helper/src/Input/InputSystem.cpp"
#include "../Helper/src/Input/InputDispatcher.cpp"

#include "../Helper/src/Math/Matrix3x3.cpp"
#include "../Helper/src/Math/Quaternion.cpp"
#include "../Helper/src/Math/Vector3.cpp"

#include "../Helper/src/TaskManager/TaskManager.cpp"

#include "../Helper/src/ResourceManager/IResource.cpp"
#include "../Helper/src/ResourceManager/ResourceInfo.cpp"
#include "../Helper/src/ResourceManager/ResourcesHolder.cpp"
#include "../Helper/src/ResourceManager/ResourceManager.cpp"

#include "../Helper/src/CommandManager/ICommand.cpp"
#include "../Helper/src/CommandManager/CmdManager.cpp"

#include "../Helper/src/Types/String.cpp"
#include "../Helper/src/Types/Time.cpp"
#include "../Helper/src/Types/Time.cpp"
#include "../Helper/src/Types/Marshal.cpp"
#include "../Helper/src/Types/Thread.cpp"
#include "../Helper/src/Types/SafeQueue.cpp"
#include "../Helper/src/Types/RawMesh.cpp"

#include "../Helper/src/Utils/Stacktrace.cpp"
#include "../Helper/src/Utils/StringUtils.cpp"
#include "../Helper/src/Utils/Features.cpp"

#include "../Helper/src/World/Chunk.cpp"
#include "../Helper/src/World/Observer.cpp"
#include "../Helper/src/World/Region.cpp"
#include "../Helper/src/World/Scene.cpp"

#include "../Helper/src/Debug.cpp"
#include "../Helper/src/Xml.cpp"
