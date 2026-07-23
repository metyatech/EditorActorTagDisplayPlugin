from __future__ import annotations

import hashlib
import json
import textwrap
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


REPO_ROOT = Path(__file__).resolve().parents[3]
FAB_ROOT = REPO_ROOT / "Marketing" / "Fab"
OUTPUT_ROOT = FAB_ROOT / "media"
REVIEW_ROOT = FAB_ROOT / "reviews"
RAW_ROOT = REPO_ROOT / ".verification" / "fab-media" / "raw"
LAYOUT_PATH = FAB_ROOT / "source" / "media-layout.json"
SOURCE_COMMIT = "84f588dadd06f28cd19e5579d90928d603cc6236"

CANVAS = (1920, 1080)
BG = "#111318"
TEXT = "#F5F7FA"
SECONDARY = "#A6ADB8"
ACCENT = "#36C8FF"
CARD = "#161A22"
GREEN = "#58E6A8"
WARM = "#FFB454"


def font(path: str, size: int) -> ImageFont.FreeTypeFont:
    return ImageFont.truetype(path, size=size)


TITLE_FONT = font("C:/Windows/Fonts/segoeuib.ttf", 48)
SUBTITLE_FONT = font("C:/Windows/Fonts/segoeui.ttf", 26)
BODY_FONT = font("C:/Windows/Fonts/segoeui.ttf", 22)
SMALL_FONT = font("C:/Windows/Fonts/segoeui.ttf", 18)
LABEL_FONT = font("C:/Windows/Fonts/segoeuib.ttf", 16)
CODE_FONT = font("C:/Windows/Fonts/consola.ttf", 22)
CODE_SMALL_FONT = font("C:/Windows/Fonts/consola.ttf", 18)


def load_raw(name: str) -> Image.Image:
    path = RAW_ROOT / name
    if not path.is_file():
        raise FileNotFoundError(f"Missing real UE capture: {path}")
    return Image.open(path).convert("RGB")


def viewport(raw: Image.Image) -> Image.Image:
    # The source is a 1920x1080 desktop capture. This crop keeps only the
    # Level Editor viewport; no pixels are drawn into it.
    return raw.crop((32, 134, 1889, 996))


def settings_panel(raw: Image.Image) -> Image.Image:
    # Project Settings window crop; browser chrome and taskbar are excluded.
    return raw.crop((333, 178, 1585, 854))


