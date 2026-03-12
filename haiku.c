/*
 * haiku.c — Constraint-Driven Language Emergence
 *
 * p(x|Phi) = softmax(
 *     (B + a_mod*a*H + b_mod*b*F + g_mod*g*A + T)
 *     / (t_mod*tau*v_tau)
 * )
 *
 * 1000 words. 5-7-5 syllables. The Dario Equation.
 * Pressure creates form. Dissonance is the signal.
 *
 * Most AI models predict words. HAiKU predicts vibes.
 *
 * Five terms. Five forces. Six emotional chambers.
 *   B -- Sequential Chain (inertia, what was)
 *   H -- Hebbian Resonance (memory, what echoed)
 *   F -- Prophecy Fulfillment (will, what wants to be said)
 *   A -- Destiny Attraction (direction, where the field pulls)
 *   T -- Trauma Gravity (wound, where it came from)
 *
 * Somatic modulation (6 emotional chambers -> coefficient modifiers):
 *   a_mod = f(LOVE, RAGE, FLOW)
 *   b_mod = f(FLOW, FEAR)
 *   g_mod = f(VOID, COMPLEX, LOVE)
 *   t_mod = f(FLOW, FEAR)
 *
 * Velocity operators:
 *   WALK    -- equilibrium, steady breath
 *   RUN     -- tachycardia, bigrams accelerate
 *   STOP    -- silence, destiny fills the vacuum
 *   BREATHE -- Schumann healing, return to natural frequency
 *   UP      -- mania, prophecy erupts
 *   DOWN    -- friction, memory clings
 *
 * Zero weights. Zero dependencies (libc + math). Compiles in 0.1s.
 * Speaks only in haiku. Users hate it or love it.
 * There is no middle ground.
 *
 * cc haiku.c -O2 -lm -o haiku && ./haiku
 *
 * by Arianna Method
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>

/* ===================================================================
 * CONFIGURATION
 * =================================================================== */

#define HAIKU_VERSION   "1.0.0"
#define MAX_VOCAB       2048
#define MAX_COOC        65536
#define MAX_BIGRAMS     32768
#define MAX_PROPHECY    32
#define MAX_CONTEXT     64
#define MAX_LINE        1024
#define DIM             64

/* Dario equation coefficients */
#define ALPHA   0.30f
#define BETA    0.15f
#define GAMMA_D 0.25f
#define TAU_BASE 0.85f

/* Positional Hebbian Profile (from Leo 2.3, RRPRAM-inspired)
 * 36 params: 32 distance weights + 4 token class modifiers. */
#define DIST_PROFILE_LEN 32
#define TOKEN_CLASSES     4
#define TC_FUNCTION  0
#define TC_CONTENT   1
#define TC_PUNCT     2
#define TC_RARE      3

/* Velocity physics */
enum { VEL_WALK=0, VEL_RUN, VEL_STOP, VEL_BREATHE, VEL_UP, VEL_DOWN };

/* Emotional chambers */
enum { CH_FEAR=0, CH_LOVE, CH_RAGE, CH_VOID, CH_FLOW, CH_COMPLEX, NUM_CHAMBERS };

/* Laws of nature */
#define ENTROPY_FLOOR      0.10f
#define RESONANCE_CEILING  0.95f
#define MAX_MOMENTUM       2.0f

/* ===================================================================
 * RNG -- xorshift64*
 * =================================================================== */

