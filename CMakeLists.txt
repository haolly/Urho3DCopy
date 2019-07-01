cmake_minimum_required(VERSION 3.9)
project(Urho3DCopy)

set(CMAKE_CXX_STANDARD 11)

include_directories(Source/ThirdParty)
include_directories(Source/ThirdParty/LZ4)
include_directories(Source/ThirdParty/PugiXml)
include_directories(Source/ThirdParty/SDL)
include_directories(Source/ThirdParty/SDL/include)
include_directories(Source/ThirdParty/STB)

add_definitions (-DURHO3D_FILEWATCHER)
add_definitions (-DURHO3D_THREADING)
add_definitions (-DDESKTOP_GRAPHICS)
add_definitions (-DURHO3D_PHYSICS)
add_definitions (-DURHO3D_URHO2D)
add_definitions (-D_DEBUG)
# test
#add_definitions (-DURHO3D_D3D11)
# does not work
add_definitions (-DURHO3D_OPENGL)

#add_executable(Urho3DCopy
#        Source/Container/Allocator.cpp
#        Source/Container/Allocator.h
#        Source/Container/ArrayPtr.h
#        Source/Container/Hash.h
#        Source/Container/HashBase.cpp
#        Source/Container/HashBase.h
#        Source/Container/HashMap.h
#        Source/Container/HashSet.cpp
#        Source/Container/HashSet.h
#        Source/Container/LinkedList.cpp
#        Source/Container/LinkedList.h
#        Source/Container/List.h
#        Source/Container/ListBase.h
#        Source/Container/Pair.h
#        Source/Container/Ptr.h
#        Source/Container/RefCounted.cpp
#        Source/Container/RefCounted.h
#        Source/Container/Sort.h
#        Source/Container/Str.cpp
#        Source/Container/Str.h
#        Source/Container/Swap.cpp
#        Source/Container/Swap.h
#        Source/Container/Vector.cpp
#        Source/Container/Vector.h
#        Source/Container/VectorBase.cpp
#        Source/Container/VectorBase.h
#        Source/Core/Attribute.h
#        Source/Core/Context.cpp
#        Source/Core/Context.h
#        Source/Core/CoreEvent.h
#        Source/Core/Mutex.cpp
#        Source/Core/Mutex.h
#        Source/Core/Object.cpp
#        Source/Core/Object.h
#        Source/Core/StringUtils.cpp
#        Source/Core/StringUtils.h
#        Source/Core/Thread.cpp
#        Source/Core/Thread.h
#        Source/Core/Timer.cpp
#        Source/Core/Timer.h
#        Source/Core/Variant.cpp
#        Source/Core/Variant.h
#        Source/Engine/EngineEvents.h
#        Source/IO/AbstractFile.h
#        Source/IO/Deserializer.cpp
#        Source/IO/Deserializer.h
#        Source/IO/File.cpp
#        Source/IO/File.h
#        Source/IO/FileSystem.cpp
#        Source/IO/FileSystem.h
#        Source/IO/FileWatcher.cpp
#        Source/IO/FileWatcher.h
#        Source/IO/IOEvent.h
#        Source/IO/Log.cpp
#        Source/IO/Log.h
#        Source/IO/MemoryBuffer.cpp
#        Source/IO/MemoryBuffer.h
#        Source/IO/PackageFile.cpp
#        Source/IO/PackageFile.h
#        Source/IO/Serializer.cpp
#        Source/IO/Serializer.h
#        Source/Math/Color.cpp
#        Source/Math/Color.h
#        Source/Math/MathDefs.cpp
#        Source/Math/MathDefs.h
#        Source/Math/StringHash.cpp
#        Source/Math/StringHash.h
#        Source/Math/Vector2.cpp
#        Source/Math/Vector2.h
#        Source/Resource/BackgroundLoader.cpp
#        Source/Resource/BackgroundLoader.h
#        Source/Resource/Image.cpp
#        Source/Resource/Image.h
#        Source/Resource/Resource.cpp
#        Source/Resource/Resource.h
#        Source/Resource/ResourceCache.cpp
#        Source/Resource/ResourceCache.h
#        Source/Resource/ResourceEvents.h
#        Source/Resource/XMLElement.cpp
#        Source/Resource/XMLElement.h
#        Source/Resource/XMLFile.cpp
#        Source/Resource/XMLFile.h
#        Source/Scene/Animatable.cpp
#        Source/Scene/Animatable.h
#        Source/Scene/Serializable.cpp
#        Source/Scene/Serializable.h
#        Source/Scene/ValueAnimationInfo.cpp
#        Source/Scene/ValueAnimationInfo.h
#        Source/Scene/AnimationDefs.cpp
#        Source/Scene/AnimationDefs.h
#        Source/Scene/ValueAnimation.cpp
#        Source/Scene/ValueAnimation.h
#        Source/Scene/ObjectAnimation.cpp
#        Source/Scene/ObjectAnimation.h
#        Source/Graphics/Model.cpp
#        Source/Graphics/Model.h
#        Source/Math/BoundingBox.cpp
#        Source/Math/BoundingBox.h
#        Source/Math/Vector3.cpp
#        Source/Math/Vector3.h
#        Source/Math/Sphere.cpp
#        Source/Math/Sphere.h
#        Source/Graphics/GPUObject.cpp
#        Source/Graphics/GPUObject.h
#        Source/Math/Plane.cpp
#        Source/Math/Plane.h
#        Source/Math/Matrix3x4.cpp
#        Source/Math/Matrix3x4.h
#        Source/Math/Matrix3.cpp
#        Source/Math/Matrix3.h
#        Source/Math/Quaternion.cpp
#        Source/Math/Quaternion.h
#        Source/Graphics/VertexBuffer.cpp
#        Source/Graphics/VertexBuffer.h
#        Source/Graphics/GraphicsDefs.h
#        Source/Graphics/Direct3D11/D3D11VertexBuffer.cpp
#        Source/Graphics/GraphicsDefs.cpp
#        Source/Graphics/IndexBuffer.cpp
#        Source/Graphics/IndexBuffer.h
#        Source/Graphics/Direct3D11/D3D11IndexBuffer.cpp
#        Source/Graphics/Geometry.cpp
#        Source/Graphics/Geometry.h
#        Source/Math/Ray.cpp Source/Math/Ray.h
#        Source/Graphics/Material.cpp
#        Source/Graphics/Material.h
#        Source/Math/Vector4.cpp Source/Math/Vector4.h
#        Source/Graphics/Technique.cpp Source/Graphics/Technique.h
#        Source/Graphics/Graphics.cpp Source/Graphics/Graphics.h
#        Source/IO/VectorBuffer.cpp Source/IO/VectorBuffer.h
#        Source/Graphics/Texture.cpp Source/Graphics/Texture.h
#        Source/Graphics/Direct3D11/D3D11Texture.cpp
#        Source/Graphics/Direct3D11/D3D11GraphicsImpl.cpp Source/Graphics/Direct3D11/D3D11GraphicsImpl.h
#        Source/Graphics/Texture3D.cpp Source/Graphics/Texture3D.h
#        Source/Graphics/Direct3D11/D3D11Texture3D.cpp
#        Source/Graphics/Shader.cpp Source/Graphics/Shader.h
#        Source/Graphics/ShaderVariation.cpp Source/Graphics/ShaderVariation.h
#        Source/Graphics/ConstantBuffer.cpp Source/Graphics/ConstantBuffer.h
#        Source/Graphics/Direct3D11/D3D11ConstantBuffer.cpp Source/Graphics/Direct3D11/D3D11ShaderVariation.cpp
#        Source/Graphics/Direct3D11/D3D11ShaderProgram.h
#        Source/Graphics/Direct3D11/D3D11Graphics.cpp
#        Source/Graphics/RenderSurface.cpp Source/Graphics/RenderSurface.h Source/Graphics/Direct3D11/D3D11RenderSurface.cpp
#        Source/Graphics/Texture2D.cpp Source/Graphics/Texture2D.h
#        Source/Graphics/Direct3D11/D3D11Texture2D.cpp
#        Source/Graphics/GraphicsEvents.h
#        Source/Math/Rect.cpp Source/Math/Rect.h
#        Source/Scene/SceneEvents.cpp Source/Scene/SceneEvents.h
#        Source/Scene/Node.cpp Source/Scene/Node.h
#        Source/Scene/Component.cpp Source/Scene/Component.h
#        Source/Scene/SceneResolver.cpp Source/Scene/SceneResolver.h
#        Source/Scene/ReplicationState.cpp Source/Scene/ReplicationState.h
#        Source/Scene/Scene.cpp Source/Scene/Scene.h
#        Source/Scene/UnknownComponent.cpp Source/Scene/UnknownComponent.h
#        Source/Graphics/Octree.cpp Source/Graphics/Octree.h
#        Source/Graphics/Drawable.cpp Source/Graphics/Drawable.h
#        Source/Graphics/OctreeQuery.cpp Source/Graphics/OctreeQuery.h
#        Source/Core/WorkQueue.cpp Source/Core/WorkQueue.h
#        Source/UI/UIBatch.cpp Source/UI/UIBatch.hpp
#        Source/UI/UIElement.cpp Source/UI/UIElement.hpp
#        Source/Container/FlagSet.cpp Source/Container/FlagSet.hpp
#        Source/UI/UIEvents.hpp Source/UI/Sprite.cpp
#        Source/UI/Sprite.hpp
#        Source/UI/UI.cpp Source/UI/UI.hpp
#        Source/Math/Matrix4.cpp Source/Math/Matrix4.hpp
#        Source/Graphics/GraphicsImpl.hpp
#        Source/Graphics/OpenGL/OGLGraphicsImpl.hpp)

file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.hpp" "Source/*.h")

file(GLOB d3d11 "Source/Graphics/Direct3D11/*.cpp" "Source/Graphics/Direct3D11/*.hpp" "Source/Graphics/Direct3D11/*.h")

message( "d3d11 is" ${d3d11})

if (NOT ANDROID AND NOT ARM AND NOT WEB)
    message("aaaa")
    if (URHO3D_OPENGL)
        message("open gl")
#        add_subdirectory(Source/ThirdParty/GLEW)
        list(REMOVE_ITEM SOURCES ${d3d11})
    elseif(NOT URHO3D_D3D11)
        # d3d9
#        add_subdirectory(Source/ThirdParty/MojoShader)
    endif ()

    if (NOT CMAKE_SYSTEM_NAME STREQUAL Linux)
#        add_subdirectory(Source/ThirdParty/LibCpuId)
    endif ()
endif ()

#add_subdirectory(Source)

add_executable(Urho3DCopy ${SOURCES} Source/Graphics/OpenGL/OGLShaderVariation.cpp)