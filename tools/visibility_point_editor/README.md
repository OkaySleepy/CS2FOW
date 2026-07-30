# CS2FOW Runtime Visibility Studio

Visibility Studio is a local browser view of CS2FOW's runtime visibility rules. The old fifteen-point editor has been removed; Preview and Play now use the same nineteen Valve capsule bindings, eight padded AABB corners, muzzle point, viewing origins, LOS order, smoke/HE rules, defaults, and fail-open boundary as the plugin.

The Studio shows:

- local CT SAS and Phoenix models with real exported animation clips;
- nineteen animated hitbox capsules driven by the runtime bone bindings;
- eight AABB corners padded 16 units sideways and 4 units upward;
- the held weapon's separate runtime muzzle point;
- direct BVH8 map loading, collision walls, viewing origins, and actual debug rays; and
- a 64 Hz first-person range with movement, navigation, weapons, smoke, HE clearing, sounds, and Runtime LOS/Debug views.

The raw LOS order is reveal-hold reuse, chest probe, eight padded AABB corners, muzzle, then the nineteen-capsule silhouette. The capsule stage uses the same compact front-to-back occluder proofs, target-fitted 32-by-32 depth view, smoke tests, and 75 ms fail-open budget as runtime. The plugin uses Intel MaskedOcclusionCulling while Studio uses a scalar JavaScript rasterizer with matching constants and conservative boundaries, so edge pixels are representative rather than guaranteed bit-identical.

Required model bones or capsule bindings are never replaced with invented points. If capture is incomplete, Studio reports that runtime capture is unavailable and would fail open. Reduced-motion preference freezes a valid real capsule pose.

Studio smoke is geometry-aware test input rendered with locally exported CS2 artwork; it is not Valve's live particle simulation. Studio also does not imitate server-only thread scheduling, lifecycle, stale-snapshot, full-update, quarantine, team, or transmit-list safeguards. Those safeguards can reveal a target but cannot make the browser hide one.

## Play controls

Click **Play**, then click the canvas to capture the mouse. Use `W/A/S/D` to move, `Shift` to walk, `Ctrl` to crouch, and `Space` to jump. Press `1` for the selected primary, `2` for the USP-S, `3` for the Karambit, and `4` for Smoke/HE. Use left/right click for weapon actions, `R` to reload, `F` to inspect, `J` for first/third person, `V` for Runtime LOS/Debug, `C` for debug rays, and `Escape` to release the mouse.

## Export local assets

Install Node.js, Python, and the .NET 10 SDK, then run:

```powershell
npm ci --prefix tools/visibility_point_editor
python tools/visibility_point_editor/export_assets.py --game "C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo"
```

Exported Valve models, maps, navigation, sounds, and particles are written under the ignored `tools/visibility_point_editor/local_assets/` directory and must not be committed.

## Run

```powershell
cd C:\path\to\CS2FOW
npm ci --prefix tools/visibility_point_editor
python -m http.server 8765
```

Open `http://127.0.0.1:8765/tools/visibility_point_editor/viewer.html`. Serving the repository root lets Studio load local bakes from `data/maps`; **Load BVH8** works when no default Mirage bake is present.

Run the alignment and simulation checks with:

```powershell
python tools/visibility_point_editor/check_runtime_alignment.py
node tools/visibility_point_editor/check_bvh8.mjs
node tools/visibility_point_editor/check_fps.mjs
```

The runtime-alignment check compares capsule bindings, AABB padding, probe order, muzzle lengths, viewing origins, defaults, reveal hold, smoke/HE behavior, and fail-open cases against the native source.