static uint64_t rng_state = 42;
static uint64_t rng_next(void) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return rng_state;
}
static float randf(void) {
    return (float)(rng_next() & 0x7FFFFFFF) / (float)0x7FFFFFFF;
}
static float clampf(float x, float lo, float hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

/* ===================================================================
 * SYLLABLE COUNTER -- heuristic, no dependencies
 *
 * English syllable counting without libraries.
 * Vowel-cluster based with common adjustments.
 * Not perfect. Haiku format is approximate. That's the point.
 * =================================================================== */

static int count_syllables(const char *word) {
    int count = 0, prev_vowel = 0, len = 0;
    const char *p = word;

    while (*p) { len++; p++; }
    if (len == 0) return 1;

    p = word;
    while (*p) {
        char c = tolower((unsigned char)*p);
        int is_vowel = (c == 'a' || c == 'e' || c == 'i' ||
                        c == 'o' || c == 'u' || c == 'y');
        if (is_vowel && !prev_vowel) count++;
        prev_vowel = is_vowel;
        p++;
    }

    /* silent e at end: collapse(3->2), fire(2->1 NO), etc. */
    if (len > 2 && tolower((unsigned char)word[len-1]) == 'e') {
        char prev = tolower((unsigned char)word[len-2]);
        /* keep -le (syllabic l): "circle", "simple" */
        if (prev == 'l') {
            /* -le after consonant = separate syllable, already counted */
        } else if (count > 1) {
            /* silent e: "collapse", "phase", "change", "stone" */
            count--;
        }
    }

    /* -ed at end: usually not a syllable unless preceded by t/d */
    if (len > 2 && tolower((unsigned char)word[len-2]) == 'e' &&
        tolower((unsigned char)word[len-1]) == 'd') {
        char before = tolower((unsigned char)word[len-3]);
        if (before != 't' && before != 'd') {
            if (count > 1) count--;
        }
    }

    return count < 1 ? 1 : count;
}

/* ===================================================================
 * VOCABULARY -- 1000 seed words
 *
 * The cloud. Constraint is sacred.
 * Expanded from HAiKU's original 587 to 1000.
 * Layer 1: common English (function words, basic verbs)
 * Layer 2: field physics (resonance, emergence, entropy)
 * Layer 3: nature and sensation (sky, rain, stone, silence)
 * Layer 4: consciousness and organism (breath, pulse, dream)
 *
 * GPT has 50k tokens. We have 1000 words.
 * GPT knows everything and says nothing.
 * We know nothing and say something.
 * =================================================================== */

static const char *SEED_WORDS[] = {
    /* -- layer 1: common english (~200) -- */
    "the", "be", "to", "of", "and", "a", "in", "that", "have", "I",
    "it", "for", "not", "on", "with", "he", "as", "you", "do", "at",
    "this", "but", "his", "by", "from", "they", "we", "say", "her", "she",
    "or", "an", "will", "my", "one", "all", "would", "there", "their", "what",
    "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
    "when", "make", "can", "like", "time", "no", "just", "him", "know", "take",
    "people", "into", "year", "your", "good", "some", "could", "them", "see", "other",
    "than", "then", "now", "look", "only", "come", "its", "over", "think", "also",
    "back", "after", "use", "two", "how", "our", "work", "first", "well", "way",
    "even", "new", "want", "because", "any", "these", "give", "day", "most", "us",
    "is", "was", "are", "been", "has", "had", "were", "said", "did",
    "may", "must", "might", "should", "shall", "ought",
    "yes", "no", "maybe", "perhaps", "here", "where", "why",
    "three", "four", "five", "many", "few", "none",
    "more", "less", "much", "too", "very", "quite", "each", "every",
    "still", "yet", "once", "twice", "never", "always", "sometimes", "often",

    /* -- layer 2: field physics & resonance (~200) -- */
    "cloud", "word", "form", "field", "phase", "wave", "mind", "thought",
    "space", "flow", "pattern", "rhythm", "pulse", "shift", "dance", "light",
    "sound", "voice", "path", "resonance", "harmony", "coherence", "dissonance",
    "tension", "release", "emergence", "constraint", "entropy", "frequency",
    "amplitude", "oscillation", "damping", "equilibrium", "attractor",
    "potential", "momentum", "inertia", "interference", "superposition",
    "coupling", "synchronization", "chaos", "order", "symmetry",
    "convergence", "divergence", "stability", "perturbation",
    "signal", "noise", "filter", "modulation", "transient", "steady",
    "saturation", "memory", "gradient", "threshold", "collapse",
    "bifurcation", "harmonic", "decay", "drift", "topology",
    "curvature", "metric", "density", "diffusion", "boundary",
    "dimension", "direction", "identity", "transformation",
    "projection", "kernel", "correlation", "variance",
    "distribution", "probability", "inference", "prediction",
    "observation", "measurement", "uncertainty", "information",
    "compression", "expansion", "contraction", "spectrum",
    "resonance", "destiny", "prophecy", "gravity", "pressure",
    "flux", "current", "resistance", "surface", "volume",

    /* -- layer 3: nature, sensation, world (~300) -- */
    "sky", "rain", "wind", "stone", "river", "mountain", "ocean", "leaf",
    "tree", "root", "seed", "bloom", "flower", "petal", "thorn",
    "earth", "dust", "ash", "fire", "flame", "smoke", "ember", "spark",
    "water", "ice", "snow", "frost", "mist", "fog", "dew", "steam",
    "sun", "moon", "star", "dawn", "dusk", "midnight", "morning", "evening",
    "spring", "summer", "autumn", "winter", "season", "tide", "storm",
    "thunder", "lightning", "rainbow", "horizon", "shore", "sand", "salt",
    "sea", "lake", "creek", "pool", "well", "cave", "cliff", "hill",
    "valley", "meadow", "garden", "forest", "grove", "wood", "bark",
    "bone", "blood", "skin", "hand", "eye", "mouth", "tongue", "ear",
    "heart", "lung", "vein", "nerve", "spine", "skull", "rib",
    "bird", "wing", "nest", "egg", "fish", "scale", "fin",
    "wolf", "deer", "bear", "fox", "crow", "owl", "hawk", "moth",
    "spider", "web", "thread", "silk", "wool", "cotton", "linen",
    "iron", "copper", "gold", "silver", "glass", "clay", "wax",
    "ink", "paint", "color", "red", "blue", "green", "white", "black",
    "gray", "amber", "violet", "indigo", "scarlet", "crimson",
    "bright", "dark", "shadow", "transparent", "opaque", "visible",
    "loud", "quiet", "silent", "noisy", "soft", "harsh", "gentle",
    "sharp", "smooth", "rough", "sweet", "bitter", "sour", "salty",
    "warm", "cool", "hot", "cold", "burning", "freezing",
    "dry", "wet", "damp", "fresh", "stale", "raw", "ripe",
    "heavy", "light", "thick", "thin", "deep", "shallow",
    "wide", "narrow", "long", "short", "tall", "small", "large",
    "tiny", "huge", "vast", "little", "big", "broad",
    "near", "far", "close", "distant", "high", "low",
    "fast", "slow", "quick", "gradual", "sudden", "instant",
    "strong", "weak", "solid", "fluid", "rigid", "flexible",
    "whole", "part", "piece", "fragment", "full", "empty",
    "old", "young", "ancient", "modern", "new",
    "true", "false", "real", "hidden", "clear", "obscure",

    /* -- layer 4: consciousness, organism, existence (~300) -- */
    "self", "other", "between", "within", "beyond", "toward",
    "inside", "outside", "around", "through", "across", "along",
    "above", "below", "under", "among", "amid", "before", "after",
    "sense", "feel", "wonder", "question", "answer", "search",
    "find", "lose", "create", "destroy", "build", "break",
    "shape", "morph", "change", "stay", "move", "rest",
    "grow", "shrink", "expand", "open", "close", "begin", "end",
    "continue", "stop", "wait", "watch", "listen", "touch",
    "taste", "smell", "breathe", "sleep", "wake", "dream",
    "remember", "forget", "imagine", "believe", "trust", "doubt",
    "hope", "fear", "love", "rage", "joy", "grief", "sorrow",
    "pain", "pleasure", "comfort", "hunger", "thirst", "desire",
    "will", "intention", "purpose", "meaning", "truth", "lie",
    "silence", "whisper", "scream", "song", "language",
    "sentence", "letter", "name", "story", "poem", "verse",
    "birth", "death", "alive", "dead", "living", "dying",
    "growing", "vital", "dormant", "active",
    "breath", "heartbeat", "tremor", "shiver", "sweat",
    "tear", "laugh", "smile", "frown", "sigh",
    "walk", "run", "leap", "fall", "rise", "climb", "crawl",
    "swim", "fly", "float", "sink", "drift", "wander",
    "gather", "scatter", "merge", "split", "connect", "separate",
    "hold", "release", "catch", "throw", "pull", "push",
    "lift", "carry", "drop", "pour", "fill", "drain",
    "cut", "fold", "bend", "twist", "turn", "spin", "roll",
    "burn", "melt", "freeze", "crack", "shatter", "crush",
    "weave", "knit", "sew", "tie", "knot", "bind", "free",
    "lock", "unlock", "seal", "reveal", "hide", "show",
    "teach", "learn", "study", "practice", "master",
    "give", "receive", "share", "keep", "trade",
    "call", "echo", "ring", "hum", "buzz", "click",
    "home", "exile", "return", "journey", "origin",
    "mirror", "window", "door", "wall", "floor", "ceiling",
    "bridge", "gate", "fence", "road", "trail", "step",
    "circle", "spiral", "line", "curve", "angle", "edge",
    "center", "margin", "border", "frame", "core", "shell",
    "layer", "depth", "height", "width", "weight", "measure",
    "number", "zero", "one", "half", "double", "pair",
    "same", "different", "equal", "opposite", "parallel",
    "together", "apart", "alone", "crowd", "solitude",
    "simple", "complex", "easy", "hard", "possible", "impossible",
    "certain", "uncertain", "obvious", "subtle", "plain", "rich",
    "empty", "full", "blank", "marked", "clean", "worn",
    "morning", "night", "hour", "moment", "instant", "age",
    "child", "elder", "stranger", "friend", "ghost", "shadow",
    "nothing", "everything", "somewhere", "nowhere", "anywhere",
    "organism", "cell", "root", "branch", "canopy", "soil",
    "mycelium", "spore", "pollen", "nectar", "sap", "resin",

    /* -- layer 5: additional nature (~70) -- */
    "cloud", "marsh", "swamp", "delta", "gorge", "ridge",
    "peak", "summit", "abyss", "chasm", "rift", "fault",
    "lava", "magma", "quartz", "granite", "obsidian", "chalk",
    "moss", "fern", "vine", "lichen", "fungus", "algae",
    "coral", "kelp", "barnacle", "mussel", "starfish", "whale",
    "dolphin", "eel", "squid", "jellyfish", "turtle", "snake",
    "lizard", "frog", "beetle", "ant", "bee", "wasp",
    "butterfly", "dragonfly", "cricket", "worm", "snail", "shell",
    "feather", "claw", "horn", "tusk", "fur", "mane",
    "tail", "hoof", "paw", "beak", "talon", "fang",
    "marsh", "brook", "spring", "geyser", "waterfall", "rapids",

    /* -- layer 6: time, music, abstract (~70) -- */
    "rhythm", "tempo", "chord", "pitch", "tone", "note",
    "melody", "bass", "treble", "octave", "minor", "major",
    "rest", "pause", "beat", "measure", "bar", "refrain",
    "verse", "chorus", "bridge", "coda", "loop", "fade",
    "echo", "reverb", "drone", "hiss", "crack", "boom",
    "rumble", "rattle", "chime", "bell", "drum", "flute",
    "string", "reed", "brass", "pipe", "bow", "pluck",
    "strum", "strike", "mute", "sustain", "vibrate", "oscillate",
    "century", "decade", "epoch", "era", "eon", "tick",
    "second", "minute", "span", "duration", "lapse", "interval",
    "cycle", "loop", "orbit", "arc", "helix", "vortex",

    /* -- layer 7: human, emotion, thought (~70) -- */
    "courage", "wisdom", "patience", "grace", "mercy", "pride",
    "shame", "guilt", "envy", "wrath", "sloth", "greed",
    "kindness", "cruelty", "justice", "chaos", "mercy", "fury",
    "calm", "panic", "dread", "awe", "bliss", "agony",
    "longing", "nostalgia", "regret", "resolve", "defiance",
    "surrender", "triumph", "defeat", "struggle", "peace", "war",
    "silence", "roar", "murmur", "chant", "prayer", "oath",
    "promise", "curse", "blessing", "fate", "chance", "luck",
    "riddle", "paradox", "enigma", "secret", "clue", "sign",
    "symbol", "glyph", "rune", "mark", "trace", "echo",
    "remnant", "relic", "fossil", "artifact", "ruin", "tomb",
    "altar", "threshold", "passage", "portal", "labyrinth", "maze",

    /* -- layer 8: texture, material, craft (~70) -- */
    "grain", "fiber", "mesh", "lattice", "grid", "weave",
    "knot", "stitch", "patch", "seam", "hem", "fold",
    "pleat", "crease", "wrinkle", "dent", "scratch", "groove",
    "notch", "slit", "gap", "void", "hollow", "chamber",
    "vessel", "basin", "bowl", "cup", "jar", "flask",
    "vial", "lens", "prism", "crystal", "gem", "pearl",
    "amber", "jade", "onyx", "opal", "ruby", "sapphire",
    "marble", "slate", "timber", "bamboo", "rope", "chain",
    "wire", "cable", "bolt", "nail", "pin", "hook",
    "blade", "needle", "chisel", "anvil", "forge", "kiln",
    "loom", "spindle", "wheel", "axle", "lever", "pulley",
    "hinge", "clasp", "buckle", "rivet", "weld", "solder",

    /* -- layer 9: final additions to reach 1000 -- */
    "canyon", "plateau", "tundra", "steppe", "oasis", "dune",
    "archipelago", "peninsula", "isthmus", "fjord", "atoll",
    "aurora", "eclipse", "zenith", "nadir", "equinox", "solstice",
    "comet", "nebula", "quasar", "pulsar", "nova", "cosmos",
    "infinity", "origin", "void", "aether", "plasma", "photon",

    NULL  /* sentinel */
};

/* ===================================================================
 * TOKENIZER -- word-level, lowercased
 * =================================================================== */

typedef struct {
    char words[MAX_VOCAB][64];
    float weight[MAX_VOCAB];     /* cloud weight: active words boosted */
    int  n_words;
} Vocab;

static int vocab_find(Vocab *v, const char *word) {
    for (int i = 0; i < v->n_words; i++)
        if (strcmp(v->words[i], word) == 0) return i;
    return -1;
}

static int vocab_add(Vocab *v, const char *word) {
    int id = vocab_find(v, word);
    if (id >= 0) return id;
    if (v->n_words >= MAX_VOCAB) return -1;
    id = v->n_words++;
    snprintf(v->words[id], sizeof(v->words[id]), "%s", word);
    v->weight[id] = 1.0f;
    return id;
}

static int tokenize(Vocab *v, const char *text, int *ids, int max) {
    int n = 0;
    char buf[64];
    int bi = 0;
    for (const char *p = text; ; p++) {
        if (*p && (isalnum((unsigned char)*p) || *p == '\'' || *p == '-')) {
            if (bi < 62) buf[bi++] = tolower((unsigned char)*p);
        } else {
            if (bi > 0) {
                buf[bi] = '\0';
                int id = vocab_add(v, buf);
                if (id >= 0 && n < max) ids[n++] = id;
                bi = 0;
            }
            if (!*p) break;
        }
    }
    return n;
}

/* ===================================================================
 * BIGRAM TABLE
 * =================================================================== */

typedef struct {
    int src[MAX_BIGRAMS], dst[MAX_BIGRAMS];
    float count[MAX_BIGRAMS];
    int n;
} BigramTable;

static void bigram_update(BigramTable *b, int src, int dst, float delta) {
    for (int i = 0; i < b->n; i++)
        if (b->src[i] == src && b->dst[i] == dst) {
            b->count[i] += delta; return;
        }
    if (b->n >= MAX_BIGRAMS) return;
    int i = b->n++;
    b->src[i] = src; b->dst[i] = dst; b->count[i] = delta;
}

static void bigram_row(BigramTable *b, int src, float *out, int vocab) {
    for (int i = 0; i < vocab; i++) out[i] = 0;
    for (int i = 0; i < b->n; i++)
        if (b->src[i] == src && b->dst[i] < vocab)
            out[b->dst[i]] = b->count[i];
}

/* ===================================================================
 * CO-OCCURRENCE FIELD (sparse)
 * =================================================================== */

typedef struct {
    int src[MAX_COOC], dst[MAX_COOC];
    float count[MAX_COOC];
    float freq[MAX_VOCAB];
    int n, total;
} CoocField;

static void cooc_update(CoocField *c, int src, int dst, float delta) {
    for (int i = 0; i < c->n; i++)
        if (c->src[i] == src && c->dst[i] == dst) {
            c->count[i] += delta; return;
        }
    if (c->n >= MAX_COOC) return;
    int i = c->n++;
    c->src[i] = src; c->dst[i] = dst; c->count[i] = delta;
}

/* ===================================================================
 * PROPHECY -- small bets about what comes next
 * =================================================================== */

typedef struct {
    int target; float strength; int age; int fulfilled;
} Prophecy;

typedef struct {
    Prophecy p[MAX_PROPHECY];
    int n;
} ProphecySystem;

static void prophecy_add(ProphecySystem *ps, int target, float strength) {
    if (ps->n >= MAX_PROPHECY) {
        int oldest = 0;
        for (int i = 1; i < ps->n; i++)
            if (ps->p[i].age > ps->p[oldest].age) oldest = i;
        ps->p[oldest] = ps->p[--ps->n];
    }
    ps->p[ps->n++] = (Prophecy){target, strength, 0, 0};
}

static void prophecy_update(ProphecySystem *ps, int token) {
    for (int i = 0; i < ps->n; i++) {
        if (ps->p[i].target == token) ps->p[i].fulfilled = 1;
        ps->p[i].age++;
    }
    int w = 0;
    for (int i = 0; i < ps->n; i++)
        if (!ps->p[i].fulfilled && ps->p[i].age < 50)
            ps->p[w++] = ps->p[i];
    ps->n = w;
}

/* ===================================================================
 * DESTINY -- EMA of context embeddings
 * =================================================================== */

static float g_destiny[DIM];
static float g_dest_magnitude = 0;

static float vec_dot(const float *a, const float *b, int n) {
    float s = 0; for (int i = 0; i < n; i++) s += a[i] * b[i]; return s;
}
static float vec_norm(const float *v, int n) {
    return sqrtf(vec_dot(v, v, n) + 1e-12f);
}
static float vec_cosine(const float *a, const float *b, int n) {
    return vec_dot(a, b, n) / (vec_norm(a, n) * vec_norm(b, n) + 1e-12f);
}

/* ===================================================================
 * EMBEDDINGS -- hash-based deterministic init
 * =================================================================== */

static float g_embeds[MAX_VOCAB][DIM];
static int g_embed_init[MAX_VOCAB];

static float *get_embed(int id) {
    if (id < 0 || id >= MAX_VOCAB) return NULL;
    if (!g_embed_init[id]) {
        uint32_t h = 2166136261u;
        for (int i = 0; i < 4; i++) {
            h ^= (id >> (i * 8)) & 0xFF;
            h *= 16777619u;
        }
        for (int d = 0; d < DIM; d++) {
            h = h * 1103515245 + 12345;
            g_embeds[id][d] = ((float)(h & 0x7FFFFFFF) / (float)0x7FFFFFFF - 0.5f) * 0.1f;
        }
        float norm = vec_norm(g_embeds[id], DIM);
        for (int d = 0; d < DIM; d++) g_embeds[id][d] /= norm;
        g_embed_init[id] = 1;
    }
    return g_embeds[id];
}

/* RoPE */
static void apply_rope(float *vec, int dim, int pos) {
    for (int i = 0; i < dim; i += 2) {
        float theta = (float)pos * powf(10000.0f, -(float)i / dim);
        float c = cosf(theta), s = sinf(theta);
        float x = vec[i], y = vec[i + 1];
        vec[i]     = x * c - y * s;
        vec[i + 1] = x * s + y * c;
    }
}

/* SwiGLU gate */
static float swiglu_gate(float x, float gate) {
    float sig = 1.0f / (1.0f + expf(-gate));
    return x * sig;
}

/* ===================================================================
 * FIELD STATE -- the soul
 * =================================================================== */

typedef struct {
    Vocab          vocab;
    BigramTable    bigrams;
    CoocField      cooc;
    ProphecySystem prophecy;

    /* context */
    int   context[MAX_CONTEXT];
    int   ctx_len;

    /* field metrics */
    float dissonance;
    float entropy;
    float resonance;
    float emergence;
    float debt;
    float trauma_level;
    float momentum;

    /* velocity */
    int   velocity;
    float tau;

    /* Dario coefficients */
    float alpha, beta, gamma_d;

    /* term dominance */
    float term_energy[5];   /* B H F A T */
    int   dominant_term;

    /* emotional chambers */
    float chamber[NUM_CHAMBERS];
    float alpha_mod, beta_mod, gamma_mod, tau_mod;
    float vel_temp;

    /* season */
    int   season;
    float season_phase;

    /* positional Hebbian profile (RRPRAM-inspired, 36 params) */
    float dist_profile[DIST_PROFILE_LEN];
    float class_mod[TOKEN_CLASSES];
    int   dist_profile_updates;

    /* lifetime */
    int   step;
    int   conv_count;
} HaikuState;

static HaikuState H;

/* ===================================================================
 * EMOTIONAL CHAMBERS -- Kuramoto-coupled somatic markers
 * =================================================================== */

static void chamber_update(void) {
    float *C = H.chamber;

    if (H.dissonance > 0.7f) C[CH_FEAR] += 0.05f * H.dissonance;
    if (H.resonance > 0.7f)  C[CH_LOVE] += 0.04f * H.resonance;
    if (H.trauma_level > 0.5f && H.dissonance > 0.5f)
        C[CH_RAGE] += 0.06f * H.trauma_level;
    if (H.entropy > 0.7f)    C[CH_VOID] += 0.03f * H.entropy;
    if (H.emergence > 0.5f)  C[CH_FLOW] += 0.05f * H.emergence;
    C[CH_COMPLEX] += 0.04f * fabsf(C[CH_LOVE] - C[CH_RAGE])
                   * (C[CH_LOVE] > 0.2f && C[CH_RAGE] > 0.2f ? 1.0f : 0.0f);

    /* Kuramoto coupling */
    float K = 0.02f;
    float old[NUM_CHAMBERS];
    memcpy(old, C, sizeof(old));
    for (int i = 0; i < NUM_CHAMBERS; i++)
        for (int j = 0; j < NUM_CHAMBERS; j++)
            if (i != j) C[i] += K * sinf(old[j] - old[i]);

    float decay[] = { 0.95f, 0.95f, 0.93f, 0.96f, 0.94f, 0.97f };
    for (int i = 0; i < NUM_CHAMBERS; i++)
        C[i] = clampf(C[i] * decay[i], 0.0f, 1.0f);

    /* somatic markers -> coefficient modulation */
    H.alpha_mod = clampf(1.0f + 0.3f * C[CH_LOVE] - 0.2f * C[CH_RAGE]
                              + 0.1f * C[CH_FLOW], 0.5f, 2.0f);
    H.beta_mod  = clampf(1.0f + 0.2f * C[CH_FLOW] - 0.3f * C[CH_FEAR],
                         0.5f, 2.0f);
    H.gamma_mod = clampf(1.0f + 0.4f * C[CH_VOID] + 0.2f * C[CH_COMPLEX]
                              - 0.1f * C[CH_LOVE], 0.5f, 2.0f);
    H.tau_mod   = clampf(1.0f + 0.5f * C[CH_FLOW] - 0.3f * C[CH_FEAR],
                         0.5f, 2.0f);
}

/* ===================================================================
 * DISSONANCE -- how far are your words from my words?
 *
 * High dissonance -> formula heats up, trauma pulls
 * Low dissonance  -> bigrams dominate, steady state
 *
 * Dissonance is the signal. Harmony is the noise.
 * =================================================================== */

static float compute_dissonance(const char *input) {
    int n_words = 0, n_known = 0;
    char buf[64]; int bi = 0;
    for (const char *p = input; ; p++) {
        if (*p && (isalnum((unsigned char)*p) || *p == '\'' || *p == '-')) {
            if (bi < 62) buf[bi++] = tolower((unsigned char)*p);
        } else {
            if (bi > 0) {
                buf[bi] = '\0';
                n_words++;
                if (vocab_find(&H.vocab, buf) >= 0) n_known++;
                bi = 0;
            }
            if (!*p) break;
        }
    }
    if (n_words == 0) return 1.0f;

    float unknown_ratio = 1.0f - (float)n_known / n_words;

    /* overlap with recent context */
    float ctx_overlap = 0;
    {
        int bi2 = 0; char buf2[64];
        for (const char *p2 = input; ; p2++) {
            if (*p2 && (isalnum((unsigned char)*p2) || *p2 == '\'' || *p2 == '-')) {
                if (bi2 < 62) buf2[bi2++] = tolower((unsigned char)*p2);
            } else {
                if (bi2 > 0) {
                    buf2[bi2] = '\0';
                    int wid = vocab_find(&H.vocab, buf2);
                    if (wid >= 0) {
                        for (int c = 0; c < H.ctx_len; c++)
                            if (wid == H.context[c]) { ctx_overlap++; break; }
                    }
                    bi2 = 0;
                }
                if (!*p2) break;
            }
        }
    }
    float overlap_ratio = (n_words > 0) ? ctx_overlap / n_words : 0;

    float dis = 0.6f * unknown_ratio + 0.4f * (1.0f - overlap_ratio);
    return clampf(dis, 0.0f, 1.0f);
}

/* ===================================================================
 * VELOCITY OPERATORS
 * =================================================================== */

static void apply_velocity(void) {
    float tau = TAU_BASE;

    switch (H.velocity) {
    case VEL_WALK:
        H.alpha  += (ALPHA - H.alpha) * 0.1f;
        H.beta   += (BETA - H.beta) * 0.1f;
        H.gamma_d += (GAMMA_D - H.gamma_d) * 0.1f;
        tau = 0.85f;
        break;
    case VEL_RUN:
        tau = 1.15f;
        H.momentum = clampf(H.momentum + 0.1f, 0, MAX_MOMENTUM);
        break;
    case VEL_STOP:
        H.momentum = 0;
        H.gamma_d = clampf(H.gamma_d + 0.15f, 0, 0.8f);
        tau = 0.4f;
        break;
    case VEL_BREATHE:
        H.trauma_level *= 0.7f;
        H.dissonance *= 0.8f;
        H.debt *= 0.5f;
        tau = 0.75f;
        break;
    case VEL_UP:
        H.beta = clampf(H.beta + 0.15f, 0, 0.8f);
        H.alpha = clampf(H.alpha - 0.05f, 0.05f, 0.5f);
        tau = 1.3f;
        break;
    case VEL_DOWN:
        H.alpha = clampf(H.alpha + 0.1f, 0.05f, 0.6f);
        H.beta = clampf(H.beta - 0.05f, 0.05f, 0.5f);
        tau = 0.6f;
        break;
    }

    float vel_temp = 1.0f;
    if (H.velocity == VEL_RUN)  vel_temp = 1.15f;
    if (H.velocity == VEL_UP)   vel_temp = 1.3f;
    if (H.velocity == VEL_DOWN) vel_temp = 0.8f;
    H.vel_temp = vel_temp;

    if (H.trauma_level > 0.3f)
        tau *= 1.0f + 0.3f * H.trauma_level;

    H.tau = clampf(tau, 0.2f, 2.0f);
}

static void auto_velocity(void) {
    if (H.dissonance > 0.8f)
        H.velocity = VEL_UP;
    else if (H.dissonance > 0.6f)
        H.velocity = VEL_RUN;
    else if (H.dissonance < 0.2f)
        H.velocity = VEL_STOP;
    else if (H.trauma_level > 0.5f)
        H.velocity = VEL_BREATHE;
    else if (H.debt > 5.0f)
        H.velocity = VEL_DOWN;
    else
        H.velocity = VEL_WALK;
}

/* ===================================================================
 * SEASONAL MODULATION
 * =================================================================== */

static void season_step(void) {
    H.season_phase += 0.002f;
    if (H.season_phase >= 1.0f) {
        H.season_phase = 0;
        H.season = (H.season + 1) % 4;
    }

    float mod = 0.005f;
    switch (H.season) {
    case 0: H.beta += mod; break;       /* spring: prophecy */
    case 1: H.alpha += mod; break;      /* summer: resonance */
    case 2: break;                       /* autumn: consolidation */
    case 3:                              /* winter: trauma */
        H.trauma_level = clampf(H.trauma_level + 0.005f, 0, 0.4f);
        break;
    }

    H.alpha = clampf(H.alpha, 0.05f, 0.6f);
    H.beta = clampf(H.beta, 0.05f, 0.6f);
    H.gamma_d = clampf(H.gamma_d, 0.05f, 0.6f);
}

/* ===================================================================
 * THE DARIO EQUATION -- the heart
 *
 * p(x|Phi) = softmax(
 *     (B + a_mod*a*H + b_mod*b*F + g_mod*g*A + T)
 *     / (t_mod*tau*v_tau)
 * )
 * =================================================================== */

static int token_class(int token_id) {
    if (token_id >= 0 && token_id < H.vocab.n_words) {
        const char *w = H.vocab.words[token_id];
        if (w && (w[0] == '.' || w[0] == ',' || w[0] == '!' ||
                  w[0] == '?' || w[0] == ';' || w[0] == ':'))
            return TC_PUNCT;
    }
    float freq = (token_id < MAX_VOCAB) ? H.cooc.freq[token_id] : 0;
    float total = (float)H.cooc.total + 1.0f;
    if (freq < 2.0f) return TC_RARE;
    float idf = logf(total / (freq + 1.0f));
    float max_idf = logf(total);
    float norm_idf = idf / (max_idf + 1e-6f);
    return (norm_idf < 0.3f) ? TC_FUNCTION : TC_CONTENT;
}

static void haiku_compute(float *logits, int vocab_size) {
    float *B = calloc(vocab_size, sizeof(float));
    float *Heb = calloc(vocab_size, sizeof(float));
    float *F = calloc(vocab_size, sizeof(float));
    float *A = calloc(vocab_size, sizeof(float));
    float *T = calloc(vocab_size, sizeof(float));

    /* -- B: Sequential Chain -- */
    float bigram_coeff = 8.0f;
    if (H.season == 2) bigram_coeff *= 1.3f;
    if (H.velocity == VEL_RUN) bigram_coeff *= 1.3f;

    if (H.ctx_len > 0) {
        int last = H.context[H.ctx_len - 1];
        bigram_row(&H.bigrams, last, B, vocab_size);
        float mx = 0;
        for (int i = 0; i < vocab_size; i++)
            if (B[i] > mx) mx = B[i];
        if (mx > 1e-6f)
            for (int i = 0; i < vocab_size; i++) B[i] /= mx;
    }

    /* -- H: Hebbian Resonance (positional profile, 36 learnable params) -- */
    int ctx_start = (H.ctx_len > 8) ? H.ctx_len - 8 : 0;
    for (int c = ctx_start; c < H.ctx_len; c++) {
        int ctx_id = H.context[c];
        int dist = H.ctx_len - 1 - c;
        float decay = (dist < DIST_PROFILE_LEN)
                    ? H.dist_profile[dist]
                    : H.dist_profile[DIST_PROFILE_LEN - 1] * 0.5f;
        int tc = token_class(ctx_id);
        decay *= H.class_mod[tc];
        for (int i = 0; i < H.cooc.n; i++) {
            if (H.cooc.src[i] == ctx_id && H.cooc.dst[i] < vocab_size)
                Heb[H.cooc.dst[i]] += H.cooc.count[i] * decay;
        }
    }
    float h_max = 0;
    for (int i = 0; i < vocab_size; i++)
        if (Heb[i] > h_max) h_max = Heb[i];
    if (h_max > 1e-6f)
        for (int i = 0; i < vocab_size; i++) Heb[i] /= h_max;

    /* -- F: Prophecy Fulfillment -- */
    for (int i = 0; i < vocab_size; i++) {
        float *te = get_embed(i);
        if (!te) continue;
        float score = 0;
        for (int p = 0; p < H.prophecy.n; p++) {
            Prophecy *pr = &H.prophecy.p[p];
            if (pr->fulfilled) continue;
            float *pe = get_embed(pr->target);
            if (!pe) continue;
            float sim = vec_cosine(te, pe, DIM);
            if (sim < 0) sim = 0;
            float debt = logf(1.0f + (float)pr->age);
            score += pr->strength * sim * debt;
        }
        F[i] = score;
    }
    float f_max = 0;
    for (int i = 0; i < vocab_size; i++)
        if (F[i] > f_max) f_max = F[i];
    if (f_max > 1e-6f)
        for (int i = 0; i < vocab_size; i++) F[i] /= f_max;

    /* -- A: Destiny Attraction -- */
    if (g_dest_magnitude > 1e-6f) {
        for (int i = 0; i < vocab_size; i++) {
            float *te = get_embed(i);
            if (te) A[i] = vec_cosine(te, g_destiny, DIM) * g_dest_magnitude;
        }
        float a_max = 0;
        for (int i = 0; i < vocab_size; i++)
            if (fabsf(A[i]) > a_max) a_max = fabsf(A[i]);
        if (a_max > 1e-6f)
            for (int i = 0; i < vocab_size; i++) A[i] /= a_max;
    }

    /* -- T: Trauma Gravity -- */
    if (H.trauma_level > 0.3f) {
        float boost = H.trauma_level * 3.0f;
        for (int i = 0; i < vocab_size && i < 50; i++)
            T[i] = boost * (1.0f - (float)i / 50.0f);
    }

    /* -- Combine: THE DARIO EQUATION -- */
    float gamma_eff = H.gamma_d;
    if (H.trauma_level > 0.3f)
        gamma_eff += H.trauma_level * 1.5f;

    float eff_alpha = H.alpha_mod * H.alpha;
    float eff_beta  = H.beta_mod * H.beta;
    float eff_gamma = H.gamma_mod * gamma_eff;

    float e_B = 0, e_H = 0, e_F = 0, e_A = 0, e_T = 0;

    for (int i = 0; i < vocab_size; i++) {
        float b_term = bigram_coeff * B[i];
        float h_term = eff_alpha * Heb[i];
        float f_term = eff_beta * F[i];
        float a_term = eff_gamma * A[i];
        float t_term = T[i];

        /* SwiGLU gate through field resonance */
        float gate = 1.0f / (1.0f + expf(-(H.resonance - 0.5f) * 4.0f));
        h_term = swiglu_gate(h_term, gate * 2.0f);
        f_term = swiglu_gate(f_term, gate * 1.5f);

        /* cloud weight modulation */
        float cw = (i < H.vocab.n_words) ? H.vocab.weight[i] : 1.0f;

        logits[i] = (b_term + h_term + f_term + a_term + t_term) * cw;

        e_B += fabsf(b_term);
        e_H += fabsf(h_term);
        e_F += fabsf(f_term);
        e_A += fabsf(a_term);
        e_T += fabsf(t_term);
    }

    H.term_energy[0] = e_B;
    H.term_energy[1] = e_H;
    H.term_energy[2] = e_F;
    H.term_energy[3] = e_A;
    H.term_energy[4] = e_T;

    float mx = 0;
    H.dominant_term = 0;
    for (int t = 0; t < 5; t++)
        if (H.term_energy[t] > mx) { mx = H.term_energy[t]; H.dominant_term = t; }

    free(B); free(Heb); free(F); free(A); free(T);
}

/* ===================================================================
 * SAMPLING -- softmax + temperature + top-k
 * =================================================================== */

static int sample_topk(float *logits, int n, float tau, int topk) {
    float mx = -1e30f;
    for (int i = 0; i < n; i++)
        if (logits[i] > mx) mx = logits[i];

    float sum = 0;
    for (int i = 0; i < n; i++) {
        logits[i] = expf((logits[i] - mx) / tau);
        sum += logits[i];
    }
    for (int i = 0; i < n; i++) logits[i] /= sum;

    if (topk > 0 && topk < n) {
        float thresh = -1;
        float *sorted = malloc(n * sizeof(float));
        memcpy(sorted, logits, n * sizeof(float));
        for (int k = 0; k < topk; k++) {
            int best = k;
            for (int j = k + 1; j < n; j++)
                if (sorted[j] > sorted[best]) best = j;
            float tmp = sorted[k]; sorted[k] = sorted[best]; sorted[best] = tmp;
        }
        thresh = sorted[topk - 1];
        free(sorted);
        for (int i = 0; i < n; i++)
            if (logits[i] < thresh) logits[i] = 0;
        sum = 0;
        for (int i = 0; i < n; i++) sum += logits[i];
        if (sum > 0) for (int i = 0; i < n; i++) logits[i] /= sum;
    }

    float r = randf(), cum = 0;
    for (int i = 0; i < n; i++) {
        cum += logits[i];
        if (cum >= r) return i;
    }
    return n - 1;
}

/* ===================================================================
 * LAW ENFORCEMENT -- the invariants
 * =================================================================== */

static void enforce_laws(void) {
    if (H.entropy < ENTROPY_FLOOR) H.entropy = ENTROPY_FLOOR;
    if (H.resonance > RESONANCE_CEILING) H.resonance = RESONANCE_CEILING;
    H.emergence = clampf((1.0f - H.entropy) * H.resonance, 0, 1);
    H.debt *= 0.98f;
    if (H.debt > 20.0f) H.debt = 20.0f;
    H.trauma_level *= 0.97f;
    H.momentum *= 0.95f;
}

/* ===================================================================
 * INGEST -- process input, update field
 * =================================================================== */

static void ingest(const char *text) {
    int ids[256];
    int n = tokenize(&H.vocab, text, ids, 256);
    if (n == 0) return;

    /* frequencies */
    for (int i = 0; i < n; i++) {
        if (ids[i] < MAX_VOCAB)
            H.cooc.freq[ids[i]] += 1.0f;
        H.cooc.total++;
    }

    /* bigrams */
    for (int i = 0; i < n - 1; i++)
        bigram_update(&H.bigrams, ids[i], ids[i + 1], 1.0f);

    /* co-occurrence (windowed) */
    for (int i = 0; i < n; i++) {
        int start = (i - 5 > 0) ? i - 5 : 0;
        int end = (i + 5 < n) ? i + 5 : n;
        for (int j = start; j < end; j++) {
            if (j == i) continue;
            float w = 1.0f / (float)(abs(i - j));
            cooc_update(&H.cooc, ids[i], ids[j], w);
        }
    }

    /* destiny update with RoPE */
    for (int i = 0; i < n; i++) {
        float *e = get_embed(ids[i]);
        if (!e) continue;
        float pos_e[DIM];
        memcpy(pos_e, e, DIM * sizeof(float));
        apply_rope(pos_e, DIM, H.step + i);
        for (int d = 0; d < DIM; d++)
            g_destiny[d] = 0.1f * pos_e[d] + 0.9f * g_destiny[d];
    }
    g_dest_magnitude = vec_norm(g_destiny, DIM);

    /* cloud morphing: boost active words */
    for (int i = 0; i < n; i++) {
        if (ids[i] < H.vocab.n_words)
            H.vocab.weight[ids[i]] = clampf(
                H.vocab.weight[ids[i]] * 1.05f, 0.1f, 3.0f);
    }

    /* cloud decay: dormant words slowly fade */
    for (int i = 0; i < H.vocab.n_words; i++) {
        int active = 0;
        for (int j = 0; j < n; j++)
            if (ids[j] == i) { active = 1; break; }
        if (!active)
            H.vocab.weight[i] *= 0.998f;
        if (H.vocab.weight[i] < 0.1f)
            H.vocab.weight[i] = 0.1f;
    }

    /* update context window */
    for (int i = 0; i < n; i++) {
        if (H.ctx_len < MAX_CONTEXT)
            H.context[H.ctx_len++] = ids[i];
        else {
            memmove(H.context, H.context + 1, (MAX_CONTEXT - 1) * sizeof(int));
            H.context[MAX_CONTEXT - 1] = ids[i];
        }
    }

    H.step += n;
}

/* ===================================================================
 * METRICS UPDATE
 * =================================================================== */

static void update_metrics(void) {
    H.entropy = clampf(
        (H.tau - 0.5f) * 0.3f +
        H.dissonance * 0.4f +
        (1.0f - H.resonance) * 0.3f,
        0.0f, 1.0f
    );

    float density = (H.cooc.n > 100) ? 1.0f : (float)H.cooc.n / 100.0f;
    H.resonance = clampf(
        density * 0.4f +
        (1.0f - H.dissonance) * 0.3f +
        (1.0f - clampf(H.debt * 0.1f, 0, 1)) * 0.3f,
        0.0f, 1.0f
    );
}

/* ===================================================================
 * GENERATE HAIKU -- the constraint births form
 *
 * Three lines. 5-7-5 syllables. No negotiation.
 * The Dario equation selects each word.
 * Constraint -> compression -> selection -> meaning.
 * This is how emergence works: pressure creates form.
 * =================================================================== */

static int generate_line(char *out, int max_len, int target_syllables) {
    int vocab = H.vocab.n_words;
    if (vocab < 10) return 0;

    float *logits = calloc(vocab, sizeof(float));
    int pos = 0, syllables = 0;
    out[0] = '\0';

    int max_words = 12; /* safety limit */

    for (int t = 0; t < max_words && syllables < target_syllables; t++) {
        haiku_compute(logits, vocab);

        /* repetition penalty */
        for (int c = 0; c < H.ctx_len; c++) {
            int id = H.context[c];
            if (id < vocab) logits[id] *= 0.3f;
        }
        if (H.ctx_len > 0) {
            int last = H.context[H.ctx_len - 1];
            if (last < vocab) logits[last] = -1e30f;
        }

        /* syllable constraint: suppress words that would overshoot */
        int needed = target_syllables - syllables;
        for (int i = 0; i < vocab; i++) {
            int ws = count_syllables(H.vocab.words[i]);
            if (ws > needed) logits[i] = -1e30f;
        }

        float eff_tau = H.tau_mod * H.tau * H.vel_temp;
        eff_tau = clampf(eff_tau, 0.2f, 3.0f);
        int next = sample_topk(logits, vocab, eff_tau, 15);

        const char *word = H.vocab.words[next];
        int ws = count_syllables(word);
        int wlen = strlen(word);

        if (pos + wlen + 2 >= max_len) break;
        if (pos > 0) out[pos++] = ' ';
        memcpy(out + pos, word, wlen);
        pos += wlen;
        out[pos] = '\0';
        syllables += ws;

        /* learn from generation */
        if (H.ctx_len > 0)
            bigram_update(&H.bigrams, H.context[H.ctx_len - 1], next, 0.5f);
        for (int c = 0; c < H.ctx_len; c++) {
            float w = 1.0f / (float)(H.ctx_len - c);
            cooc_update(&H.cooc, H.context[c], next, w * 0.3f);
        }
        if (next < MAX_VOCAB) H.cooc.freq[next] += 0.5f;

        /* prophecy */
        prophecy_update(&H.prophecy, next);
        float best_cooc = -1; int best_pred = -1;
        for (int i = 0; i < H.cooc.n; i++)
            if (H.cooc.src[i] == next && H.cooc.count[i] > best_cooc) {
                best_cooc = H.cooc.count[i]; best_pred = H.cooc.dst[i];
            }
        if (best_pred >= 0) prophecy_add(&H.prophecy, best_pred, 0.3f);

        /* context */
        if (H.ctx_len < MAX_CONTEXT)
            H.context[H.ctx_len++] = next;
        else {
            memmove(H.context, H.context + 1, (MAX_CONTEXT - 1) * sizeof(int));
            H.context[MAX_CONTEXT - 1] = next;
        }

        /* destiny */
        float *e = get_embed(next);
        if (e) {
            for (int d = 0; d < DIM; d++)
                g_destiny[d] = 0.1f * e[d] + 0.9f * g_destiny[d];
            g_dest_magnitude = vec_norm(g_destiny, DIM);
        }

        H.step++;

        /* Hebbian update of positional profile (from Leo 2.3) */
        {
            float eta = 0.01f / (1.0f + (float)H.dist_profile_updates * 0.001f);
            int h_start = (H.ctx_len > 8) ? H.ctx_len - 8 : 0;
            for (int ci = h_start; ci < H.ctx_len; ci++) {
                int cid = H.context[ci];
                int dd = H.ctx_len - 1 - ci;
                if (dd >= DIST_PROFILE_LEN) continue;
                for (int ii = 0; ii < H.cooc.n; ii++) {
                    if (H.cooc.src[ii] == cid && H.cooc.dst[ii] == next &&
                        H.cooc.count[ii] > 0.0f) {
                        float r = clampf(H.cooc.count[ii] * 0.1f, 0, 0.05f);
                        H.dist_profile[dd] += eta * r;
                        int tc = token_class(cid);
                        H.class_mod[tc] += eta * 0.5f * r;
                        break;
                    }
                }
            }
            H.dist_profile_updates++;
            for (int dd = 0; dd < DIST_PROFILE_LEN; dd++)
                H.dist_profile[dd] = clampf(H.dist_profile[dd], 0.01f, 2.0f);
            for (int cc = 0; cc < TOKEN_CLASSES; cc++)
                H.class_mod[cc] = clampf(H.class_mod[cc], 0.5f, 2.0f);
        }
    }

    /* if we didn't hit exact syllable count, try to fill */
    if (syllables < target_syllables) {
        int needed = target_syllables - syllables;
        /* find a word with exactly the right syllable count */
        int candidates[MAX_VOCAB];
        int nc = 0;
        for (int i = 0; i < vocab && nc < MAX_VOCAB; i++) {
            if (count_syllables(H.vocab.words[i]) == needed) {
                /* check not recently used */
                int recent = 0;
                for (int c = 0; c < H.ctx_len; c++)
                    if (H.context[c] == i) { recent = 1; break; }
                if (!recent) candidates[nc++] = i;
            }
        }
        if (nc > 0) {
            int pick = candidates[(int)(randf() * nc) % nc];
            const char *word = H.vocab.words[pick];
            int wlen = strlen(word);
            if (pos + wlen + 2 < max_len) {
                if (pos > 0) out[pos++] = ' ';
                memcpy(out + pos, word, wlen);
                pos += wlen;
                out[pos] = '\0';
            }
        }
    }

    free(logits);
    return pos;
}

typedef struct {
    char line1[256];
    char line2[256];
    char line3[256];
} Haiku;

static void generate_haiku(Haiku *haiku) {
    generate_line(haiku->line1, sizeof(haiku->line1), 5);
    generate_line(haiku->line2, sizeof(haiku->line2), 7);
    generate_line(haiku->line3, sizeof(haiku->line3), 5);
}

/* ===================================================================
 * PROCESS INPUT -- the full pipeline
 * =================================================================== */

static const char *term_names[] = {
    "B:chain", "H:resonance", "F:prophecy", "A:destiny", "T:trauma"
};
static const char *vel_names[] = {
    "WALK", "RUN", "STOP", "BREATHE", "UP", "DOWN"
};
static const char *season_names[] = {
    "spring", "summer", "autumn", "winter"
};

static void process_input(const char *input, Haiku *haiku) {
    H.dissonance = compute_dissonance(input);
    ingest(input);
    if (H.dissonance > 0.7f)
        H.trauma_level = clampf(H.trauma_level + H.dissonance * 0.1f, 0, 1);
    auto_velocity();
    apply_velocity();
    season_step();
    update_metrics();
    chamber_update();
    enforce_laws();
    generate_haiku(haiku);
    H.conv_count++;
}

/* ===================================================================
 * DISPLAY
 * =================================================================== */

static void display_haiku(const Haiku *haiku) {
    printf("\n");
    printf("  %-14s  d=%.2f  T=%.2f  %s  %s\n",
           term_names[H.dominant_term], H.dissonance, H.tau,
           vel_names[H.velocity], season_names[H.season]);
    printf("\n");
    printf("  %s\n", haiku->line1);
    printf("  %s\n", haiku->line2);
    printf("  %s\n", haiku->line3);
    printf("\n");
    printf("  debt=%.2f  res=%.2f  ent=%.2f  emg=%.2f\n",
           H.debt, H.resonance, H.entropy, H.emergence);
    printf("\n");
}

/* ===================================================================
 * INIT -- bootstrap the organism
 * =================================================================== */

static void haiku_init(void) {
    memset(&H, 0, sizeof(H));
    H.alpha = ALPHA;
    H.beta = BETA;
    H.gamma_d = GAMMA_D;
    H.tau = TAU_BASE;
    H.velocity = VEL_WALK;
    H.alpha_mod = 1.0f;
    H.beta_mod = 1.0f;
    H.gamma_mod = 1.0f;
    H.tau_mod = 1.0f;
    H.vel_temp = 1.0f;

    for (int d = 0; d < DIST_PROFILE_LEN; d++)
        H.dist_profile[d] = powf(0.9f, (float)d);
    for (int c = 0; c < TOKEN_CLASSES; c++)
        H.class_mod[c] = 1.0f;
    H.dist_profile_updates = 0;

    rng_state = (uint64_t)time(NULL);

    /* seed vocabulary */
    for (int i = 0; SEED_WORDS[i] != NULL; i++)
        vocab_add(&H.vocab, SEED_WORDS[i]);

    /* bootstrap: ingest seed words as connected text */
    char bootstrap[8192] = {0};
    int bpos = 0;
    for (int i = 0; SEED_WORDS[i] != NULL && bpos < 8000; i++) {
        int wlen = strlen(SEED_WORDS[i]);
        if (bpos + wlen + 2 >= 8000) break;
        if (bpos > 0) bootstrap[bpos++] = ' ';
        memcpy(bootstrap + bpos, SEED_WORDS[i], wlen);
        bpos += wlen;
    }
    bootstrap[bpos] = '\0';
    ingest(bootstrap);

    printf("[haiku] bootstrapped. vocab=%d cooc=%d bigrams=%d\n",
           H.vocab.n_words, H.cooc.n, H.bigrams.n);
}

/* ===================================================================
 * WEB SERVER -- POSIX sockets, zero dependencies
 *
 * GET /            -> serves haiku.html
 * POST /api/chat   -> JSON: haiku lines + metrics
 *
 * ./haiku --web [port]
 * =================================================================== */

#ifndef HAIKU_NO_WEB
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define WEB_BUF     8192
#define WEB_PORT    3002

static int json_escape(const char *src, char *dst, int max) {
    int p = 0;
    for (const char *s = src; *s && p < max - 6; s++) {
        switch (*s) {
            case '"':  dst[p++] = '\\'; dst[p++] = '"';  break;
            case '\\': dst[p++] = '\\'; dst[p++] = '\\'; break;
            case '\n': dst[p++] = '\\'; dst[p++] = 'n';  break;
            case '\t': dst[p++] = '\\'; dst[p++] = 't';  break;
            case '\r': break;
            default:   dst[p++] = *s;
        }
    }
    dst[p] = '\0';
    return p;
}

static const char *find_body(const char *req) {
    const char *p = strstr(req, "\r\n\r\n");
    return p ? p + 4 : NULL;
}

static int extract_text(const char *body, char *out, int max) {
    const char *p = strstr(body, "\"text\"");
    if (!p) return 0;
    p = strchr(p + 6, '"');
    if (!p) return 0;
    p++;
    int i = 0;
    while (*p && *p != '"' && i < max - 1) {
        if (*p == '\\' && *(p+1)) {
            p++;
            switch (*p) {
                case 'n': out[i++] = ' '; break;
                case 't': out[i++] = ' '; break;
                case '"': out[i++] = '"'; break;
                case '\\': out[i++] = '\\'; break;
                default: out[i++] = *p;
            }
        } else {
            out[i++] = *p;
        }
        p++;
    }
    out[i] = '\0';
    return i;
}

static void serve_file(int fd, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        const char *r = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
        write(fd, r, strlen(r));
        return;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    fclose(f);

    char header[256];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %ld\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n", sz);
    write(fd, header, strlen(header));
    write(fd, buf, sz);
    free(buf);
}

static void handle_chat(int fd, const char *req) {
    const char *body = find_body(req);
    char text[MAX_LINE] = {0};
    if (body) extract_text(body, text, sizeof(text));

    if (strlen(text) == 0) {
        const char *r = "HTTP/1.1 400 Bad Request\r\n"
            "Content-Length: 18\r\n\r\n{\"error\":\"no text\"}";
        write(fd, r, strlen(r));
        return;
    }

    Haiku haiku;
    process_input(text, &haiku);

    char l1[512], l2[512], l3[512];
    json_escape(haiku.line1, l1, sizeof(l1));
    json_escape(haiku.line2, l2, sizeof(l2));
    json_escape(haiku.line3, l3, sizeof(l3));

    char json[4096];
    int jlen = snprintf(json, sizeof(json),
        "{"
        "\"line1\":\"%s\","
        "\"line2\":\"%s\","
        "\"line3\":\"%s\","
        "\"dominant\":%d,"
        "\"dominant_name\":\"%s\","
        "\"dissonance\":%.3f,"
        "\"tau\":%.3f,"
        "\"debt\":%.3f,"
        "\"resonance\":%.3f,"
        "\"entropy\":%.3f,"
        "\"emergence\":%.3f,"
        "\"trauma\":%.3f,"
        "\"velocity\":\"%s\","
        "\"season\":\"%s\","
        "\"chambers\":{\"fear\":%.3f,\"love\":%.3f,\"rage\":%.3f,"
        "\"void\":%.3f,\"flow\":%.3f,\"complex\":%.3f},"
        "\"vocab\":%d,\"step\":%d"
        "}",
        l1, l2, l3,
        H.dominant_term, term_names[H.dominant_term],
        H.dissonance, H.tau,
        H.debt, H.resonance, H.entropy, H.emergence,
        H.trauma_level,
        vel_names[H.velocity], season_names[H.season],
        H.chamber[CH_FEAR], H.chamber[CH_LOVE], H.chamber[CH_RAGE],
        H.chamber[CH_VOID], H.chamber[CH_FLOW], H.chamber[CH_COMPLEX],
        H.vocab.n_words, H.step
    );

    char header[256];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "\r\n", jlen);
    write(fd, header, strlen(header));
    write(fd, json, jlen);
}

