# Zero-CPU Pipeline Remediation Plan

This plan addresses the CPU violations and performance stalls identified in the Senior Architect audit. We will move all domain logic (time calculation) to the GPU and eliminate expensive driver-level stalls (EGLImage creation).

## User Review Required

> [!IMPORTANT]
> This change introduces an EGL Image cache. While this significantly improves performance, it assumes that the `AHardwareBuffer` pointers returned by `AImageReader` are recycled by the system. If the system constantly generates new buffer pointers without recycling, the cache could grow. However, in typical Android Camera/Media flows, these are part of a fixed-size buffer queue.

## Proposed Changes

### [Component] [Components](file:///Users/leo/AndroidStudioProjects/ZeroStallCam/app/src/main/cpp/Components/)

#### [MODIFY] [TimestampComponent.h](file:///Users/leo/AndroidStudioProjects/ZeroStallCam/app/src/main/cpp/Components/TimestampComponent.h)
- Change `h, m, s` fields to a single `epochSeconds` field to offload time math.

#### [MODIFY] [GraphicsComponent.h](file:///Users/leo/AndroidStudioProjects/ZeroStallCam/app/src/main/cpp/Components/GraphicsComponent.h)
- Add `std::unordered_map<AHardwareBuffer*, EGLImageKHR> eglImageCache` to prevent per-frame image creation stalls.

---

### [Component] [Systems](file:///Users/leo/AndroidStudioProjects/ZeroStallCam/app/src/main/cpp/Systems/)

#### [MODIFY] [Engine.cpp](file:///Users/leo/AndroidStudioProjects/ZeroStallCam/app/src/main/cpp/Systems/Engine.cpp)
- Simplify `UpdateTimestamp` to only pass the raw epoch to the GPU.
- Remove all CPU-side performance math (FPS calculation) from the hot path.

#### [MODIFY] [RenderSystem.cpp](file:///Users/leo/AndroidStudioProjects/ZeroStallCam/app/src/main/cpp/Systems/RenderSystem.cpp)
- **Vertex Shader:** Add logic to decompose `epochSeconds` into `H:M:S` directly on the GPU.
- **UpdateCameraBuffer:** Implement cache-lookup for `EGLImageKHR`.
- **DrawFrame:** Use `glBufferSubData` for UBO updates to avoid implicit reallocations.
- **Deinit:** Properly destroy all cached EGL images.

## Verification Plan

### Automated Tests
- Build the project to ensure no C++ compilation errors.
- Run `app:assembleDebug`.

### Manual Verification
- Deploy to device and verify the on-screen clock still functions correctly (this confirms the GPU temporal math).
- Inspect Logcat for "Performance Summary" (now significantly more accurate as CPU overhead is removed).
- Use `systrace` / `perfetto` to verify the disappearance of `eglCreateImageKHR` calls from the frame loop.
