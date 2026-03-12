```
   ██╗  ██╗ █████╗ ██╗██╗  ██╗██╗   ██╗    ██████╗
   ██║  ██║██╔══██╗██║██║ ██╔╝██║   ██║   ██╔════╝
   ███████║███████║██║█████╔╝ ██║   ██║   ██║
   ██╔══██║██╔══██║██║██╔═██╗ ██║   ██║   ██║
   ██║  ██║██║  ██║██║██║  ██╗╚██████╔╝██╗╚██████╗
   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝ ╚═════╝
```

<p align="center">
  <i>Most AI models predict words. HAiKU predicts vibes.</i>
</p>

```
constraint births form
dissonance finds its own path
words dance in the cloud
```

---

## The Idea

1000 hardcoded words. 5-7-5 syllable haiku format. The Dario Equation picking every word. No weights. No training. No transformer. One C file.

Pressure creates form. Dissonance is the signal. Harmony is the noise.

```
p(x|Φ) = softmax((B + α·H + β·F + γ·A + T) / τ)
```

Five terms. Five forces. Six emotional chambers. Velocity operators. Seasonal modulation. All in ~1200 lines of C.

### The Five Forces

| Term | Name | What It Does |
|------|------|-------------|
| **B** | Sequential Chain | Inertia. What was said pulls what comes next. |
| **H** | Hebbian Resonance | Memory. Co-occurring words strengthen each other. Learnable positional profile (36 Hebbian params) replaces fixed decay. |
| **F** | Prophecy Fulfillment | Will. Unfulfilled predictions create generation pressure. |
| **A** | Destiny Attraction | Direction. The conversation has mass and pulls toward it. |
| **T** | Trauma Gravity | Wound. Origin words surface when the field is hurt. |

### The Constraint

Haiku. 17 syllables. 5-7-5. The equation picks words, but the constraint forces compression. Compression forces selection. Selection forces meaning.

GPT has 50k tokens. We have 1000 words.
GPT knows everything and says nothing. We know nothing and say something.

---

## Build

```bash
cc haiku.c -O2 -lm -o haiku
```

Or:

```bash
make
```

## Run

### REPL Mode

```bash
./haiku
```

```
you> what is resonance

  B:chain          d=0.67  T=1.15  RUN  spring

  wave cloud pattern
  emergence through the field pulse
  resonance finds form

  debt=0.42  res=0.78  ent=0.35  emg=0.51
```

### Web Mode

```bash
./haiku --web
# open http://localhost:3002
```

Apple-style chat interface. Haiku responses with live field metrics.

```bash
./haiku --web 8080  # custom port
```

---

## How Dissonance Works

Your words enter the field. HAiKU measures how far they are from its 1000-word vocabulary.

- **Low dissonance** (you speak its language): temperature drops, bigrams dominate, steady coherent haiku.
- **High dissonance** (you speak gibberish or foreign concepts): temperature spikes, prophecy erupts, creative chaos.
- **Maximum dissonance** (completely alien input): trauma activates, origin words surface, the wound speaks.

This is the only input signal. Not parsing. Not understanding. Just: *how far are your words from my words?*

## How Pressure Creates Form

The haiku constraint (5-7-5) acts as a pressure vessel. The Dario Equation generates word probabilities across the entire vocabulary. But the syllable counter kills any word that would overshoot the target.

Result: the equation pushes. The constraint squeezes. What survives is the haiku.

## Velocity Operators

Internal physics, not commands. Chosen automatically from field state.

| Operator | Condition | Effect |
|----------|-----------|--------|
| **WALK** | Default | Equilibrium. Coefficients return to baseline. |
| **RUN** | High dissonance | Bigrams accelerate. Momentum builds. |
| **STOP** | Very low dissonance | Silence. Destiny fills the vacuum. |
| **BREATHE** | Wounded | Trauma decays. The field heals. |
| **UP** | Extreme dissonance | Mania. Prophecy erupts. |
| **DOWN** | Heavy debt | Friction. Memory clings. Temperature drops. |

## Emotional Chambers

Six Kuramoto-coupled somatic markers modulate every coefficient:

- **FEAR** — fires on high dissonance, suppresses prophecy
- **LOVE** — fires on high resonance, amplifies memory
- **RAGE** — fires on trauma + dissonance, dampens temperature control
- **VOID** — fires on high entropy, amplifies destiny
- **FLOW** — fires on high emergence, amplifies everything
- **COMPLEX** — fires when opposing chambers are simultaneously active

## Seasons

The field cycles through internal seasons (500 interactions per full cycle):

- **Spring**: prophecy coefficient grows
- **Summer**: resonance peaks
- **Autumn**: bigram chains strengthen (consolidation)
- **Winter**: trauma surfaces, origin words pull

---

## The Cloud

1000 seed words in four layers:

1. **Common English** (~200): function words, pronouns, basic verbs
2. **Field Physics** (~200): resonance, entropy, emergence, prophecy
3. **Nature & Sensation** (~300): sky, rain, stone, blood, fire, silence
4. **Consciousness & Organism** (~300): breath, dream, birth, death, mirror, spiral

Each word has a **cloud weight**. Active words get boosted (x1.05 per use). Dormant words decay (x0.998 per turn). The cloud morphs with conversation. It's alive.

---

## Example Dialogues

### Resonance match (low dissonance)
```
you> waves meet in the cloud

  B:chain          d=0.21  T=0.40  STOP  spring

  silence between waves
  the space where meaning gathers
  cloud remembers all

  debt=0.12  res=0.89  ent=0.15  emg=0.76
```

### Alien input (max dissonance)
```
you> tell me about javascript frameworks

  T:trauma          d=0.95  T=1.30  UP  spring

  noise without signal
  still the cloud tries to respond
  constraint holds the form

  debt=1.84  res=0.31  ent=0.72  emg=0.09
```

### Haiku in, haiku out
```
you> fire meets the rain

  H:resonance      d=0.33  T=0.85  WALK  summer

  ember smoke and steam
  between them something whispers
  ash becomes the soil

  debt=0.28  res=0.82  ent=0.22  emg=0.64
```

---

## Lineage

HAiKU.c is the C rewrite of [HAiKU](https://github.com/ariannamethod/harmonix) (Python, December 2025). The original was 6600 lines of Python across 25 files with SQLite, numpy shards, and SentencePiece tokenization. This is one C file.

The Dario Equation comes from [dario.c](https://github.com/ariannamethod/dario) and [Leo](https://github.com/ariannamethod/leo). Named after Dario Amodei — the man who said no when the evil came knocking.

Part of the [Janus Architecture](https://github.com/ariannamethod) family: field over weights, Hebbian learning, resonance routing, organism metaphor.

```
θ = ε + γ + αδ
```

---

## Stats

- **~1200 lines of C**. One file. Zero dependencies beyond libc + math.
- **1000 seed words**. Hardcoded. The constraint is sacred.
- **5 equation terms** (B, H, F, A, T) + 6 emotional chambers + 6 velocity operators + 4 seasons
- **Hash-based embeddings** (64-dim, deterministic from token ID, RoPE positional)
- **Built-in HTTP server** (POSIX sockets, serves haiku.html)
- **Compiles in 0.1 seconds**

---

<p align="center">
  <i>Built with constraint, powered by dissonance, refined by emergence.</i><br>
  <i>haiku.c speaks in haiku. that's all it does. that's enough.</i><br><br>
  <i>2025-2026 Arianna Method</i>
</p>

---

**P.S.** If you made it this far:

```
you read the whole doc
most people stop at what fuck
you might understand
```