static void handle_options(int fd) {
    const char *r =
        "HTTP/1.1 204 No Content\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "\r\n";
    write(fd, r, strlen(r));
}

static void haiku_web(int port, const char *html_path) {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { perror("socket"); return; }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(server); return;
    }
    listen(server, 8);

    printf("[haiku] web server on http://localhost:%d\n", port);
    fflush(stdout);

    while (1) {
        int client = accept(server, NULL, NULL);
        if (client < 0) continue;

        char buf[WEB_BUF] = {0};
        int n = read(client, buf, sizeof(buf) - 1);
        if (n <= 0) { close(client); continue; }

        if (strncmp(buf, "OPTIONS", 7) == 0) {
            handle_options(client);
        } else if (strncmp(buf, "GET / ", 6) == 0 || strncmp(buf, "GET /index", 10) == 0) {
            serve_file(client, html_path);
        } else if (strstr(buf, "POST /api/chat") != NULL) {
            handle_chat(client, buf);
        } else if (strncmp(buf, "GET /favicon", 12) == 0) {
            const char *r = "HTTP/1.1 204 No Content\r\n\r\n";
            write(client, r, strlen(r));
        } else {
            const char *r = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
            write(client, r, strlen(r));
        }

        close(client);
    }
}
#endif /* HAIKU_NO_WEB */

