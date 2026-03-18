# Haiku Daily Health Report (Template)

## Date
- YYYY-MM-DD (UTC):

## Input seed (what Haiku received)
- Raw input text:
- Input context/source (if applicable):

## Output (haiku generated)
- Line 1:
- Line 2:
- Line 3:

## Syllable validation
> Count syllables per line using the same heuristic as `count_syllables()` in haiku.c (vowel-cluster based, silent-e collapse, -ed rule). Approximate — the organism's syllable counter is heuristic, not dictionary-based.

- Line 1 syllable count (target 5):
- Line 2 syllable count (target 7):
- Line 3 syllable count (target 5):
- Constraint met (yes/no):

## Metrics
> Fill from raw CLI or web JSON output when available.

- debt:
- resonance (or `res`):
- entropy (or `ent`):
- emergence (or `emg`):
- trauma:
- momentum:

## Runtime signals
- dominant term (`B:chain` / `H:resonance` / `F:prophecy` / `A:destiny` / `T:trauma`):
- dissonance (`d`):
- temperature (`tau`):
- velocity (`WALK` / `RUN` / `STOP` / `BREATHE` / `UP` / `DOWN`):
- season (`spring` / `summer` / `autumn` / `winter`):
- season phase (0.0–1.0):

## Emotional chambers
> Available from web JSON (`chambers` object) or `/stats` REPL command.

- FEAR:
- LOVE:
- RAGE:
- VOID:
- FLOW:
- COMPLEX:

## Internal state (from `/stats` or web JSON)
- vocab size:
- co-occurrence entries:
- bigram entries:
- step count:
- conversation count:

## Status
- [ ] HEALTHY
- [ ] DEGRADED
- [ ] FAILED

## Notes on anomalies
- Constraint anomalies (non 5-7-5 output, malformed lines, empty lines):
- Metric anomalies (spikes/collapse in debt/resonance/entropy/emergence):
- Runtime anomalies (unexpected velocity/season/term behavior):
- Chamber anomalies (stuck chambers, COMPLEX not firing when expected):
- Cascade anomalies (empty seed from upstream, broken pipe to downstream):
- Other observations:
