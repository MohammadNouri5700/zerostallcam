cmake_minimum_required(VERSION 3.22.1)
project("zerostallcam")

add_library(${CMAKE_PROJECT_NAME} SHARED
        JNI/JniBridge.cpp
        Systems/Engine.cpp
        Systems/CameraSystem.cpp
        Systems/BufferSystem.cpp
        Systems/RenderSystem.cpp
        Systems/EglManager.cpp
        Systems/ShaderManager.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        camera2ndk
        mediandk
        EGL
        GLESv3
        jnigraphics
        log)
