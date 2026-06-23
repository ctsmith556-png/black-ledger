#!/usr/bin/env python3
# Black Ledger - regenerate ONLY engine_loop.wav (the diesel idle) into
# tools/audio_raw/, WITHOUT touching the other wavs there - so it won't clobber
# the real SFX you've already dropped in (mg_fire, mg_hit, missile_*, ...).
# Then run tools/unreal/import_audio.py to rebuild the bank.
#
#   python tools/gen_engine.py
#
# (Imports the shared DSP from gen_placeholder_audio.py; importing does NOT
#  regenerate the full set - main() only runs when that file is run directly.)

import gen_placeholder_audio as g

g.write_wav("engine_loop", g.engine_idle())
print("wrote engine_loop.wav (diesel idle) ->", g.OUT)
