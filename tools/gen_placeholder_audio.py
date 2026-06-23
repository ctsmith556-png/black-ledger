#!/usr/bin/env python3
# Black Ledger - generate placeholder audio so the wired audio system is AUDIBLE
# before real sound design. Writes 16-bit mono WAVs to tools/audio_raw/;
# tools/unreal/import_audio.py imports them and builds the bank.
#
# PROCEDURAL stand-ins (stdlib only) tuned for the game's grim/realistic tone -
# filtered-noise ballistics, sub-bass explosions with pitch-drop + grit, a
# combustion-pulse engine idle, a lub-dub heartbeat - NOT final assets. The point
# is routing + feel; swap for real SFX via the same import step.
#
# Run from the repo root:  python3 tools/gen_placeholder_audio.py

import math, os, random, struct, wave

SR = 44100
OUT = os.path.join(os.path.dirname(__file__), "audio_raw")
os.makedirs(OUT, exist_ok=True)
random.seed(7)


def noise(n):
    return [random.uniform(-1.0, 1.0) for _ in range(n)]


def lowpass(x, a):
    y = [0.0] * len(x); p = 0.0
    for i, s in enumerate(x):
        p += a * (s - p); y[i] = p
    return y


def highpass(x, a):
    lp = lowpass(x, a)
    return [x[i] - lp[i] for i in range(len(x))]


def decay_mult(t):
    return math.exp(-1.0 / max(t * SR, 1.0))


def decay_env(n, t):
    m = decay_mult(t); e = [0.0] * n; v = 1.0
    for i in range(n):
        e[i] = v; v *= m
    return e


def apply_env(x, e):
    n = min(len(x), len(e))
    return [x[i] * e[i] for i in range(n)]


def sweep(f0, f1, dur, curve=1.0):
    n = int(SR * dur); out = [0.0] * n; ph = 0.0
    for i in range(n):
        t = i / max(n - 1, 1)
        f = f0 + (f1 - f0) * (t ** curve)
        ph += 2.0 * math.pi * f / SR
        out[i] = math.sin(ph)
    return out


def tone(freq, dur):
    n = int(SR * dur)
    return [math.sin(2.0 * math.pi * freq * i / SR) for i in range(n)]


def soft_clip(x, drive):
    return [math.tanh(s * drive) for s in x]


def mix(layers):
    n = max(len(s) for s, _ in layers); out = [0.0] * n
    for s, g in layers:
        for i in range(len(s)):
            out[i] += s[i] * g
    return out


def reverb_tail(x, dur=0.25, decay=0.35, delays=(1009, 1523, 2131)):
    n = len(x) + int(SR * dur); out = list(x) + [0.0] * (n - len(x))
    for d in delays:
        for i in range(d, n):
            out[i] += out[i - d] * decay
    return out


def normalize(x, peak=0.92):
    m = max((abs(s) for s in x), default=0.0)
    if m < 1e-6:
        return x
    g = peak / m
    return [s * g for s in x]


def write_wav(name, samples):
    samples = normalize(samples)
    with wave.open(os.path.join(OUT, name + ".wav"), "w") as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
        frames = bytearray()
        for s in samples:
            frames += struct.pack("<h", int(max(-1.0, min(1.0, s)) * 32767))
        w.writeframes(bytes(frames))


def gunshot(dur=0.11):
    n = int(SR * dur)
    crack = apply_env(highpass(noise(n), 0.55), decay_env(n, 0.010))
    body = apply_env(lowpass(noise(n), 0.16), decay_env(n, 0.035))
    thump = apply_env(sweep(120, 55, dur), decay_env(n, 0.030))
    return soft_clip(mix([(crack, 0.9), (body, 0.6), (thump, 0.9)]), 1.7)


def metal_hit(dur=0.16):
    n = int(SR * dur)
    click = apply_env(highpass(noise(n), 0.6), decay_env(n, 0.006))
    rings = []
    for f in (1700, 3100, 5300, 7400):
        rings.append((apply_env(tone(f, dur), decay_env(n, random.uniform(0.05, 0.12))),
                      random.uniform(0.12, 0.22)))
    return soft_clip(mix([(click, 0.7)] + rings), 1.3)


def explosion(dur=0.9, sub=1.0, grit=3.2):
    n = int(SR * dur)
    crack = apply_env(highpass(noise(n), 0.5), decay_env(n, 0.012))
    boom = apply_env(sweep(130, 32, dur, curve=0.5), decay_env(n, dur * 0.45))
    rumble = apply_env(lowpass(noise(n), 0.05), decay_env(n, dur * 0.6))
    mid = apply_env(lowpass(highpass(noise(n), 0.2), 0.4), decay_env(n, dur * 0.3))
    body = soft_clip(mix([(crack, 0.8), (boom, sub), (rumble, 0.7), (mid, 0.5)]), grit)
    return reverb_tail(body, dur=0.22, decay=0.3)


def vehicle_death():
    base = explosion(dur=1.2, sub=1.1, grit=3.6)
    groan = apply_env(mix([(sweep(150, 70, 1.0), 1.0), (sweep(153, 68, 1.0), 1.0)]),
                      decay_env(int(SR * 1.0), 0.6))
    groan = groan + [0.0] * (len(base) - len(groan))
    return mix([(base, 1.0), (groan, 0.35)])


