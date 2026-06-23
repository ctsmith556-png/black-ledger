#!/usr/bin/env python3
# Black Ledger - trim WAV(s) to a max length with a short fade-out (so the cut
# doesn't click). Handy for one-shots that came out too long - e.g. a rapid-fire
# weapon sound that must be short so 10 shots/sec don't smear.
# Stdlib only; handles mono/stereo, 16/24/32-bit signed PCM. Overwrites in place.
#
#   python tools/trim_wav.py 0.4 tools\audio_raw\mg_fire.wav tools\audio_raw\mg_hit.wav

import sys
import wave


def trim(path, seconds, fade_ms=8.0):
    with wave.open(path, "rb") as w:
        ch, sw, fr, n = w.getnchannels(), w.getsampwidth(), w.getframerate(), w.getnframes()
        data = w.readframes(n)
    keep = min(n, int(seconds * fr))
    frame = ch * sw
    out = bytearray(data[:keep * frame])
    # linear fade-out over the last `fade` frames to avoid a click at the cut
    fade = min(keep, int(fade_ms / 1000.0 * fr))
    for i in range(keep - fade, keep):
        g = (keep - i) / max(fade, 1)
        base = i * frame
        for c in range(ch):
            off = base + c * sw
            s = int.from_bytes(out[off:off + sw], "little", signed=True)
            s = int(s * g)
            out[off:off + sw] = s.to_bytes(sw, "little", signed=True)
    with wave.open(path, "wb") as w:
        w.setnchannels(ch); w.setsampwidth(sw); w.setframerate(fr)
        w.writeframes(bytes(out))
    print(f"trimmed: {path} -> {keep / fr:.2f}s")


def main(args):
    if len(args) < 2:
        print("usage: python tools/trim_wav.py <seconds> <file.wav> ...")
        return
    seconds = float(args[0])
    for p in args[1:]:
        try:
            trim(p, seconds)
        except Exception as e:
            print(f"FAILED {p}: {e}")


if __name__ == "__main__":
    main(sys.argv[1:])
