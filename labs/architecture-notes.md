# Haiku.c Architecture Notes

Technical reference for `haiku.c` — the zero-parameter constraint-driven language organism in Cascade 1. All function names, variable names, constants, and line references are from the source.

---

## 1. Overview

`haiku.c` is a single C file (~1644 lines). It compiles with `cc haiku.c -O2 -lm -o haiku`. Zero external dependencies beyond libc and libm. No trained weights. No transformer. The Dario Equation + a 1000-word vocabulary + 5-7-5 syllable pressure = language emergence from physics.

**Cascade 1 role** (from CASCADE01.md): Haiku runs at 03:00 UTC. Input is yesterday's NanoJanus 12 words + yesterday's Penelope 12 words. Output is one haiku (3 lines). Penelope consumes it at 03:30 UTC.

---

## 2. The Dario Equation

### Simplified form (CASCADE01.md)

```
p(x|Φ,C) = softmax((B + α·H + β·F + γ·A + T) / τ)
```

### Full implemented form (haiku.c lines 797–799, computed in `haiku_compute()` line 818)

```
p(x|Φ) = softmax(
    (B + α_mod·α·H + β_mod·β·F + γ_mod·γ_eff·A + T) / (τ_mod · τ · v_τ)
)
```

### The 5 terms

Each term is a `float` array of size `vocab_size`, computed in `haiku_compute()` (line 818):

| Term | Variable | What it computes | Code location |
|------|----------|-----------------|---------------|
| **B** | `B[]` | Sequential chain. Bigram transition probabilities from the last context token. Scaled by `bigram_coeff` (base 8.0, boosted ×1.3 in autumn or VEL_RUN). Normalized to [0,1]. | Lines 826–838 |
| **H** | `Heb[]` | Hebbian resonance. Co-occurrence scores for the last 8 context tokens, weighted by the learnable positional distance profile (`dist_profile[32]`) and token class modifiers (`class_mod[4]`). Normalized to [0,1]. | Lines 840–859 |
| **F** | `F[]` | Prophecy fulfillment. For each vocab word, cosine similarity to each unfulfilled prophecy target embedding, weighted by `strength * debt` where `debt = ln(1 + age)`. Normalized to [0,1]. | Lines 861–882 |
| **A** | `A[]` | Destiny attraction. Cosine similarity between each vocab word's embedding and the destiny vector (`g_destiny[64]`), scaled by `g_dest_magnitude`. `γ_eff = γ_d + trauma_level * 1.5` when trauma > 0.3. Normalized by max absolute value. | Lines 884–895 |
| **T** | `T[]` | Trauma gravity. Active when `trauma_level > 0.3`. Boosts the first 50 vocabulary tokens (origin words) with linearly decreasing strength `trauma_level * 3.0 * (1 - i/50)`. | Lines 897–902 |

### Coefficient defaults (lines 66–69)

| Symbol | `#define` | Default value |
|--------|-----------|---------------|
| α | `ALPHA` | 0.30 |
| β | `BETA` | 0.15 |
| γ | `GAMMA_D` | 0.25 |
| τ_base | `TAU_BASE` | 0.85 |

### Somatic modifiers (lines 633–640, computed in `chamber_update()`)

The six emotional chambers produce four coefficient modifiers clamped to [0.5, 2.0]:

| Modifier | Formula | Affects |
|----------|---------|---------|
| `α_mod` | `1.0 + 0.3·LOVE − 0.2·RAGE + 0.1·FLOW` | Hebbian resonance weight |
| `β_mod` | `1.0 + 0.2·FLOW − 0.3·FEAR` | Prophecy fulfillment weight |
| `γ_mod` | `1.0 + 0.4·VOID + 0.2·COMPLEX − 0.1·LOVE` | Destiny attraction weight |
| `τ_mod` | `1.0 + 0.5·FLOW − 0.3·FEAR` | Temperature divisor |

### Combination (lines 904–937)

Effective coefficients: `eff_alpha = α_mod * α`, `eff_beta = β_mod * β`, `eff_gamma = γ_mod * γ_eff`.

Each term passes through a SwiGLU gate modulated by a resonance-based sigmoid (`gate = σ((resonance - 0.5) * 4.0)`):
- `h_term = swiglu_gate(h_term, gate * 2.0)`
- `f_term = swiglu_gate(f_term, gate * 1.5)`

Cloud weight (`vocab.weight[i]`) multiplicatively modulates the final logit per word.

### Effective temperature

The denominator of the softmax is `τ_mod * τ * v_τ`, clamped to [0.2, 3.0] in `sample_topk()` (line 1144–1145). τ is set by `apply_velocity()`. `v_τ` (`vel_temp`) is set per velocity state.