def missile_launch(dur=0.6):
    n = int(SR * dur)
    rocket = apply_env(lowpass(noise(n), 0.12),
                       [min(1.0, i / (SR * 0.12)) * 0.9 for i in range(n)])
    bp = highpass(lowpass(noise(n), 0.35), 0.08)
    swell = [0.4 + 0.6 * (i / n) for i in range(n)]
    return soft_clip(mix([(rocket, 0.8), (apply_env(bp, swell), 0.5), (tone(70, dur), 0.4)]), 1.6)


def special(dur=0.7):
    n = int(SR * dur)
    riser = apply_env(highpass(lowpass(noise(n), 0.3), 0.05), [(i / n) ** 1.5 for i in range(n)])
    drn = apply_env(mix([(sweep(180, 320, dur), 1.0), (sweep(181, 360, dur), 1.0)]),
                    [0.3 + 0.7 * (i / n) for i in range(n)])
    return soft_clip(mix([(riser, 0.5), (drn, 0.4)]), 1.4)


def pickup(dur=0.18):
    n = int(SR * dur)
    chunk = apply_env(lowpass(noise(n), 0.25), decay_env(n, 0.04))
    low = apply_env(tone(160, dur), decay_env(n, 0.05))
    return soft_clip(mix([(chunk, 0.7), (low, 0.5)]), 1.4)


def ui_tick(dur=0.05):
    n = int(SR * dur)
    return apply_env(lowpass(noise(n), 0.2), decay_env(n, 0.012))


def engine_idle(pulses=9, rate=18.0):
    # lumpy DIESEL idle: irregular low combustion pulses + a deep saturated rumble bed
    # + mechanical hiss, heavily low-passed so it reads as a rumble (not a tonal fan).
    # Loops cleanly: integer pulses; bed fundamentals at integer cycles over the buffer.
    dur = pulses / rate; n = int(SR * dur); out = [0.0] * n
    pulse_len = int(SR * 0.05)
    for k in range(pulses):
        jit = random.uniform(-0.004, 0.004)                      # per-cylinder timing wobble
        start = int((k / rate + jit) * SR) % n
        amp = random.uniform(0.7, 1.15)
        thump = apply_env(sweep(70, 36, 0.05), decay_env(pulse_len, 0.020))             # combustion thud
        knock = apply_env(lowpass(noise(pulse_len), 0.10), decay_env(pulse_len, 0.010)) # mechanical knock
        for i in range(pulse_len):
            out[(start + i) % n] += (thump[i] * 0.85 + knock[i] * 0.5) * amp
    for f, g in ((36.0, 0.45), (72.0, 0.16)):                    # deep rumble bed (integer cycles)
        for i in range(n):
            out[i] += math.sin(2.0 * math.pi * f * i / SR) * g
    body = lowpass(noise(n), 0.05)                               # broadband mechanical hiss
    for i in range(n):
        out[i] += body[i] * 0.10
    out = lowpass(out, 0.45)                                     # tame highs -> rumble, not whine
    return soft_clip(out, 1.6)                                   # grind


def heartbeat(dur=1.0):
    n = int(SR * dur); out = [0.0] * n

    def thump(at, freq, length, gain):
        ln = int(SR * length)
        t = apply_env(tone(freq, length), decay_env(ln, length * 0.4))
        b = apply_env(lowpass(noise(ln), 0.08), decay_env(ln, length * 0.3))
        start = int(SR * at)
        for i in range(ln):
            if start + i < n:
                out[start + i] += (t[i] * 0.8 + b[i] * 0.3) * gain

    thump(0.00, 58, 0.10, 1.0)
    thump(0.16, 50, 0.12, 0.8)
    return out


def drone(freqs, dur=3.0, vol=0.16, noisy=0.06):
    n = int(SR * dur); out = [0.0] * n
    for f in freqs:
        for i in range(n):
            out[i] += math.sin(2.0 * math.pi * f * i / SR) * vol
    if noisy > 0:
        body = lowpass(noise(n), 0.02)
        for i in range(n):
            lfo = 0.5 + 0.5 * math.sin(2.0 * math.pi * 0.2 * i / SR)
            out[i] += body[i] * noisy * lfo
    fade = int(SR * 0.06)
    for i in range(fade):
        g = i / fade
        out[i] *= g; out[n - 1 - i] *= g
    return out


def main():
    files = {
        "mg_fire": gunshot(),
        "mg_hit": metal_hit(),
        "missile_launch": missile_launch(),
        "missile_impact": explosion(dur=0.75, sub=0.95, grit=3.0),
        "power_impact": explosion(dur=1.0, sub=1.1, grit=3.6),
        "explosion": explosion(dur=0.9, sub=1.0, grit=3.3),
        "vehicle_death": vehicle_death(),
        "special": special(),
        "pickup": pickup(),
        "ui_select": ui_tick(),
        "engine_loop": engine_idle(),
        "heartbeat": heartbeat(),
        "music_menu": drone([55, 82.4, 110], vol=0.14),
        "music_combat": drone([65.4, 98, 130.8], vol=0.16),
        "music_boss1": drone([49, 73.4, 98], vol=0.18),
        "music_boss2": drone([49, 58.3, 87.3, 116.5], vol=0.2),
        "music_boss3": drone([49, 55, 77.8, 116.5, 155.6], vol=0.22, noisy=0.1),
        "music_victory": drone([65.4, 98, 130.8, 164.8], vol=0.16),
        "music_death": drone([43.7, 58.3, 61.7], vol=0.16, noisy=0.1),
    }
    for name, samples in files.items():
        write_wav(name, samples)
    print(f"wrote {len(files)} placeholder wavs to {OUT}")


if __name__ == "__main__":
    main()
