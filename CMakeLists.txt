cmake_minimum_required(VERSION 3.9)
project(Urho3DCopy)

set(CMAKE_CXX_STANDARD 11)

include_directories(ThirdParty)
include_directories(ThirdParty/LZ4)
include_directories(ThirdParty/PugiXml)
include_directories(ThirdParty/SDL)
include_directories(ThirdParty/SDL/include)
include_directories(ThirdParty/STB)

add_definitions (-DURHO3D_FILEWATCHER)
add_definitions (-DURHO3D_THREADING)
add_definitions (-DDESKTOP_GRAPHICS)

add_executable(Urho3DCopy
        Container/Allocator.cpp
        Container/Allocator.h
        Container/ArrayPtr.h
        Container/Hash.h
        Container/HashBase.cpp
        Container/HashBase.h
        Container/HashMap.h
        Container/HashSet.cpp
        Container/HashSet.h
        Container/LinkedList.cpp
        Container/LinkedList.h
        Container/List.h
        Container/ListBase.h
        Container/Pair.h
        Container/Ptr.h
        Container/RefCounted.cpp
        Container/RefCounted.h
        Container/Sort.h
        Container/Str.cpp
        Container/Str.h
        Container/Swap.cpp
        Container/Swap.h
        Container/Vector.cpp
        Container/Vector.h
        Container/VectorBase.cpp
        Container/VectorBase.h
        Core/Attribute.h
        Core/Context.cpp
        Core/Context.h
        Core/CoreEvent.h
        Core/Mutex.cpp
        Core/Mutex.h
        Core/Object.cpp
        Core/Object.h
        Core/StringUtils.cpp
        Core/StringUtils.h
        Core/Thread.cpp
        Core/Thread.h
        Core/Timer.cpp
        Core/Timer.h
        Core/Variant.cpp
        Core/Variant.h
        Engine/EngineEvents.h
        IO/AbstractFile.h
        IO/Deserializer.cpp
        IO/Deserializer.h
        IO/File.cpp
        IO/File.h
        IO/FileSystem.cpp
        IO/FileSystem.h
        IO/FileWatcher.cpp
        IO/FileWatcher.h
        IO/IOEvent.h
        IO/Log.cpp
        IO/Log.h
        IO/MemoryBuffer.cpp
        IO/MemoryBuffer.h
        IO/PackageFile.cpp
        IO/PackageFile.h
        IO/Serializer.cpp
        IO/Serializer.h
        Math/Color.cpp
        Math/Color.h
        Math/MathDefs.cpp
        Math/MathDefs.h
        Math/StringHash.cpp
        Math/StringHash.h
        Math/Vector2.cpp
        Math/Vector2.h
        Resource/BackgroundLoader.cpp
        Resource/BackgroundLoader.h
        Resource/Image.cpp
        Resource/Image.h
        Resource/Resource.cpp
        Resource/Resource.h
        Resource/ResourceCache.cpp
        Resource/ResourceCache.h
        Resource/ResourceEvents.h
        Resource/XMLElement.cpp
        Resource/XMLElement.h
        Resource/XMLFile.cpp
        Resource/XMLFile.h
        Scene/Animatable.cpp
        Scene/Animatable.h
        Scene/Serializable.cpp
        Scene/Serializable.h
        Scene/ValueAnimationInfo.cpp
        Scene/ValueAnimationInfo.h
        Scene/AnimationDefs.cpp
        Scene/AnimationDefs.h
        Scene/ValueAnimation.cpp
        Scene/ValueAnimation.h
        Scene/ObjectAnimation.cpp
        Scene/ObjectAnimation.h
        Graphics/Model.cpp
        Graphics/Model.h
        Math/BoundingBox.cpp
        Math/BoundingBox.h
        Math/Vector3.cpp
        Math/Vector3.h
        Math/Sphere.cpp
        Math/Sphere.h
        Graphics/GPUObject.cpp
        Graphics/GPUObject.h
        Math/Plane.cpp
        Math/Plane.h
        Math/Matrix3x4.cpp
        Math/Matrix3x4.h
        Math/Matrix3.cpp
        Math/Matrix3.h
        Math/Quaternion.cpp
        Math/Quaternion.h
        Graphics/VertexBuffer.cpp
        Graphics/VertexBuffer.h
        Graphics/GraphicsDefs.h
        Graphics/Direct3D11/D3D11VertexBuffer.cpp
        Graphics/GraphicsDefs.cpp
        Graphics/IndexBuffer.cpp
        Graphics/IndexBuffer.h
        Graphics/Direct3D11/D3D11IndexBuffer.cpp
        Graphics/Geometry.cpp
        Graphics/Geometry.h
        Math/Ray.cpp Math/Ray.h
        Graphics/Material.cpp
        Graphics/Material.h
        Math/Vector4.cpp Math/Vector4.h
        Graphics/Technique.cpp Graphics/Technique.h
        Graphics/Graphics.cpp Graphics/Graphics.h
        IO/VectorBuffer.cpp IO/VectorBuffer.h
        Graphics/Texture.cpp Graphics/Texture.h
        Graphics/Direct3D11/D3D11Texture.cpp
        Graphics/Direct3D11/D3D11GraphicsImpl.cpp Graphics/Direct3D11/D3D11GraphicsImpl.h
        Graphics/Texture3D.cpp Graphics/Texture3D.h
        Graphics/Direct3D11/D3D11Texture3D.cpp
        Graphics/Shader.cpp Graphics/Shader.h
        Graphics/ShaderVariation.cpp Graphics/ShaderVariation.h
        Graphics/ConstantBuffer.cpp Graphics/ConstantBuffer.h
        Graphics/Direct3D11/D3D11ConstantBuffer.cpp Graphics/Direct3D11/D3D11ShaderVariation.cpp
        Graphics/Direct3D11/D3D11ShaderProgram.h
        Graphics/Direct3D11/D3D11Graphics.cpp
        Graphics/RenderSurface.cpp Graphics/RenderSurface.h Graphics/Direct3D11/D3D11RenderSurface.cpp
        Graphics/Texture2D.cpp Graphics/Texture2D.h
        Graphics/Direct3D11/D3D11Texture2D.cpp
        Graphics/GraphicsEvents.h
        Math/Rect.cpp Math/Rect.h)
        Scene/SceneEvents.cpp Scene/SceneEvents.h
        Scene/Node.cpp Scene/Node.h)