def fit_cover(image: Image.Image, size: tuple[int, int]) -> Image.Image:
    target_w, target_h = size
    source_w, source_h = image.size
    scale = max(target_w / source_w, target_h / source_h)
    resized = image.resize((round(source_w * scale), round(source_h * scale)), Image.Resampling.LANCZOS)
    left = max(0, (resized.width - target_w) // 2)
    top = max(0, (resized.height - target_h) // 2)
    return resized.crop((left, top, left + target_w, top + target_h))


def fit_contain(image: Image.Image, size: tuple[int, int], fill: str = CARD) -> Image.Image:
    target_w, target_h = size
    scale = min(target_w / image.width, target_h / image.height)
    resized = image.resize((round(image.width * scale), round(image.height * scale)), Image.Resampling.LANCZOS)
    result = Image.new("RGB", size, fill)
    result.paste(resized, ((target_w - resized.width) // 2, (target_h - resized.height) // 2))
    return result


def base_canvas() -> Image.Image:
    image = Image.new("RGB", CANVAS, BG)
    pixels = image.load()
    for y in range(CANVAS[1]):
        blend = y / CANVAS[1]
        r = round(17 + 7 * blend)
        g = round(19 + 8 * blend)
        b = round(24 + 12 * blend)
        for x in range(CANVAS[0]):
            pixels[x, y] = (r, g, b)
    return image


def rounded_image(base: Image.Image, image: Image.Image, box: tuple[int, int, int, int], radius: int = 18, border: str = ACCENT) -> None:
    draw = ImageDraw.Draw(base)
    x, y, w, h = box
    draw.rounded_rectangle((x + 8, y + 10, x + w + 8, y + h + 10), radius=radius, fill="#090B10")
    draw.rounded_rectangle((x, y, x + w, y + h), radius=radius, fill=CARD, outline=border, width=2)
    clipped = fit_cover(image, (w - 4, h - 4))
    mask = Image.new("L", clipped.size, 0)
    ImageDraw.Draw(mask).rounded_rectangle((0, 0, clipped.width - 1, clipped.height - 1), radius=max(0, radius - 3), fill=255)
    base.paste(clipped, (x + 2, y + 2), mask)


def text(draw: ImageDraw.ImageDraw, xy: tuple[int, int], value: str, used_font: ImageFont.FreeTypeFont, fill: str = TEXT) -> None:
    draw.text(xy, value, font=used_font, fill=fill)


def heading(draw: ImageDraw.ImageDraw, title: str, tagline: str) -> None:
    text(draw, (96, 88), title, TITLE_FONT)
    text(draw, (98, 150), tagline, SUBTITLE_FONT, SECONDARY)


def footer(draw: ImageDraw.ImageDraw) -> None:
    text(draw, (96, 1018), "Demo scene not included.", SMALL_FONT, SECONDARY)
    label = "metyatech"
    width = draw.textbbox((0, 0), label, font=SMALL_FONT)[2]
    text(draw, (1824 - width, 1018), label, SMALL_FONT, TEXT)


def proof(draw: ImageDraw.ImageDraw, value: str, color: str = ACCENT) -> None:
    box = (96, 930, 420, 972)
    draw.rounded_rectangle(box, radius=18, fill="#202632", outline=color, width=2)
    text(draw, (116, 940), value, LABEL_FONT, color)


def media_title(draw: ImageDraw.ImageDraw, label: str, box: tuple[int, int, int, int], color: str = ACCENT) -> None:
    x, y, _, _ = box
    draw.rounded_rectangle((x + 18, y + 18, x + 160, y + 56), radius=12, fill="#111318", outline=color, width=2)
    text(draw, (x + 36, y + 27), label, LABEL_FONT, color)


def make_hero() -> None:
    image = base_canvas()
    draw = ImageDraw.Draw(image)
    heading(draw, "ACTOR METADATA OVERLAY", "See the data. Stay in the viewport.")
    box = (96, 226, 1728, 662)
    rounded_image(image, viewport(load_raw("selected-screen.png")), box)
    proof(draw, "RULE-BASED EDITOR OVERLAYS")
    footer(draw)
    image.save(OUTPUT_ROOT / "01-actor-metadata-overlay-hero.jpg", quality=94, optimize=True, subsampling=0)


def make_before_after() -> None:
    image = base_canvas()
    draw = ImageDraw.Draw(image)
    heading(draw, "METADATA, WITHOUT THE PANEL HUNT", "The same level. The context is finally visible.")
    left = (96, 264, 824, 560)
    right = (1000, 264, 824, 560)
    rounded_image(image, viewport(load_raw("before-off-screen.png")), left, border="#303743")
    rounded_image(image, viewport(load_raw("selected-screen.png")), right, border=ACCENT)
    media_title(draw, "BEFORE", left, SECONDARY)
    media_title(draw, "AFTER", right, ACCENT)
    footer(draw)
    image.save(OUTPUT_ROOT / "02-before-after.jpg", quality=94, optimize=True, subsampling=0)


def make_rule_settings() -> None:
    image = base_canvas()
    draw = ImageDraw.Draw(image)
    heading(draw, "RULES THAT MATCH YOUR WORKFLOW", "Classes, tags, colors, distance and templates.")
    rounded_image(image, settings_panel(load_raw("project-settings-rule0-screen.png")), (96, 226, 1728, 662), border=ACCENT)
    proof(draw, "FIRST MATCH WINS", GREEN)
    footer(draw)
    image.save(OUTPUT_ROOT / "03-rule-settings.jpg", quality=94, optimize=True, subsampling=0)


def make_selected_mode() -> None:
    image = base_canvas()
    draw = ImageDraw.Draw(image)
    heading(draw, "FOCUS ON WHAT YOU SELECT", "Selected Actors mode keeps the viewport quiet.")
    rounded_image(image, viewport(load_raw("selected-screen.png")), (96, 226, 1728, 662), border=ACCENT)
    proof(draw, "SELECTED / ALL / OFF", ACCENT)
    footer(draw)
    image.save(OUTPUT_ROOT / "04-selected-mode.jpg", quality=94, optimize=True, subsampling=0)


def make_distance_bounds() -> None:
    image = base_canvas()
    draw = ImageDraw.Draw(image)
    heading(draw, "SEE WHAT MATTERS. HIDE THE REST.", "Distance filtering and optional bounds for dense levels.")
    rounded_image(image, viewport(load_raw("all-distance-bounds-screen.png")), (96, 226, 1728, 662), border=GREEN)
    proof(draw, "PER-RULE CONTROL", GREEN)
    footer(draw)
    image.save(OUTPUT_ROOT / "05-distance-and-bounds.jpg", quality=94, optimize=True, subsampling=0)


def make_template_tokens() -> None:
    image = base_canvas()
    draw = ImageDraw.Draw(image)
    heading(draw, "SHOW THE DATA YOU NEED", "Labels, tags, layers and safe direct properties.")
    screenshot_box = (96, 226, 1088, 662)
    code_box = (1220, 226, 604, 662)
    rounded_image(image, viewport(load_raw("selected-screen.png")), screenshot_box, border=ACCENT)
    x, y, w, h = code_box
    draw.rounded_rectangle((x + 8, y + 10, x + w + 8, y + h + 10), radius=18, fill="#090B10")
    draw.rounded_rectangle((x, y, x + w, y + h), radius=18, fill=CARD, outline=ACCENT, width=2)
    text(draw, (x + 30, y + 28), "TEMPLATE TOKENS", LABEL_FONT, ACCENT)
    lines = [
        "{ActorLabel}",
        "{ActorClass}",
        "{ActorTags}",
        "{GameplayTags}",
        "{DataLayers}",
        "{Property:State}",
        "{Property:Priority}",
    ]
    line_y = y + 90
    for line in lines:
        text(draw, (x + 30, line_y), line, CODE_FONT, TEXT)
        line_y += 58
    proof(draw, "FLEXIBLE TEMPLATES", ACCENT)
    footer(draw)
    image.save(OUTPUT_ROOT / "06-template-tokens.jpg", quality=94, optimize=True, subsampling=0)


def make_previews(finals: list[Path]) -> None:
    REVIEW_ROOT.mkdir(parents=True, exist_ok=True)
    thumbs = []
    for path in finals:
        thumb = Image.open(path).convert("RGB")
        thumb.thumbnail((320, 180), Image.Resampling.LANCZOS)
        canvas = Image.new("RGB", (320, 180), BG)
        canvas.paste(thumb, ((320 - thumb.width) // 2, (180 - thumb.height) // 2))
        thumbs.append((path.stem, canvas))
    sheet = Image.new("RGB", (960, 540), BG)
    for index, (name, thumb) in enumerate(thumbs):
        sheet.paste(thumb, ((index % 3) * 320, (index // 3) * 180))
    sheet.save(REVIEW_ROOT / "contact-sheet.jpg", quality=92, optimize=True)
    sheet.resize((640, 360), Image.Resampling.LANCZOS).save(REVIEW_ROOT / "gallery-preview.jpg", quality=92, optimize=True)
    thumbs[0][1].save(REVIEW_ROOT / "thumbnail-preview.jpg", quality=92, optimize=True)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def write_manifest(finals: list[Path]) -> None:
    entries = []
    source_map = {
        "01-actor-metadata-overlay-hero.jpg": [".verification/fab-media/raw/selected-screen.png"],
        "02-before-after.jpg": [".verification/fab-media/raw/before-off-screen.png", ".verification/fab-media/raw/selected-screen.png"],
        "03-rule-settings.jpg": [".verification/fab-media/raw/project-settings-rule0-screen.png"],
        "04-selected-mode.jpg": [".verification/fab-media/raw/selected-screen.png"],
        "05-distance-and-bounds.jpg": [".verification/fab-media/raw/all-distance-bounds-screen.png"],
        "06-template-tokens.jpg": [".verification/fab-media/raw/selected-screen.png"],
    }
    for path in finals:
        with Image.open(path) as opened:
            entries.append({
                "filename": path.name,
                "width": opened.width,
                "height": opened.height,
                "format": "JPEG",
                "mode": opened.mode,
                "bytes": path.stat().st_size,
                "sha256": sha256(path),
                "source_captures": source_map[path.name],
                "capture_engine": "Unreal Engine 5.8 Level Editor viewport",
                "ai_generated": False,
            })
    manifest = {
        "product": "Actor Metadata Overlay",
        "source_commit": SOURCE_COMMIT,
        "design_system": "Fab 1920x1080 dark overlay system",
        "capture_project": ".verification/fab-media/host/ActorMetadataOverlaySmoke.uproject",
        "capture_date": "2026-07-23",
        "assets": entries,
    }
    (FAB_ROOT / "media-manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")


def main() -> None:
    OUTPUT_ROOT.mkdir(parents=True, exist_ok=True)
    make_hero()
    make_before_after()
    make_rule_settings()
    make_selected_mode()
    make_distance_bounds()
    make_template_tokens()
    finals = sorted(OUTPUT_ROOT.glob("*.jpg"))
    if len(finals) != 6:
        raise RuntimeError(f"Expected six final JPGs, found {len(finals)}")
    make_previews(finals)
    write_manifest(finals)
    print(json.dumps({"finals": [path.name for path in finals], "manifest": str(FAB_ROOT / "media-manifest.json")}, indent=2))


if __name__ == "__main__":
    main()
