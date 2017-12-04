cmake_minimum_required(VERSION 3.8)
project(Urho3DCopy)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Container/RefCounted.cpp Container/RefCounted.h Core/Object.cpp Core/Object.h Core/Context.cpp Core/Context.h Math/StringHash.cpp Math/StringHash.h Math/MathDefs.h Math/MathDefs.cpp Container/Ptr.h Container/Str.cpp Container/Str.h Container/VectorBase.cpp Container/VectorBase.h Core/Variant.cpp Core/Variant.h Container/Vector.cpp Container/Vector.h Container/Swap.cpp Container/Swap.h Container/LinkedList.cpp Container/LinkedList.h Container/HashMap.cpp Container/HashMap.h Container/HashBase.cpp Container/HashBase.h Math/Vector2.cpp Math/Vector2.h Core/StringUtils.cpp Core/StringUtils.h Core/Thread.cpp Core/Thread.h Container/HashSet.cpp Container/HashSet.h)
add_executable(Urho3DCopy ${SOURCE_FILES})