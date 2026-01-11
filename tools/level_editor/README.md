# Ink Level Editor

Simple desktop editor to create and edit `assets/levels/*.json` files.

## Run

From the repo root:

```bash
python3 tools/level_editor/level_editor.py
```

## Notes

- Textures are pulled from `assets/textures` by filename (without `.png`).
- The current loader supports `platform` and `character` objects.
- Use `Add Entity...` for custom types; the runtime will ignore unknown types
  until you add support in `source/core/levelLoader.cc`.
- The canvas preview shows a top-down 2D view (x/y). Drag objects to move them.
- Enable Snap to align dragging to a grid (set size in world units).
- Moving platforms can store a simple 2-point path and speed for future support.
