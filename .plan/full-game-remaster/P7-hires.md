# P7 4K asset pipeline + pilot

Goal: a resumable pipeline that turns original assets into 4K replacements without changing size, pivot or timing, and a pilot set the user approves in motion.
Needs: P4, P5, P6.
Outputs: `tools/hires/`, `remastered/`, `docs/hires-pipeline.md`, `docs/art-guide.md`.

## Pipeline

- [ ] **[HUMAN]** Ask the user:
  - the authorization scope (art, music, AI processing, distribution), recorded in `docs/authorization-scope.md` (Q-02)
  - the art route: which AI upscaler/model and licence, hand redraw, or a mix (Q-06)
- [ ] Derive the logical view → 3840x2160 scale, the aspect policy (Q-04) and per-family texture budgets.
- [ ] Write a replacement-manifest schema and `tools/hires/contract.py`. Each entry holds source identity, geometry, timing, provenance and output.
- [ ] Write `tools/hires/jobs.py`, a resumable job state machine:
  - states: pending → generated → review → approved → packed
  - identity = hash(source + settings)
  - bounded retries
- [ ] Write `tools/hires/checks.py`, which checks dimensions, alpha, pivot × scale, frame count and tile seams.
- [ ] Write `tools/hires/atlas.py`, which packs approved assets with padding/extrusion and reports texture memory.
- [ ] Add the generator adapter for the approved route. If it needs a tool, pin its version and SHA-256 with the user's OK.

## Pilot

- [ ] Choose the pilot set: hero idle/run/attack, one enemy, one background section, one repeating tile, one UI element.
- [ ] Make reference sheets per sequence and the art-guide pilot section.
- [ ] Generate the pilot through `jobs.py` until the checks pass. Render side-by-side GIFs at game speed and run the slice at 3840x2160 with the pilot assets.
- [ ] **[HUMAN]** The user approves the pilot in motion or lists corrections.

Done when the pilot is approved. Bulk art starts only after that.

## Notes
