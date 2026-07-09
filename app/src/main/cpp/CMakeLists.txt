cmake_minimum_required(VERSION 3.22.1)
project("zerostallcam")

add_library(${CMAKE_PROJECT_NAME} SHARED
        jni/JniBridge.cpp
        engine/Renderer.cpp
        engine/CameraEngine.cpp
        graphics/EglManager.cpp
        graphics/ShaderProgram.cpp
        graphics/FontAtlas.cpp
        graphics/SceneGeometry.cpp
        camera/NDKCamera.cpp
        camera/ImageProvider.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        camera2ndk
        mediandk
        EGL
        GLESv3
        log)