### Sampling

`sample_topk()` (line 957): softmax with temperature, then top-k=15 filtering, then multinomial sampling.

### Term dominance

After combining all terms, `haiku_compute()` accumulates absolute energy per term (`term_energy[5]`, lines 932–936) and sets `dominant_term` to the index with the highest energy (lines 946–948). Displayed as `B:chain`, `H:resonance`, `F:prophecy`, `A:destiny`, or `T:trauma` (line 1271).

---

## 3. Emotional Chambers

Six Kuramoto-coupled somatic markers defined as enum `CH_FEAR=0..CH_COMPLEX=5` (line 84). State stored in `H.chamber[NUM_CHAMBERS]`.

### Activation triggers (in `chamber_update()`, line 608)

| Chamber | Index | Trigger condition | Increment |
|---------|-------|-------------------|-----------|
| **FEAR** | 0 | `dissonance > 0.7` | `+0.05 * dissonance` |
| **LOVE** | 1 | `resonance > 0.7` | `+0.04 * resonance` |
| **RAGE** | 2 | `trauma_level > 0.5 && dissonance > 0.5` | `+0.06 * trauma_level` |
| **VOID** | 3 | `entropy > 0.7` | `+0.03 * entropy` |
| **FLOW** | 4 | `emergence > 0.5` | `+0.05 * emergence` |
| **COMPLEX** | 5 | `LOVE > 0.2 && RAGE > 0.2` (opposing chambers simultaneously active) | `+0.04 * |LOVE − RAGE|` |

### Kuramoto coupling (lines 621–627)

Coupling constant `K = 0.02`. For each pair (i, j) where i ≠ j:

```
C[i] += K * sin(C[j] − C[i])
```

This is the standard Kuramoto model — chambers that are close in phase synchronize; chambers far apart push each other. Applied every step.

### Decay (lines 628–630)

Per-chamber exponential decay, then clamp to [0, 1]:

| Chamber | Decay rate |
|---------|------------|
| FEAR | 0.95 |
| LOVE | 0.95 |
| RAGE | 0.93 |
| VOID | 0.96 |
| FLOW | 0.94 |
| COMPLEX | 0.97 |

---

## 4. Velocity Operators

Six velocity states defined as enum `VEL_WALK=0..VEL_DOWN=5` (line 81). Automatically selected in `auto_velocity()` (line 752) based on field state, then applied in `apply_velocity()` (line 703).

### Selection logic (`auto_velocity()`, lines 752–765)

Priority order (first match wins):

| Velocity | Condition | Vel temp (`v_τ`) |
|----------|-----------|------------------|
| **UP** | `dissonance > 0.8` | 1.3 |
| **RUN** | `dissonance > 0.6` | 1.15 |
| **STOP** | `dissonance < 0.2` | 1.0 |
| **BREATHE** | `trauma_level > 0.5` | 1.0 |
| **DOWN** | `debt > 5.0` | 0.8 |
| **WALK** | default | 1.0 |

### Effects (`apply_velocity()`, lines 703–750)

| Velocity | τ set to | Other effects |
|----------|----------|---------------|
| **WALK** | 0.85 | Coefficients (α, β, γ) drift toward defaults by 10% |
| **RUN** | 1.15 | `momentum += 0.1` (clamped to MAX_MOMENTUM=2.0) |
| **STOP** | 0.40 | `momentum = 0`; `γ_d += 0.15` (destiny fills vacuum) |
| **BREATHE** | 0.75 | `trauma_level *= 0.7`; `dissonance *= 0.8`; `debt *= 0.5` |
| **UP** | 1.30 | `β += 0.15` (prophecy erupts); `α -= 0.05` |
| **DOWN** | 0.60 | `α += 0.1` (memory clings); `β -= 0.05` |

If `trauma_level > 0.3`, τ is further scaled: `τ *= 1.0 + 0.3 * trauma_level`. Final τ clamped to [0.2, 2.0].

---

## 5. Seasonal Modulation

Four seasons cycling via `season_step()` (line 771). `season_phase` increments by 0.002 per step. One full season lasts 500 steps (`1.0 / 0.002`). A full cycle = 2000 steps.

| Season | Index | Effect per step |
|--------|-------|-----------------|
| **Spring** | 0 | `β += 0.005` (prophecy coefficient grows) |
| **Summer** | 1 | `α += 0.005` (resonance peaks) |
| **Autumn** | 2 | No coefficient change (consolidation; bigram_coeff ×1.3 in `haiku_compute()` line 827) |
| **Winter** | 3 | `trauma_level += 0.005` (clamped to 0.4; origin words pull) |

