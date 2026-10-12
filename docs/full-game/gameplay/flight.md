# Flight mode (ENG1, TITANS level 9) — L08.GP.02

VERIFIED-STATIC (Ghidra, ENG1.DLL of this build) unless marked. The on-foot hero is `docs/gameplay-spec.md`; only the flight rules are here. Game state gs = `DAT_10070f24` (EXE 0x615c98).

## Setup

- `HERO_Init` 0x15090 clears the gauge gs+0x2a in level 9 and returns through `HERO_InitFlight` 0x374c0: hero state 0x28 with the Pegasus scripts 0x633e8, attached child kind 0xa007 (`FUN_10027d40`), helper objects 0x707dc / 0x707fc.
- Palette by restart index: 2 → 0x63ef8, 1 → 0x636f0, otherwise 0x63a78 (the same palettes the Titans fade to; `encounters.md` § Titans).
- `LVL_InitTitans` 0x3c170 picks the scroll table 0x65580[restart index] (→ 0x6fff4) and spawns kind 0x36 at the hero. Bank 48 PEGASUS comes from the level list.

## Per tick: `HERO_TickFlightMode` 0x379e0

1. Grace counter 0x6fff0 counts down (no damage while > 0).
2. Steering, `HERO_SteerPegasusByPad` 0x377f0 (the Pegasus object is 0x7217c):

| pad (gs+0x5c) | effect |
|---|---|
| 0x10 up | vy (+0xb8) −2.0 per tick, down to −7.5 |
| 0x40 down | vy +2.0 per tick, up to +7.5 |
| 0x20 right | vx (+0xa8) +2.0 per tick, up to base + 7.5 |
| 0x80 left | vx −2.0 per tick, down to base − 7.5 |
| released | vy decays by 1.0 per tick to 0; vx decays by 1.0 per tick to the base |

   The base is the scroll speed 0x707e8, so letting go keeps pace with the scroll. Position += velocity every tick. While the hero is dying (state 0x23, until anim state 8) the Pegasus drifts x −2.0, y +3.0 per tick instead.

3. Attacks, from state 2 when the attack button is pressed (gs+0x6c & gs+0x5c), by the weapon index 0x6efb4:

| weapon | states | notes |
|---|---|---|
| 0 | 0x48 (anim 2) → 0x4b | single strike; `FUN_100372b0` at the end |
| 1 | 0x49 (anim 3) → 0x4a held (anim 4; `FUN_1001a3e0` once, `FUN_100371a0` each held tick) → 0x4b on release (anim 5, `FUN_1001a410`) | hold-to-charge |
| 2 | 0x4c (anim 6) → 0x4d (anim 7) → 0x4b | spawns effect 0x4005 at hero + (32, 20) |

   Per-weapon repeat timers 0x72140[weapon] reload from 0x55320; gs+0x30 bits 0x20/0x40/0x80/0x100 reset the timers of weapons 1–4 to 5 each tick (their meaning is open).

4. Recoil: state 0x24 → at the end `FUN_10037230`, grace 15 ticks.
5. Death: state 0x23 → at the end of its anim, gs+3 = 0x22 if lives gs+0xc = 0 (game over), else 0x21 (restart at the restart index).

## Damage: `HERO_ApplyDamageInFlight` 0x37e30(level)

- Skipped while 0x72110 ≠ 0, gs+0x30 & 2, the grace counter is running, or the hero is in 0x23/0x24.
- Health gs+0x24 −= table 0x55348[difficulty][level] (index = level directly; level ≥ 8 → 1). Hazards pass level 2 (10/25/30) or 3 (12/30/38).
- Hit: sfx 0x6278 or 0x6279 (random), state 0x24, Pegasus helper scripts 0x63350 / 0x63378.
- Health ≤ 0 and not gs+0x30 & 1 → `HERO_KillInstant` 0x37080 (health 0, state 0x23, lives − 1).
- Callers: hazard kinds 0x32 (level 3), 0x34, 0x5a, 0x52/0x53, 0x3f (level 2), and the contact tests `HERO_CheckPegasusContact` 0x3c4d0 (Pegasus box; called by lava part 0x3d, the harpy helper 0x3b200 and the Spy helper 0x3bcb0) and `HERO_CheckFlightHeroContact` 0x3c510 (hero box; lava pieces 0x56), both level 2.

## Debug keys (not gameplay)

With gs+0x30 & 0x4000, the second pad word gs+0x64 bits 0x400/0x100/0x200 kill the hero; with gs+0x30 & 0x2000, bit 0x800 ends the level (gs+3 = 0x31). gs+0x64 is written by the EXE (ENG1 only clears it at 0x1f361).

## Open

- How the pad maps to the screen at runtime, the weapon index source (0x6efb4) and the 0x55320 timer values: TITANS play session 18.
