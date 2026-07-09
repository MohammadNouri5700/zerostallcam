# **High-Performance NDK-Accelerated OpenGL Rendering**



ZeroStallCam is a high-performance graphics engine for Android that leverages the **Android NDK** and **OpenGL ES** to deliver low-latency, hardware-accelerated rendering. By bypassing the standard Android View hierarchy and using a zero-copy hardware buffer pipeline, ZeroStallCam achieves "zero-stall" performance, making it ideal for real-time computer vision and augmented reality.

## **Performance Profiling**

ZeroStallCam is engineered for stability. Our profiling results demonstrate a highly optimized pipeline that consistently exceeds the 60 FPS requirement with minimal CPU overhead. Traces can be analyzed using [Perfetto UI](https://ui.perfetto.dev/).

| total_frames_captured | avg_frame_time_ms | max_frame_time_ms | avg_blocking_time_ms |
| --- | --- | --- | --- |
| 1668 | 15.28 | 22.3 | 1.32 |

## **Performance Analysis**
*   **Targeting 60 FPS:** With an **average frame time of 15.28 ms**, ZeroStallCam fits within the 16.66 ms budget required for a fluid 60 FPS experience.
*   **Thread Efficiency:** The **1.32 ms blocking time** indicates that the native thread is rarely starved for CPU resources. Synchronization between JNI and C++ is minimized to prevent "jank."
*   **GPU vs CPU Budget:** The engine utilizes `GL_EXT_disjoint_timer_query` (where supported) to measure GPU execution time independently of CPU logic, ensuring a balanced workload.

## **Core Technology & Optimization**

ZeroStallCam is built on a custom **Entity Component System (ECS)** architecture, optimizing data locality and minimizing cache misses. Our "Zero-Stall" promise is achieved through several advanced rendering techniques:

### **1. Zero-Copy Direct-to-GPU Pipeline**
Traditional Android camera previews often involve copying data from the camera buffer to a temporary surface. ZeroStallCam utilizes `AImageReader` and `AHardwareBuffer` to map camera frames directly into OpenGL textures via `EGLImageKHR`.
*   **Performance Gain:** Eliminates the CPU overhead of pixel copying.
*   **Latency:** The camera writes directly to a buffer that the GPU samples as a `GL_TEXTURE_EXTERNAL_OES` texture.

### **2. std140 Uniform Buffer Objects (UBOs)**
State updates (Transforms, Timestamps) are batched using UBOs.
*   **Memory Layout:** We use the `std140` layout to ensure C++ structures (aligned via `alignas(16)`) match the GPU's memory expectations exactly.
*   **Efficiency:** Updating a UBO is significantly faster than multiple `glUniform` calls and reduces the state-change footprint in the driver.

### **3. Asynchronous GPU Profiling**
We utilize `GL_EXT_disjoint_timer_query` to measure actual GPU execution time independently of the CPU. This provides high-fidelity performance metrics without the pipeline stalls associated with `glFinish()`.

## **Project Structure & File Map**

| Category | File | Description |
| :--- | :--- | :--- |
| **Orchestration** | `JniBridge.cpp` | Primary JNI entry point; maps JVM lifecycle events to Native Engine calls. |
| | `Engine.h/.cpp` | Core orchestrator; manages ECS data and the high-level frame loop. |
| **Systems** | `BufferSystem.h/.cpp` | Manages `AImageReader` and zero-copy hardware buffer acquisition. |
| | `CameraSystem.h/.cpp` | NDK Camera2 wrapper; handles device discovery and capture sessions. |
| | `RenderSystem.h/.cpp` | GPU pipeline manager; handles textures, UBOs, and draw calls. |
| | `EglManager.h/.cpp` | EGL lifecycle manager; initializes GLES 3.0 context and extensions. |
| | `ShaderManager.h/.cpp` | Utility for loading and compiling GLSL shaders from assets. |
| **Components** | `GraphicsComponent.h` | Container for OpenGL handles, EGL images, and profiling metrics. |
| | `CameraComponent.h` | Stores NDK Camera2 handles and state callbacks. |
| | `BufferComponent.h` | Stores `AImageReader` and native window references. |
| | `TransformComponent.h` | `std140` aligned data for screen/camera transformations. |
| | `TimestampComponent.h` | `std140` aligned epoch data for shader overlays. |
| | `HardwareBufferLayouts.h`| Vertex structure definitions optimized for GPU alignment. |
| **Shaders** | `scene.vert` | Procedural vertex shader; calculates geometry on-the-fly via vertex IDs. |
| | `scene.frag` | Multi-sampler fragment shader; blends camera OES and font textures. |

## **Frame Lifecycle**
Understanding the path of a single pixel is critical to our optimization strategy:

Vsync -> JNI -> Engine -> Buffer -> OpenGL -> SurfaceFlinger -> Display

## **Detailed Engine Flow**

1.  **Choreographer Trigger:** The JVM layer triggers the JNI bridge upon every Vsync.
2.  **Buffer Acquisition:** `BufferSystem` acquires the latest `AImage` from the `AImageReader` queue.
3.  **Texture Mapping:** The `AHardwareBuffer` is converted to an `EGLImage` and bound to a `GL_TEXTURE_EXTERNAL_OES` target.
4.  **UBO Update:** `TransformComponent` and `TimestampComponent` data are uploaded to the GPU using `glBufferSubData`.
5.  **Rendering:** A procedural GLSL 3.0 shader processes the frame and overlays in a single pass.
6.  **Swap & Display:** `EglManager` performs a buffer swap, handing the frame to Android's `SurfaceFlinger`.

## **Getting Started**

### **Prerequisites**
*   Android Studio Ladybug (or newer)
*   Android NDK (Side-by-side)
*   CMake 3.22.1+

### **Build Instructions**
1.  **Clone the repository:**
    ```bash
    git clone https://github.com/MohammadNouri5700/zerostallcam.git
    ```
2.  **Open in Android Studio:** Select the root `ZeroStallCam` directory.
3.  **Run:** Click **Run 'app'**. The engine will automatically initialize the NDK layer and start the camera preview.

## **License**
This project is licensed under the Apache License 2.0.

---