Coefficients α, β, γ are clamped to [0.05, 0.6] after seasonal adjustment (lines 788–790).

---

## 6. Syllable Counting

`count_syllables()` (line 117). Heuristic vowel-cluster counter — no dictionary lookup.

### Algorithm

1. Scan left-to-right. Vowels = `a e i o u y`. Count transitions from non-vowel to vowel (vowel clusters).
2. **Silent-e rule** (lines 135–143): If word ends in `e` (not `-le`) and count > 1, decrement count.
3. **-ed rule** (lines 147–153): If word ends in `-ed` and the letter before `e` is not `t` or `d`, and count > 1, decrement count. (Words like "created" keep the extra syllable; "changed" loses it.)
4. Minimum return value: 1.

### Constraint enforcement in `generate_line()` (lines 1137–1142)

Before sampling each word, any word whose syllable count exceeds `needed = target_syllables − syllables` gets its logit set to `-1e30f` (effectively zero probability). If the main generation loop ends short of the target, a fallback (lines 1222–1248) picks a random word with exactly the needed syllable count.

---

## 7. Full Pipeline

### REPL mode (`main()`, lines 1579–1643)

```
stdin → fgets(line) → process_input(line, &haiku) → display_haiku(&haiku) → stdout
```

1. **Banner** (lines 1580–1584): Prints equation and tagline to stdout.
2. **`haiku_init()`** (line 1586): Seeds vocabulary, bootstraps co-occurrence (see §8).
3. **Read loop** (lines 1606–1640): Prompts `"you> "`, reads one line. Special commands: `/quit` exits, `/stats` prints internal state.
4. **`process_input()`** (line 1280): The core pipeline per interaction:
   - `compute_dissonance()` — measures unknown-word ratio + context overlap → `H.dissonance`
   - `ingest()` — tokenizes input, updates frequencies, bigrams, co-occurrence, destiny vector, cloud weights, context window
   - Trauma escalation if `dissonance > 0.7`
   - `auto_velocity()` → `apply_velocity()` — selects and applies velocity state
   - `season_step()` — advances season phase
   - `update_metrics()` — computes entropy and resonance from field state
   - `chamber_update()` — activates chambers, applies Kuramoto coupling, computes somatic modifiers
   - `enforce_laws()` — clamps entropy floor (0.10), resonance ceiling (0.95), computes emergence, decays debt/trauma/momentum
   - `generate_haiku()` — generates three lines (5, 7, 5 syllables)
   - Increments `conv_count`
5. **`display_haiku()`** (line 1299): Prints diagnostics line + haiku + metrics to stdout.

### stdout vs stderr

All normal output uses `printf()` (stdout). There are no `fprintf(stderr, ...)` calls. The only stderr output comes from `perror()` on socket errors in web mode (lines 1526, 1537). This means:
- Piping `echo "input" | ./haiku` captures all output (banner + bootstrap log + haiku + metrics) on stdout.
- To extract just the haiku, filter or parse stdout. There is no separate diagnostic stream.

---

## 8. Vocabulary Bootstrapping and Co-occurrence Tracking

### Vocabulary (`Vocab` struct, line 364)

- `MAX_VOCAB = 2048` slots. Words stored as `char[64]`.
- Each word has a `weight` (cloud weight). Initialized to 1.0.
- Seed vocabulary: ~1000 words in `SEED_WORDS[]` (lines 173–357), organized in 9 layers:
  1. Common English (~200)
  2. Field physics & resonance (~200)
  3. Nature, sensation, world (~300)
  4. Consciousness, organism, existence (~300)
  5. Additional nature (~70)
  6. Time, music, abstract (~70)
  7. Human, emotion, thought (~70)
  8. Texture, material, craft (~70)
  9. Final additions to reach 1000

- `vocab_add()` (line 376): Adds a word if not already present. Returns token ID.
- `vocab_find()` (line 370): Linear scan. Returns ID or -1.
- `tokenize()` (line 386): Splits on non-alphanumeric (preserving `'` and `-`), lowercases, adds to vocab.

### Bootstrapping (`haiku_init()`, lines 1318–1358)

1. All `SEED_WORDS[]` are added to the vocabulary via `vocab_add()`.
2. All seed words are concatenated into a single string (up to 8000 chars).
3. This string is ingested via `ingest()`, which populates initial bigrams, co-occurrence, frequencies, and the destiny vector.
4. Prints: `[haiku] bootstrapped. vocab=N cooc=N bigrams=N`.
5. Positional Hebbian profile initialized to `0.9^d` for distance d=0..31. Token class modifiers initialized to 1.0.