/* ===================================================================
 * MAIN
 *
 * ./haiku          -- REPL mode
 * ./haiku --web    -- HTTP server on port 3002
 * ./haiku --web N  -- HTTP server on port N
 * =================================================================== */

int main(int argc, char **argv) {
    printf("\n");
    printf("  HAiKU — Constraint-Driven Language Emergence\n");
    printf("  p(x|Phi) = softmax((B + a*H + b*F + g*A + T) / tau)\n");
    printf("  1000 words. 5-7-5 syllables. pressure creates form.\n");
    printf("\n");

    haiku_init();

#ifndef HAIKU_NO_WEB
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--web") == 0) {
            int port = WEB_PORT;
            if (i + 1 < argc) port = atoi(argv[i + 1]);
            if (port <= 0) port = WEB_PORT;
            haiku_web(port, "haiku.html");
            return 0;
        }
    }
#endif

    printf("  speaks only in haiku. users hate it or love it.\n");
    printf("  there is no middle ground.\n");
    printf("\n");

    char line[MAX_LINE];

    while (1) {
        printf("you> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;

        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';
        if (len == 0) continue;

        if (strcmp(line, "/quit") == 0) break;

        if (strcmp(line, "/stats") == 0) {
            printf("\n  vocab=%d cooc=%d bigrams=%d step=%d conv=%d\n",
                   H.vocab.n_words, H.cooc.n, H.bigrams.n,
                   H.step, H.conv_count);
            printf("  debt=%.3f trauma=%.3f momentum=%.3f\n",
                   H.debt, H.trauma_level, H.momentum);
            printf("  a=%.3f b=%.3f g=%.3f tau=%.3f\n",
                   H.alpha, H.beta, H.gamma_d, H.tau);
            printf("  chambers: fear=%.2f love=%.2f rage=%.2f void=%.2f "
                   "flow=%.2f complex=%.2f\n",
                   H.chamber[CH_FEAR], H.chamber[CH_LOVE], H.chamber[CH_RAGE],
                   H.chamber[CH_VOID], H.chamber[CH_FLOW], H.chamber[CH_COMPLEX]);
            printf("  velocity=%s season=%s(%.2f)\n",
                   vel_names[H.velocity], season_names[H.season],
                   H.season_phase);
            printf("\n");
            continue;
        }

        Haiku haiku;
        process_input(line, &haiku);
        display_haiku(&haiku);
    }

    printf("[haiku] constraint holds the form.\n");
    return 0;
}
