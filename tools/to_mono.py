#!/usr/bin/env python3
# Black Ledger - downmix stereo WAVs to mono (UE needs mono sources for true 3D
# attenuation). ElevenLabs / many libraries export stereo; run this on the
# positional one-shots (mg_fire, mg_hit, impacts) before dropping them in
# tools/audio_raw/.  Stdlib only; handles 16/24/32-bit signed PCM WAV.
#
#   python tools/to_mono.py path\to\mg_fire.wav path\to\mg_hit.wav
#   python tools/to_mono.py path\to\folder            (does every .wav inside)
#
# Writes the mono file back to the SAME path (originals are overwritten - keep a
# copy if you want the stereo source). Already-mono files are left untouched.

import os
import sys
import wave


def to_mono(path):
    with wave.open(path, "rb") as w:
        ch = w.getnchannels()
        sw = w.getsampwidth()
        fr = w.getframerate()
        n = w.getnframes()
        data = w.readframes(n)
    if ch == 1:
        print(f"skip (already mono): {path}")
        return
    out = bytearray()
    frame = ch * sw
    for i in range(0, len(data), frame):
        acc = 0
        for c in range(ch):
            b = data[i + c * sw:i + c * sw + sw]
            acc += int.from_bytes(b, "little", signed=True)
        avg = acc // ch
        # clamp to the signed range for this width
        lo = -(1 << (sw * 8 - 1))
        hi = (1 << (sw * 8 - 1)) - 1
        avg = max(lo, min(hi, avg))
        out += avg.to_bytes(sw, "little", signed=True)
    with wave.open(path, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(sw)
        w.setframerate(fr)
        w.writeframes(bytes(out))
    print(f"mono: {path}  ({ch}ch -> 1ch, {sw*8}-bit {fr}Hz)")


def main(args):
    if not args:
        print("usage: python tools/to_mono.py <file.wav | folder> ...")
        return
    targets = []
    for a in args:
        if os.path.isdir(a):
            targets += [os.path.join(a, f) for f in os.listdir(a) if f.lower().endswith(".wav")]
        else:
            targets.append(a)
    for t in targets:
        try:
            to_mono(t)
        except Exception as e:
            print(f"FAILED {t}: {e}  (try Audacity for non-PCM/24-bit edge cases)")


if __name__ == "__main__":
    main(sys.argv[1:])
