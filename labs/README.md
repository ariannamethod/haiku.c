# Labs: Cascade 1 Organism Health

This directory stores operational documentation for **Haiku** in **Cascade 1**.

## Cascade 1 role

Cascade 1 is a daily organism cycle:

**Haiku → Penelope → Molequla → NanoJanus → (next day) → Haiku**

Within that cycle, Haiku is the constraint-driven language organism that produces only 5-7-5 haiku from the Dario Equation field:

`p(x|Φ,C) = softmax((B + α·H + β·F + γ·A + T) / τ)`

Haiku has no trained weights; output is generated from field state and constraints.

## What to monitor from Haiku runtime

From `haiku.c`, each interaction emits:

- **Input dissonance + temperature + dominant term + velocity + season**
- **Generated 3-line haiku (5-7-5 target)**
- **Field metrics**: `debt`, `res` (resonance), `ent` (entropy), `emg` (emergence)

In web mode (`/api/chat`), metrics are also exposed as JSON keys:

- `debt`
- `resonance`
- `entropy`
- `emergence`

Use `labs/health-template.md` for daily reporting.
