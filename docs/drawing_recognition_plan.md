**Goal**
- Capture strokes between LMB down/up, render a small on-screen canvas, and classify the drawing in real time. Start with simple shapes (tiny CNN), keep path open for complex, multi-stroke objects.

**Engine Integration**
- **Stroke Capture:** Poll `GLFW_MOUSE_BUTTON_LEFT` and cursor pos each frame; maintain states idle→drawing→done and a vector of 2D points.
- **CPU Canvas:** Rasterize lines into a `64x64` grayscale buffer as points arrive; no GPU readbacks.
- **Overlay:** Upload the CPU canvas to a dynamic `GL_TEXTURE_2D`; render a small quad overlay using existing shader.
- **Preprocess:** Ink bbox → pad/scale to 20x20 within 28x28, center of mass, normalize to [0,1].
- **Inference:** Tiny CNN forward on CPU at 10–20 Hz while drawing; smooth predictions and emit `{label, confidence}` to the main thread.

**Milestones**
- M0: Dynamic texture support
  - Add dynamic texture creation and CPU→GPU update APIs.
  - Verify by uploading a simple pattern and drawing it as a `SceneObject`.
- M1: Stroke capture
  - Add `StrokeRecorder` system; buffer points between LMB down/up.
- M2: Rasterization + Overlay
  - Draw into `64x64` CPU canvas and update dynamic texture each frame.
- M3: Preprocess + Mock Inference
  - Implement MNIST-style preprocessing and a stub classifier for plumbing.
- M4: Training Pipeline
  - Python scripts for dataset (synthetic + Quick, Draw! subset), training, and `.npz` export.
- M5: C++ Inference
  - Implement tiny CNN forward + weight loader; integrate in recognizer.
- M6: UX Polish
  - Smoothing, thresholds, toggles, debug HUD.

**API Changes (M0)**
- `Texture`
  - `Texture(int width, int height, const unsigned char* rgba = nullptr, bool generateMipmaps = false)`
  - `void update(const unsigned char* rgba, bool regenerateMipmaps = false)`
  - Internally track `m_hasMipmaps` and select minification filter accordingly.
- `TextureManager`
  - `bool createDynamicTexture(const std::string& name, int width, int height, const unsigned char* rgba = nullptr)`
  - `bool updateDynamicTexture(const std::string& name, const unsigned char* rgba, bool regenerateMipmaps = false)`

**Next Steps**
- Implement M0 (in this patch) and validate by drawing a test dynamic texture.
- M1–M2: Add `StrokeRecorder` + CPU canvas and hook an overlay `SceneObject` that updates each frame.
- M3–M5: Training scripts, weight export, and C++ inference integration.