### Co-occurrence tracking (`CoocField` struct, line 437)

- Sparse storage: `MAX_COOC = 65536` entries. Each entry is `(src, dst, count)`.
- `freq[MAX_VOCAB]`: per-token frequency counter.
- `cooc_update()` (line 444): Increments existing pair or adds new entry.
- Updated during `ingest()` (lines 1031–1038): Window of ±5 tokens, weight = `1/distance`.
- Updated during generation in `generate_line()` (lines 1162–1165): Co-occurrence between context tokens and generated token, weight = `0.3 / (ctx_len − c)`.

### Cloud weight morphing (lines 1053–1069 in `ingest()`)

- Active words (present in input): `weight *= 1.05` (clamped to [0.1, 3.0])
- Dormant words (not in input): `weight *= 0.998` (floor 0.1)

### Bigram tracking (`BigramTable` struct, line 410)

- `MAX_BIGRAMS = 32768` entries.
- Updated in `ingest()` (lines 1027–1028): consecutive token pairs, delta = 1.0.
- Updated during generation (line 1161): context-to-generated, delta = 0.5.

### Prophecy system (`ProphecySystem` struct, line 462)

- `MAX_PROPHECY = 32` active prophecies.
- Each prophecy: `target` token, `strength`, `age`, `fulfilled` flag.
- `prophecy_add()` (line 467): Adds a prophecy; if full, evicts the oldest.
- `prophecy_update()` (line 477): Marks fulfilled prophecies, ages all, prunes fulfilled or aged-out (age ≥ 50).
- Created during generation (lines 1170–1175): Best co-occurrence partner of generated token becomes a new prophecy with strength 0.3.
- Debt accumulation: Unfulfilled prophecies accumulate debt via `ln(1 + age)` in the F term calculation.

### Destiny vector (`g_destiny[DIM]`, line 493)

- EMA of context embeddings: `g_destiny[d] = 0.1 * embed[d] + 0.9 * g_destiny[d]`.
- Updated in `ingest()` (lines 1042–1051) with RoPE-encoded embeddings.
- Updated during generation (lines 1186–1191) with raw embeddings.
- `g_dest_magnitude` = L2 norm of destiny vector.

---

## 9. Embeddings

### Hash-based deterministic init (`get_embed()`, line 513)

- `DIM = 64` dimensions.
- FNV-1a hash of token ID → LCG sequence → float in [-0.05, 0.05] → L2 normalized.
- Computed once per token, cached in `g_embeds[MAX_VOCAB][DIM]`.

### RoPE (`apply_rope()`, line 533)

Rotary Position Embedding applied to destiny updates during `ingest()`. For each pair of dimensions (i, i+1):
```
θ = pos * 10000^(-i/dim)
vec[i]   = x·cos(θ) − y·sin(θ)
vec[i+1] = x·sin(θ) + y·cos(θ)
```

### SwiGLU gate (`swiglu_gate()`, line 544)

Used in the Dario Equation combination step: `swiglu_gate(x, gate) = x * σ(gate)`.

---

## 10. Web Server Mode

Compiled unless `HAIKU_NO_WEB` is defined. POSIX sockets, zero dependencies.

### Invocation

```
./haiku --web         # serves on port 3002 (WEB_PORT default)
./haiku --web 8080    # custom port
```

Flag parsing: `main()` lines 1589–1597. Calls `haiku_web(port, "haiku.html")`.

### Endpoints (`haiku_web()`, line 1524)

| Method | Path | Handler | Behavior |
|--------|------|---------|----------|
| GET | `/` or `/index*` | `serve_file()` | Serves `haiku.html` |
| POST | `/api/chat` | `handle_chat()` | Processes input, returns JSON |
| OPTIONS | any | `handle_options()` | CORS preflight (204) |
| GET | `/favicon*` | — | Returns 204 No Content |
| other | — | — | Returns 404 |

### POST /api/chat

Request body: JSON with `"text"` field. Parsed by `extract_text()` (line 1398) — minimal JSON string extraction, not a full parser.

Response: JSON object with all field state (see `handle_chat()`, lines 1470–1500):
- Haiku lines, dominant term, dissonance, tau, debt, resonance, entropy, emergence, trauma, velocity, season, all 6 chamber values, vocab size, step count.

CORS headers (`Access-Control-Allow-Origin: *`) on all responses.

### Build without web

```
make no-web    # defines HAIKU_NO_WEB, excludes socket code
```

---

