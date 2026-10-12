# Making the 4K art with an online image model

You generate each image with your online model (GPT Image 2.5 Sunburst) and the tools do everything else: layout, prompts, fitting to the exact canvas, checks and packing into the game. Rules for the art itself: `docs/art-guide.md`.

All commands run from `C:\Games\GameHercules\Remaster\tools\hires` with `..\..\.venv\Scripts\python.exe` (written `py` below).

## 0. Prompt packs (already made for the pilot)

`remastered/prompts/README.md` lists the packs in order. Each `remastered/prompts/<pack>/` folder holds:

| file | what to do with it |
|---|---|
| `input.png` | upload it to the model as the image to edit or redraw |
| `prompt.txt` | paste it as the prompt |
| `layout.json` | leave it; `fit.py` reads it |
| `<pack>.png` | **you save the model's result here**, same name as the folder (e.g. `bird_flap/bird_flap.png`) |

**Image size.** `input.png` is already at the size to request (1536×1024, 1024×1536 or 1024×1024). Each original pixel is a block of 6–9 pixels in it.
- If the model can output larger images (2048 or 4096 px), rebuild the packs with fewer frames per pack and closer to the 9× target: `py prompts.py --sizes 2048x2048,4096x2304` (any list of sizes it supports).
- If it returns a different size with the same shape, `fit.py` rescales it.

## 1. Generate

Start with the character anchor, then keep it as a style reference:

1. **`hero_idle`:** upload `input.png`, paste `prompt.txt`, generate. Repeat until Hercules looks right in all 8 cells: same face and costume, nothing moved.
2. **Every later hero pack:** upload its `input.png` **and** your approved `hero_idle` result as a second image (the prompt already says to match a style reference).
3. **Birds:** do `bird_flap` first, then use it as the style reference for `bird_dive`.
4. **Tile, HUD and TRAINING texture pieces:** one generation each, no reference needed.

Save every download into its pack folder, named after the folder: `remastered/prompts/bird_flap/bird_flap.png`. The tools only look there.

**What to reject straight away:**
- anything moved, cropped or resized inside a cell;
- extra or missing limbs or frames;
- a background that isn't flat magenta;
- shadows, a floor, text or frame numbers.

**Refusals.** The service may refuse to recreate Disney characters. If it does, try wording such as "remaster my game sprite sheet". Otherwise the same packs work with any other tool, as long as it returns a PNG.

## 2. Take the result (fit + check + review in one step)

```
py jobs.py take bird_flap         # one pack: reads remastered/prompts/bird_flap/bird_flap.png
py jobs.py take                   # every pack that has a <pack>.png
```

This step:
- keys out the magenta (soft edges, no pink fringe);
- cuts each cell out and resamples it to 9× the original;
- aligns each sprite frame first: the model often draws the figure larger or shifted inside its cell, so the cell is scaled (up to ±30 %) and moved until its silhouette best overlaps the original (printed as `aligned {...}` with the overlap before/after; `--no-align` switches it off). A low overlap after alignment (below about 0.7) means the pose itself differs: regenerate that pack;
- trims stray pixels outside the original silhouette (grown by 4 px; `--grow N` to change it, `--no-snap` to switch it off);
- writes the frames to `remastered/incoming/<pack>/` (e.g. `incoming/bird_dive/`);
- creates the pack's jobs, runs the checks and writes side-by-side sheets to `remastered/review/`.

Only a wrong size blocks a frame. Other failed checks (alpha, pivot, seams) are printed as `warn` and the sheet gets a red divider: judge those by eye.

A pack whose `<pack>.png` hasn't changed since its last take is skipped, so approved and packed frames keep their state. To redo a pack, overwrite its `<pack>.png` (it is taken again, even if approved) or add `--force`.

Level pieces and the HUD are pasted into one canvas per level or sheet, so pieces from several packs add up.

## 3. Approve and pack

Every command takes a pack name or a regex over entry ids:

```
py jobs.py approve bird_dive              # or: py jobs.py reject bird_dive --note "why"
py jobs.py status bird_dive
py jobs.py pack                           # cuts approved art into working/game/art/
```

Regexes still work for parts of a pack. A digit range is one character, so frames 17–23 are `"^sprite/0485/f00(1[7-9]|2[0-3])$"`, not `f000[17-23]`.

`jobs.py add / run / review` still exist as separate steps (`add` uses the `manual` generator by default).

**Pilot regex** (all pilot entries):

`^(sprite/0765/f000[1-8]|sprite/0560/f00(0[2-9]|1[0-5])|sprite/0560/f0(21[1-7]|24[67])|sprite/0485/f00(09|1[0-9]|2[0-4])|level/TRAINING|ui/STPANEL1)$`

**What the checks test:**
- size: exactly 9× the original;
- alpha: silhouette within 3 % of the original;
- pivot: within 1 original pixel;
- tiling textures still tile.

**Packing** packs a sprite bank only when all its frames are approved (`--allow-partial` overrides this for the pilot, so the game shows a mix of old and new frames).

## 4. See it in the game

`working/game/hercules_display.ini`:
- `renderer = gpu` is already set;
- `art_dir = art` points at `working/game/art/`.

Start `working/game/HERCULES.EXE` and play TRAINING. A frame without a file keeps its original pixels. To go back to the original art, rename the `art` folder.

To judge the sprites in motion before approving, `py motion.py` writes `remastered/review/motion/<sequence>.gif`: original | new art side by side, on the frame pivot, at the runtime display ticks.

For the originals alone, `py refsheet.py` writes `remastered/reference/<sequence>.gif` from the originals. Compare them with your approved frames.
