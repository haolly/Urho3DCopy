cmake_minimum_required(VERSION 3.8)
project(Urho3DCopy)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Container/RefCounted.cpp Container/RefCounted.h Core/Object.cpp Core/Object.h Core/Context.cpp Core/Context.h Math/StringHash.cpp Math/StringHash.h Math/MathDefs.h Math/MathDefs.cpp Container/Ptr.h Container/Str.cpp Container/Str.h Container/VectorBase.cpp Container/VectorBase.h)
add_executable(Urho3DCopy ${SOURCE_FILES})