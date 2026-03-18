# Labs: Cascade 1 Organism Health

This directory stores operational documentation for **Haiku** in **Cascade 1**.

## Cascade 1 role

Cascade 1 is a daily organism cycle:

**Haiku → Penelope → Molequla → NanoJanus → (next day) → Haiku**

Within that cycle, Haiku is the constraint-driven language organism that produces only 5-7-5 haiku from the Dario Equation field:

```
p(x|Φ) = softmax((B + α_mod·α·H + β_mod·β·F + γ_mod·γ·A + T) / (τ_mod·τ·v_τ))
```

Haiku has no trained weights; output is generated from field state and constraints. The somatic modifier terms (`α_mod`, `β_mod`, `γ_mod`, `τ_mod`) are driven by the six emotional chambers, and `v_τ` is the velocity temperature multiplier. See `labs/architecture-notes.md` for full details.

## What to monitor from Haiku runtime

From `haiku.c`, each REPL interaction prints to **stdout** (all output uses `printf()`):

- **Diagnostics line**: dominant term, dissonance (`d`), temperature (`T`), velocity, season
- **Generated 3-line haiku** (5-7-5 target)
- **Field metrics**: `debt`, `res` (resonance), `ent` (entropy), `emg` (emergence)

The `/stats` command also prints: vocab size, co-occurrence count, bigram count, step, conversation count, debt, trauma, momentum, coefficients (α, β, γ, τ), all six chamber values, velocity, and season phase.

In web mode (`--web`, endpoint `POST /api/chat`), the full field state is returned as JSON:

- `line1`, `line2`, `line3` — haiku text
- `dominant`, `dominant_name` — dominant term index and name (e.g. `"B:chain"`)
- `dissonance`, `tau` — input dissonance and temperature
- `debt`, `resonance`, `entropy`, `emergence` — field metrics
- `trauma` — trauma level
- `velocity`, `season` — current velocity operator and season names
- `chambers` — object with `fear`, `love`, `rage`, `void`, `flow`, `complex`
- `vocab`, `step` — vocabulary size and generation step count

Use `labs/health-template.md` for daily reporting. See `labs/architecture-notes.md` for source-level documentation.