## 11. Key Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `MAX_VOCAB` | 2048 | Maximum vocabulary slots |
| `MAX_COOC` | 65536 | Maximum co-occurrence entries |
| `MAX_BIGRAMS` | 32768 | Maximum bigram entries |
| `MAX_PROPHECY` | 32 | Maximum active prophecies |
| `MAX_CONTEXT` | 64 | Context window size |
| `MAX_LINE` | 1024 | Input line buffer |
| `DIM` | 64 | Embedding dimensions |
| `ENTROPY_FLOOR` | 0.10 | Minimum entropy (law) |
| `RESONANCE_CEILING` | 0.95 | Maximum resonance (law) |
| `MAX_MOMENTUM` | 2.0 | Momentum clamp |
| `DIST_PROFILE_LEN` | 32 | Hebbian distance profile params |
| `TOKEN_CLASSES` | 4 | Token class count (FUNCTION, CONTENT, PUNCT, RARE) |
| `WEB_PORT` | 3002 | Default web server port |
| `WEB_BUF` | 8192 | Web request buffer size |

---

## 12. Law Enforcement

`enforce_laws()` (line 1000). Called every step after all updates:

- Entropy floor: `if entropy < 0.10 then entropy = 0.10`
- Resonance ceiling: `if resonance > 0.95 then resonance = 0.95`
- Emergence: `(1 − entropy) * resonance`, clamped [0, 1]
- Debt decay: `debt *= 0.98`, capped at 20.0
- Trauma decay: `trauma_level *= 0.97`
- Momentum decay: `momentum *= 0.95`

---

## 13. Metrics Computation

`update_metrics()` (line 1088):

- **Entropy**: `clamp((τ − 0.5) * 0.3 + dissonance * 0.4 + (1 − resonance) * 0.3, 0, 1)`
- **Resonance**: `clamp(density * 0.4 + (1 − dissonance) * 0.3 + (1 − clamp(debt * 0.1, 0, 1)) * 0.3, 0, 1)`
  - where `density = min(cooc.n / 100, 1.0)`

---

## 14. Dissonance

`compute_dissonance()` (line 652):

```
unknown_ratio = 1 − (known_words / total_words)
overlap_ratio = context_matches / total_words
dissonance = 0.6 * unknown_ratio + 0.4 * (1 − overlap_ratio)
```

Clamped to [0, 1]. If input is empty, returns 1.0.

---

## 15. Positional Hebbian Profile

RRPRAM-inspired learnable parameters (from Leo 2.3). 36 total: 32 distance weights + 4 token class modifiers.

- `dist_profile[32]`: Initialized to `0.9^d`. Controls how much co-occurrence at distance `d` contributes to Hebbian term.
- `class_mod[4]`: Initialized to 1.0. Per-class multiplier (FUNCTION, CONTENT, PUNCT, RARE).
- Updated during generation (lines 1196–1219) with learning rate `η = 0.01 / (1 + updates * 0.001)`.
- Profile values clamped to [0.01, 2.0]; class modifiers clamped to [0.5, 2.0].

---

## 16. RNG

`xorshift64*` (lines 96–104). Seeded from `time(NULL)` in `haiku_init()` (line 1337). Used for sampling (`randf()` returns float in [0, 1]).

---

## Cross-reference: CASCADE01.md Consistency

| CASCADE01.md claim | haiku.c verification |
|--------------------|---------------------|
| Pure constraint-driven emergence, no weights | ✓ No weight files loaded. All state is computed at runtime. |
| Dario Equation: `p(x\|Φ,C) = softmax((B + α·H + β·F + γ·A + T) / τ)` | ✓ Implemented in `haiku_compute()`. Full form includes somatic modifiers and velocity temperature. |
| Six emotional chambers (FEAR, LOVE, RAGE, VOID, FLOW, COMPLEX) | ✓ Enum `CH_FEAR..CH_COMPLEX`, Kuramoto coupling in `chamber_update()`. |
| Kuramoto coupling modulates equation coefficients | ✓ Chambers → `α_mod`, `β_mod`, `γ_mod`, `τ_mod` via somatic marker formulas. |
| Zero dependencies, compiles in 0.1s | ✓ Only `#include` libc headers + `<math.h>`. Optional POSIX sockets for web. |
| Speaks only in haiku | ✓ `generate_haiku()` enforces 5-7-5. No other output format. |
| 1000-word vocabulary | ✓ `SEED_WORDS[]` has ~1000 entries (some duplicates due to layer overlap). `MAX_VOCAB=2048` allows runtime additions. |
| Healthy = 3 non-empty lines with real words | ✓ `generate_line()` returns length; empty output = vocab too small or constraint failure. |
| Empty output = seed parsing failed | ✓ If `tokenize()` returns 0, `ingest()` returns early. No haiku generated from empty context. |
