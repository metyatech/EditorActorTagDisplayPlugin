# Actor Metadata Overlay Fab media

This directory contains the six Fab listing images for Actor Metadata Overlay. The final upload set is `media/*.jpg`; `media-manifest.json` records dimensions, byte sizes, SHA-256 checksums, and the real UE 5.8 capture used for each image.

The source capture project is temporary and is not part of the plugin or the Fab upload. The images are generated deterministically by `source/generate-fab-media.py` from real Unreal Engine 5.8 Level Editor viewport captures. Pillow is used only for the outer marketing composition, labels, cards, gradient, and token code card; no pixels are added to the viewport captures.

## Regenerate

From the repository root, after recreating the temporary host and raw captures:

```powershell
python Marketing/Fab/source/generate-fab-media.py
```

The script writes the six final JPEGs to `Marketing/Fab/media/`, review previews to `Marketing/Fab/reviews/`, and updates `Marketing/Fab/media-manifest.json`.

## Upload set

Upload the six JPGs inside `media/` only. Do not upload `source/`, `reviews/`, raw captures, the temporary host project, or build artifacts.
