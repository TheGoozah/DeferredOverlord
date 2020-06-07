# Deferred Overlord Engine
Partial version of the DirectX 11 Overlord Engine supporting Deferred Rendering.
Both a **build** and the **source code** is available.

## Interesting files
- [Deferred Renderer](_PROJECT/OverlordEngine/DeferredRenderer.cpp): the heart of the deferred rendering pass. Creates the resources and binds the resources for rendering.
- [Deferred Drawer](_PROJECT/OverlordEngine/DeferredLightDrawer.cpp): presents the final images by accumulating all the information, pushing it to the GPU and invoking the pipeline by using fullscreen quad (computer shader could be used instead).
- [Deferred G-Buffer Fill Pass](_PROJECT/OverlordProject/Resources/Effects/Deferred/BasicEffect_Deferred.fx): hlsl shader filling in the G-Buffer.
- [Deferred G-Buffer Lighting Pass](_PROJECT/OverlordProject/Resources/Effects/Deferred/Deferred_LightPass.fx): hlsl shader used to combine the G-Buffer data and doing the volumetric lighting.
- [HLSL Helper Functions](_PROJECT/OverlordProject/Resources/Effects/Deferred/Deferred_Helpers.fx): hlsl helper functions for the lighting pass.
