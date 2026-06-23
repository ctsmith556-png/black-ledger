#!/usr/bin/env python3
# Black Ledger - regenerate Source/BlackLedger/UI/BLVehicleBios.h from the Design
# Bible. Pulls each contestant's Backstory + Intro Cinematic storyboard, sanitizes
# to ASCII (curly quotes/dashes -> plain), and writes the C++ data table the roster
# Biography/Intro screens read. Run from the repo root:
#   python3 tools/gen_vehicle_bios.py
# Requires python-docx (pip install python-docx).

import docx

DOC = 'docs/DesignBible_v0.3.docx'
OUT = 'Source/BlackLedger/UI/BLVehicleBios.h'

d = docx.Document(DOC)
P = d.paragraphs


def block(start, end):
    out = {}
    cur = None
    for i in range(start, end):
        p = P[i]
        t = p.text.strip()
        s = p.style.name if p.style else ''
        if not t:
            continue
        if s == 'Heading 3':
            cur = t
            out[cur] = []
        elif cur is not None:
            out[cur].append(t)
    return out


def san(s):
    rep = {'’': "'", '‘': "'", '“': '"', '”': '"',
           '—': ' - ', '–': '-', '≈': '~', '…': '...',
           ' ': ' ', 'é': 'e'}
    for k, v in rep.items():
        s = s.replace(k, v)
    s = s.replace('\\', '\\\\').replace('"', '\\"')
    return ''.join(ch for ch in s if ord(ch) < 128)


bible = {'Surgeon': 57, 'Hollow': 75, 'Antoinette': 93, 'Warden': 111, 'Pup': 129,
         'Crucible': 147, 'Cartographer': 165, 'Lien': 183, 'Hemlock': 201,
         'Shepherd': 219, 'Specter': 237, 'Refuse': 255, 'Bride': 273, 'Hunter': 291,
         'Photographer': 309, 'Vault': 327, '__END__': 348}
order = ['Surgeon', 'Hollow', 'Antoinette', 'Warden', 'Pup', 'Crucible', 'Cartographer',
         'Lien', 'Hemlock', 'Shepherd', 'Specter', 'Refuse', 'Bride', 'Hunter',
         'Photographer', 'Vault', '__END__']


def get(name):
    idx = bible[name]
    nxt = bible[order[order.index(name) + 1]]
    quote = P[idx + 1].text.strip()
    b = block(idx, nxt)
    bio = '\\n\\n'.join(san(x) for x in b.get('Backstory', []))
    intro = ''
    ending = ''
    for k, v in b.items():
        if k.startswith('Intro Cinematic'):
            intro = ' '.join(san(x) for x in v)
        elif k.startswith('Ending Cinematic'):
            ending = ' '.join(san(x) for x in v)
    return san(quote), bio, intro, ending


# roster numbering from CLAUDE.md (authoritative for the game / asset folders)
roster = [('01', 'Surgeon', 'THE SURGEON'), ('02', 'Antoinette', 'MISS ANTOINETTE'),
          ('03', 'Hollow', 'HOLLOW'), ('04', 'Warden', 'THE WARDEN'), ('05', 'Pup', 'PUP'),
          ('06', 'Crucible', 'CRUCIBLE'), ('07', 'Cartographer', 'THE CARTOGRAPHER'),
          ('08', 'Lien', 'THE LIEN'), ('09', 'Hemlock', 'HEMLOCK'),
          ('10', 'Shepherd', 'THE SHEPHERD'), ('11', 'Specter', 'SPECTER'),
          ('12', 'Refuse', 'REFUSE'), ('13', 'Bride', 'THE BRIDE'),
          ('14', 'Hunter', 'THE HUNTER'), ('15', 'Photographer', 'THE PHOTOGRAPHER'),
          ('16', 'Vault', 'VAULT')]

# Foundryman boss (section 6.1)
fb = block(451, 467)
fb_quote = san(P[452].text.strip())
fb_bio = ' '.join(san(x) for x in fb.get('Backstory', []))
fb_intro = ''
fb_ending = ''
for k, v in fb.items():
    if k.startswith('Fight Intro'):
        fb_intro = ' '.join(san(x) for x in v)
    elif k.startswith('On Defeat'):
        fb_ending = ' '.join(san(x) for x in v)

L = []
L.append('// Black Ledger - vehicle/boss biography data (auto-generated from')
L.append('// docs/DesignBible_v0.3.docx by tools/gen_vehicle_bios.py - do not hand-edit).')
L.append('// Backstory + Intro storyboard per contestant; the Intro text is the')
L.append('// placeholder shown on "Watch Intro" until the real motion-comics exist.')
L.append('#pragma once')
L.append('#include "CoreMinimal.h"')
L.append('')
L.append('struct FBLVehicleBio')
L.append('{')
L.append('\tconst TCHAR* Key;          // matches save unlock names / asset folder')
L.append('\tconst TCHAR* Number;       // roster index label')
L.append('\tconst TCHAR* DisplayName;')
L.append('\tconst TCHAR* Tagline;')
L.append('\tconst TCHAR* Bio;')
L.append('\tconst TCHAR* Intro;     // ~30s intro storyboard (motion-comic beats)')
L.append('\tconst TCHAR* Ending;    // ~60-90s ending storyboard')
L.append('\tbool bBoss;')
L.append('};')
L.append('')
L.append('// --- launch roster (16), in roster order ---')
L.append('static const FBLVehicleBio GBLLaunchBios[] =')
L.append('{')
for num, key, disp in roster:
    q, bio, intro, ending = get(key)
    L.append('\t{')
    L.append(f'\t\tTEXT("{key}"), TEXT("{num}"), TEXT("{disp}"),')
    L.append(f'\t\tTEXT("{q}"),')
    L.append(f'\t\tTEXT("{bio}"),')
    L.append(f'\t\tTEXT("{intro}"),')
    L.append(f'\t\tTEXT("{ending}"),')
    L.append('\t\tfalse')
    L.append('\t},')
L.append('};')
L.append('')
L.append('// --- Collector bosses (shown on the roster only once unlocked) ---')
L.append('static const FBLVehicleBio GBLBossBios[] =')
L.append('{')
L.append('\t{')
L.append('\t\tTEXT("Foundryman"), TEXT("B1"), TEXT("THE FOUNDRYMAN - \\"TAP\\""),')
L.append(f'\t\tTEXT("{fb_quote}"),')
L.append(f'\t\tTEXT("{fb_bio}"),')
L.append(f'\t\tTEXT("{fb_intro}"),')
L.append(f'\t\tTEXT("{fb_ending}"),')
L.append('\t\ttrue')
L.append('\t},')
L.append('};')
L.append('')

open(OUT, 'w').write('\n'.join(L) + '\n')
print('wrote', OUT, '-', len(roster), 'launch +', 1, 'boss')
