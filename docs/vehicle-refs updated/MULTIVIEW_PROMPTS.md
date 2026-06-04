# Black Ledger — Meshy Multi-View Prompt Pack

**Purpose:** generate clean, consistent front / side / rear views of each vehicle to feed Meshy 6 **Image to 3D -> Multi-view**. These are the *quality* path; the cropped panels in `/multiview/` are the *quick-test* path.

## How to use

1. Generate these in the **same image tool you used for the dossier sheets**, so the style matches.
2. **Keep the design consistent across the three views.** The reliable way: use that vehicle's existing SIDE render from the dossier as an **image / style reference (img2img)** at roughly **0.3-0.5 strength** - low enough to change the camera angle, high enough to keep the same vehicle. Generate the Side first (it should match the reference almost exactly), then Front, then Rear.
3. **Lock framing across all three:** same background, same lighting, same zoom, vehicle centered and the same size in each frame. Mismatched scale/zoom is the #1 cause of bad multi-view reconstructions.
4. Export square or 4:3, **>=1024 px**.
5. In Meshy 6: **Image to 3D -> turn on Multi-view -> upload Front, then Side, then Rear** (front-first is clearest; order doesn't change the result). Standard model, Image Enhancement on, License **Private**.

## Append this style suffix to every prompt

> Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## Do / Don't

- **Do:** one vehicle per image, plain gray background, identical scale and lighting across views.
- **Don't:** include any text/labels/callouts, put multiple angles in one image, change zoom or lighting between views, or use a busy background.

> **Note on the bosses (B1-B10):** they're overpowered showpieces - you can push them ~80-120k tris on remesh (fewer on screen at once). For the Foundryman/Jackknife molten glow, the Headliner light strings, and the Auditor's swirling papers, model the vehicle clean and add the glow/particles as **VFX in Unreal**, not baked into the mesh.

---

## 01 — The Surgeon "The Theater"

*1980s American box-type ambulance, dirty off-white body with faded red crosses, heavy rust and grime, reinforced steel push-bumper, roof light bar, boxy patient module, Twisted-Metal horror tone.*

- **Front:** 1980s American box-type ambulance, dirty off-white body with faded red crosses, heavy rust and grime, reinforced steel push-bumper, roof light bar, boxy patient module, Twisted-Metal horror tone, front three-quarter view showing the grille, dual round headlights, reinforced push-bumper and windshield. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** 1980s American box-type ambulance, dirty off-white body with faded red crosses, heavy rust and grime, reinforced steel push-bumper, roof light bar, boxy patient module, Twisted-Metal horror tone, direct left-side profile showing the full length of the boxy patient module, side door, red cross, wheelbase. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** 1980s American box-type ambulance, dirty off-white body with faded red crosses, heavy rust and grime, reinforced steel push-bumper, roof light bar, boxy patient module, Twisted-Metal horror tone, rear three-quarter view showing the rear loading doors, red cross, tail lights and bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 02 — Miss Antoinette "Forever Yours"

*1957 Cadillac Eldorado convertible, faded glossy rose-pink over decay, tall sharp tailfins, heavy cracked-and-pitted chrome, floral mildew bloom in the seams, hood ornament shaped like a young woman.*

- **Front:** 1957 Cadillac Eldorado convertible, faded glossy rose-pink over decay, tall sharp tailfins, heavy cracked-and-pitted chrome, floral mildew bloom in the seams, hood ornament shaped like a young woman, front three-quarter view showing the wide chrome Cadillac grille, quad headlights, hood ornament. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** 1957 Cadillac Eldorado convertible, faded glossy rose-pink over decay, tall sharp tailfins, heavy cracked-and-pitted chrome, floral mildew bloom in the seams, hood ornament shaped like a young woman, direct left-side profile showing the long low profile, chrome trim, open cabin and the tall rear tailfins. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** 1957 Cadillac Eldorado convertible, faded glossy rose-pink over decay, tall sharp tailfins, heavy cracked-and-pitted chrome, floral mildew bloom in the seams, hood ornament shaped like a young woman, rear three-quarter view showing the dramatic tailfins, bullet tail lights, chrome bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 03 — Hollow "Pyre"

*1970s pumper fire engine, entirely blackened and scorched end to end, heat-warped ladder on top, ember-scarred metal, a single gleaming polished chrome bell that is pristine, charred red undertones.*

- **Front:** 1970s pumper fire engine, entirely blackened and scorched end to end, heat-warped ladder on top, ember-scarred metal, a single gleaming polished chrome bell that is pristine, charred red undertones, front three-quarter view showing the scorched grille and cab face, the polished chrome bell, blackened bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** 1970s pumper fire engine, entirely blackened and scorched end to end, heat-warped ladder on top, ember-scarred metal, a single gleaming polished chrome bell that is pristine, charred red undertones, direct left-side profile showing the full pumper body, warped roof ladder, pump panels and ember scars. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** 1970s pumper fire engine, entirely blackened and scorched end to end, heat-warped ladder on top, ember-scarred metal, a single gleaming polished chrome bell that is pristine, charred red undertones, rear three-quarter view showing the rear pump housing, hose bed, charred tail. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 04 — The Warden "The Cage"

*Armored prison transport van, blackened riveted steel body, welded steel mesh over the windows, barred rear compartment, roof-mounted chain launcher, reinforced ram bumper with grille guard.*

- **Front:** armored prison transport van, blackened riveted steel body, welded steel mesh over the windows, barred rear compartment, roof-mounted chain launcher, reinforced ram bumper with grille guard, front three-quarter view showing the reinforced ram bumper, grille guard, mesh-covered windshield. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** armored prison transport van, blackened riveted steel body, welded steel mesh over the windows, barred rear compartment, roof-mounted chain launcher, reinforced ram bumper with grille guard, direct left-side profile showing the slab armored side, mesh windows, riveted panels, roof chain launcher. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** armored prison transport van, blackened riveted steel body, welded steel mesh over the windows, barred rear compartment, roof-mounted chain launcher, reinforced ram bumper with grille guard, rear three-quarter view showing the barred rear prisoner doors, hanging chains, tail. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 05 — Pup "Sundae"

*1980s step-van ice cream truck, faded pastel mint-and-cream paint, peeling uncanny cartoon smiley decals with slightly wrong faces, warped roof loudspeaker, frosted serving window, spiked push bumper, rust.*

- **Front:** 1980s step-van ice cream truck, faded pastel mint-and-cream paint, peeling uncanny cartoon smiley decals with slightly wrong faces, warped roof loudspeaker, frosted serving window, spiked push bumper, rust, front three-quarter view showing the boxy cab face, welded grille bars, uncanny smiley decal, spiked bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** 1980s step-van ice cream truck, faded pastel mint-and-cream paint, peeling uncanny cartoon smiley decals with slightly wrong faces, warped roof loudspeaker, frosted serving window, spiked push bumper, rust, direct left-side profile showing the serving window, smiley decals, warped roof loudspeaker, side panels. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** 1980s step-van ice cream truck, faded pastel mint-and-cream paint, peeling uncanny cartoon smiley decals with slightly wrong faces, warped roof loudspeaker, frosted serving window, spiked push bumper, rust, rear three-quarter view showing the rear door with internal bar, rust streaks, bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 06 — Crucible "Bessemer"

*Massive six-wheel industrial steel-mill slag hauler, welded and bolted mill-steel armor plates, a huge front ladle bucket dripping molten orange slag, heavy rust and soot, armored cab with heat-resistant glass.*

- **Front:** massive six-wheel industrial steel-mill slag hauler, welded and bolted mill-steel armor plates, a huge front ladle bucket dripping molten orange slag, heavy rust and soot, armored cab with heat-resistant glass, front three-quarter view showing the huge front ladle bucket dripping molten slag, armored cab. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** massive six-wheel industrial steel-mill slag hauler, welded and bolted mill-steel armor plates, a huge front ladle bucket dripping molten orange slag, heavy rust and soot, armored cab with heat-resistant glass, direct left-side profile showing the long six-wheel armored chassis, bolted plates, ladle arm. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** massive six-wheel industrial steel-mill slag hauler, welded and bolted mill-steel armor plates, a huge front ladle bucket dripping molten orange slag, heavy rust and soot, armored cab with heat-resistant glass, rear three-quarter view showing the rear of the hauler chassis, exhaust stacks, heavy bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 07 — The Cartographer "True North"

*1972 Plymouth Fury station wagon, the body completely covered in layered paper topographic maps and charts in wax and tape, roof rack loaded with survey tubes, antennae and instruments, reinforced ram bar.*

- **Front:** 1972 Plymouth Fury station wagon, the body completely covered in layered paper topographic maps and charts in wax and tape, roof rack loaded with survey tubes, antennae and instruments, reinforced ram bar, front three-quarter view showing the reinforced map-table ram bar, quad headlights, map-covered hood. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** 1972 Plymouth Fury station wagon, the body completely covered in layered paper topographic maps and charts in wax and tape, roof rack loaded with survey tubes, antennae and instruments, reinforced ram bar, direct left-side profile showing the long wagon profile plastered in maps, roof rack of instruments. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** 1972 Plymouth Fury station wagon, the body completely covered in layered paper topographic maps and charts in wax and tape, roof rack loaded with survey tubes, antennae and instruments, reinforced ram bar, rear three-quarter view showing the rear tailgate, spare canisters, map-covered rear, tail lights. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 08 — The Lien "Arrears"

*Heavy black repossession tow truck, a hydraulic repossession boom with a large weighted hook, draped trophies (a grandfather clock on the flank, a child's bicycle, wedding rings on a chain, a wheelchair bolted to the roof), grimy industrial steel.*

- **Front:** heavy black repossession tow truck, a hydraulic repossession boom with a large weighted hook, draped trophies (a grandfather clock on the flank, a child's bicycle, wedding rings on a chain, a wheelchair bolted to the roof), grimy industrial steel, front three-quarter view showing the grille guard, headlight cages, reinforced tow hitch and ram plow. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** heavy black repossession tow truck, a hydraulic repossession boom with a large weighted hook, draped trophies (a grandfather clock on the flank, a child's bicycle, wedding rings on a chain, a wheelchair bolted to the roof), grimy industrial steel, direct left-side profile showing the cab and the long rear tow boom, hung trophies, wheelchair on roof. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** heavy black repossession tow truck, a hydraulic repossession boom with a large weighted hook, draped trophies (a grandfather clock on the flank, a child's bicycle, wedding rings on a chain, a wheelchair bolted to the roof), grimy industrial steel, rear three-quarter view showing the rear tow frame, weighted hook, chains and drag points. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 09 — Hemlock "Wormwood"

*Municipal water-district tanker truck, faded white-and-green, a large oval tank weeping black-green toxic slurry, choked in creeping kudzu vines and pale fungal growth, corroded valves and pipes.*

- **Front:** municipal water-district tanker truck, faded white-and-green, a large oval tank weeping black-green toxic slurry, choked in creeping kudzu vines and pale fungal growth, corroded valves and pipes, front three-quarter view showing the truck cab, corroded grille, vines creeping over the front. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** municipal water-district tanker truck, faded white-and-green, a large oval tank weeping black-green toxic slurry, choked in creeping kudzu vines and pale fungal growth, corroded valves and pipes, direct left-side profile showing the long oval tank, weeping seams, kudzu growth, discharge pipes. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** municipal water-district tanker truck, faded white-and-green, a large oval tank weeping black-green toxic slurry, choked in creeping kudzu vines and pale fungal growth, corroded valves and pipes, rear three-quarter view showing the rear tank cap, discharge manifold, dripping valves. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 10 — The Shepherd "The Hollow Light"

*Haunted rusted congregation bus, faded hand-painted scripture along the sides, windows fogged from inside with handprints, a roof bell and a welded guardrail-steel cross, barbed wire wrapped around the roofline and grille.*

- **Front:** haunted rusted congregation bus, faded hand-painted scripture along the sides, windows fogged from inside with handprints, a roof bell and a welded guardrail-steel cross, barbed wire wrapped around the roofline and grille, front three-quarter view showing the bus front, barbed-wire-wrapped grille, fogged windshield, welded cross. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** haunted rusted congregation bus, faded hand-painted scripture along the sides, windows fogged from inside with handprints, a roof bell and a welded guardrail-steel cross, barbed wire wrapped around the roofline and grille, direct left-side profile showing the long bus body, faded scripture, fogged handprint windows, roof bell. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** haunted rusted congregation bus, faded hand-painted scripture along the sides, windows fogged from inside with handprints, a roof bell and a welded guardrail-steel cross, barbed wire wrapped around the roofline and grille, rear three-quarter view showing the rear of the bus, emergency door, tail lights, barbed wire. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 11 — Specter "Overwatch"

*Militarized ground-control surveillance truck, matte tan armored box body, windows that are dark monitor screens with faint reticles, a roof bristling with antenna arrays, a satellite dome and a retractable Ku-band mast, rugged military tires.*

- **Front:** militarized ground-control surveillance truck, matte tan armored box body, windows that are dark monitor screens with faint reticles, a roof bristling with antenna arrays, a satellite dome and a retractable Ku-band mast, rugged military tires, front three-quarter view showing the armored cab face, monitor-screen windshield, reinforced bumper, mast. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** militarized ground-control surveillance truck, matte tan armored box body, windows that are dark monitor screens with faint reticles, a roof bristling with antenna arrays, a satellite dome and a retractable Ku-band mast, rugged military tires, direct left-side profile showing the armored box side, monitor windows, roof antenna arrays and dome. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** militarized ground-control surveillance truck, matte tan armored box body, windows that are dark monitor screens with faint reticles, a roof bristling with antenna arrays, a satellite dome and a retractable Ku-band mast, rugged military tires, rear three-quarter view showing the rear service access, ladder, antenna array, tow points. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 12 — Refuse "Route 7"

*Municipal rear-loader garbage truck, filthy gray-green paint, a dented rear compactor hopper leaking dark ooze, an amber roof hazard beacon, the city seal scrubbed half-off, heavy industrial wear.*

- **Front:** municipal rear-loader garbage truck, filthy gray-green paint, a dented rear compactor hopper leaking dark ooze, an amber roof hazard beacon, the city seal scrubbed half-off, heavy industrial wear, front three-quarter view showing the boxy cab, pitted steel grille, bar-guarded mirrors, tow hooks. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** municipal rear-loader garbage truck, filthy gray-green paint, a dented rear compactor hopper leaking dark ooze, an amber roof hazard beacon, the city seal scrubbed half-off, heavy industrial wear, direct left-side profile showing the long body, scrubbed city seal, amber beacon, side panels. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** municipal rear-loader garbage truck, filthy gray-green paint, a dented rear compactor hopper leaking dark ooze, an amber roof hazard beacon, the city seal scrubbed half-off, heavy industrial wear, rear three-quarter view showing the rear-loader compactor hopper, grinding teeth, dripping ooze. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 13 — The Bride "Honeymoon"

*Decrepit 1980s stretch white wedding limousine, a long three-axle wheelbase, faded yellowed paint with rust and cracked chrome, a dead bouquet and wilted ribbons tied on, JUST MARRIED faded on the rear, dragging tin cans and a single white shoe.*

- **Front:** decrepit 1980s stretch white wedding limousine, a long three-axle wheelbase, faded yellowed paint with rust and cracked chrome, a dead bouquet and wilted ribbons tied on, JUST MARRIED faded on the rear, dragging tin cans and a single white shoe, front three-quarter view showing the limo front, cracked chrome grille, dead bouquet, faded paint. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** decrepit 1980s stretch white wedding limousine, a long three-axle wheelbase, faded yellowed paint with rust and cracked chrome, a dead bouquet and wilted ribbons tied on, JUST MARRIED faded on the rear, dragging tin cans and a single white shoe, direct left-side profile showing the very long three-axle stretch profile, dark windows, wilted ribbons. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** decrepit 1980s stretch white wedding limousine, a long three-axle wheelbase, faded yellowed paint with rust and cracked chrome, a dead bouquet and wilted ribbons tied on, JUST MARRIED faded on the rear, dragging tin cans and a single white shoe, rear three-quarter view showing the rear with faded JUST MARRIED, dragging tin cans and a shoe. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 14 — The Hunter "First Light"

*Mud-caked 1980s square-body off-road hunting pickup, a front winch with cable, off-road tires wired across the front grille, a tarp lashed over a bulky shape on the hood, a blaze-orange cap on the mirror, a rifle in a rear rack.*

- **Front:** mud-caked 1980s square-body off-road hunting pickup, a front winch with cable, off-road tires wired across the front grille, a tarp lashed over a bulky shape on the hood, a blaze-orange cap on the mirror, a rifle in a rear rack, front three-quarter view showing the front winch, tires wired across the grille, mud-caked bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** mud-caked 1980s square-body off-road hunting pickup, a front winch with cable, off-road tires wired across the front grille, a tarp lashed over a bulky shape on the hood, a blaze-orange cap on the mirror, a rifle in a rear rack, direct left-side profile showing the pickup profile, hood tarp cargo, blaze-orange cap, rifle rack, bed. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** mud-caked 1980s square-body off-road hunting pickup, a front winch with cable, off-road tires wired across the front grille, a tarp lashed over a bulky shape on the hood, a blaze-orange cap on the mirror, a rifle in a rear rack, rear three-quarter view showing the open truck bed, rifle rack, tailgate, off-road tires. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 15 — The Photographer "Exposure"

*Rigged full-size press sedan from the late 1960s, dark black-and-maroon body stained with darkroom chemicals and rust, a homemade roof rig of large flashbulbs wired together, cameras and tripods heaped in back, a red darkroom glow inside.*

- **Front:** rigged full-size press sedan from the late 1960s, dark black-and-maroon body stained with darkroom chemicals and rust, a homemade roof rig of large flashbulbs wired together, cameras and tripods heaped in back, a red darkroom glow inside, front three-quarter view showing the sedan front, flashbulb rig glare, press bumper, red glow in cab. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** rigged full-size press sedan from the late 1960s, dark black-and-maroon body stained with darkroom chemicals and rust, a homemade roof rig of large flashbulbs wired together, cameras and tripods heaped in back, a red darkroom glow inside, direct left-side profile showing the long sedan profile, roof flashbulb rig, chemical-stained panels. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** rigged full-size press sedan from the late 1960s, dark black-and-maroon body stained with darkroom chemicals and rust, a homemade roof rig of large flashbulbs wired together, cameras and tripods heaped in back, a red darkroom glow inside, rear three-quarter view showing the rear with heaped cameras, trunk gear, flash rig, tail lights. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## 16 — Vault "Iron Page"

*Jet-black armored cash-transport truck, slab-sided riveted steel, no markings (ground off), blacked-out armored windows, a heavy reinforced front ram prow with serrated steel teeth, sinister and imposing.*

- **Front:** jet-black armored cash-transport truck, slab-sided riveted steel, no markings (ground off), blacked-out armored windows, a heavy reinforced front ram prow with serrated steel teeth, sinister and imposing, front three-quarter view showing the heavy front ram prow with serrated steel teeth, blacked-out windshield. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** jet-black armored cash-transport truck, slab-sided riveted steel, no markings (ground off), blacked-out armored windows, a heavy reinforced front ram prow with serrated steel teeth, sinister and imposing, direct left-side profile showing the slab-sided riveted armored body, blacked-out windows, wheel armor. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** jet-black armored cash-transport truck, slab-sided riveted steel, no markings (ground off), blacked-out armored windows, a heavy reinforced front ram prow with serrated steel teeth, sinister and imposing, rear three-quarter view showing the sealed rear service door, riveted armor, heavy bumper. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B1 — The Foundryman "Cupola" (BOSS)

*A torpedo ladle car boss vehicle: a giant elongated oval crucible body of forged riveted steel mounted on heavy armored crawler tracks, glowing incandescent molten seams, a rear cage module holding a man-shaped molten figure, soot-black with orange glow.*

- **Front:** a torpedo ladle car boss vehicle: a giant elongated oval crucible body of forged riveted steel mounted on heavy armored crawler tracks, glowing incandescent molten seams, a rear cage module holding a man-shaped molten figure, soot-black with orange glow, front three-quarter view showing the front of the oval crucible, slag-shedding deflector lip, heat-bloom vents, crawler tracks. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** a torpedo ladle car boss vehicle: a giant elongated oval crucible body of forged riveted steel mounted on heavy armored crawler tracks, glowing incandescent molten seams, a rear cage module holding a man-shaped molten figure, soot-black with orange glow, direct left-side profile showing the full elongated oval crucible shell, molten seams, rail guides, armored crawler tracks, rear cage. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** a torpedo ladle car boss vehicle: a giant elongated oval crucible body of forged riveted steel mounted on heavy armored crawler tracks, glowing incandescent molten seams, a rear cage module holding a man-shaped molten figure, soot-black with orange glow, rear three-quarter view showing the rear cage module with the glowing man-shaped figure, track housing. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B2 — Curfew "Order" (BOSS)

*An armored riot-control truck boss (BearCat-style), heavy welded ballistic riot shields across the front with small vision ports, a roof PA tower with multiple speakers and a red/blue light bar, a roof-mounted water cannon, dark paramilitary livery.*

- **Front:** an armored riot-control truck boss (BearCat-style), heavy welded ballistic riot shields across the front with small vision ports, a roof PA tower with multiple speakers and a red/blue light bar, a roof-mounted water cannon, dark paramilitary livery, front three-quarter view showing the welded riot-shield front, vision ports, ORDER stencil, light bar. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** an armored riot-control truck boss (BearCat-style), heavy welded ballistic riot shields across the front with small vision ports, a roof PA tower with multiple speakers and a red/blue light bar, a roof-mounted water cannon, dark paramilitary livery, direct left-side profile showing the armored side, riot shields, PA tower and water cannon on the roof. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** an armored riot-control truck boss (BearCat-style), heavy welded ballistic riot shields across the front with small vision ports, a roof PA tower with multiple speakers and a red/blue light bar, a roof-mounted water cannon, dark paramilitary livery, rear three-quarter view showing the rear armored doors, overlapping shield panels, tow points. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B3 — Condemned "Eminent Domain" (BOSS)

*A demolition wrecking-crane truck boss, an industrial lattice-boom crane carrying a massive rusted wrecking ball and a clamshell grapple, dust-caked armor with yellow hazard stripes and torn caution tape, a slat-armored operator cab, a front demolition blade.*

- **Front:** a demolition wrecking-crane truck boss, an industrial lattice-boom crane carrying a massive rusted wrecking ball and a clamshell grapple, dust-caked armor with yellow hazard stripes and torn caution tape, a slat-armored operator cab, a front demolition blade, front three-quarter view showing the armored operator cab, front demolition blade, hazard stripes, hooded headlights. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** a demolition wrecking-crane truck boss, an industrial lattice-boom crane carrying a massive rusted wrecking ball and a clamshell grapple, dust-caked armor with yellow hazard stripes and torn caution tape, a slat-armored operator cab, a front demolition blade, direct left-side profile showing the full rig: lattice boom, wrecking ball, clamshell grapple, heavy wheels. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** a demolition wrecking-crane truck boss, an industrial lattice-boom crane carrying a massive rusted wrecking ball and a clamshell grapple, dust-caked armor with yellow hazard stripes and torn caution tape, a slat-armored operator cab, a front demolition blade, rear three-quarter view showing the rear power unit, twin diesel stacks, counterweight, boom base. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B4 — Jackknife "Long Way Down" (BOSS)

*A black cab-over semi truck boss hauling a battered tanker trailer, a chrome bull-bar grille with a skull emblem and a spiked ram guard, aggressive low narrow eye-like headlights, heat-stressed soot-covered metal, molten ember seams.*

- **Front:** a black cab-over semi truck boss hauling a battered tanker trailer, a chrome bull-bar grille with a skull emblem and a spiked ram guard, aggressive low narrow eye-like headlights, heat-stressed soot-covered metal, molten ember seams, front three-quarter view showing the chrome bull-bar grille with skull emblem, narrow eye headlights, spiked ram. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** a black cab-over semi truck boss hauling a battered tanker trailer, a chrome bull-bar grille with a skull emblem and a spiked ram guard, aggressive low narrow eye-like headlights, heat-stressed soot-covered metal, molten ember seams, direct left-side profile showing the full cab-over tractor plus the long tanker trailer, molten vent seams. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** a black cab-over semi truck boss hauling a battered tanker trailer, a chrome bull-bar grille with a skull emblem and a spiked ram guard, aggressive low narrow eye-like headlights, heat-stressed soot-covered metal, molten ember seams, rear three-quarter view showing the rear of the tanker trailer, spreader bars, drip rails, mud flaps. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B5 — The Sexton "Plot" (BOSS)

*A mausoleum hearse boss: a 1930s heavy-duty hearse chassis with gothic stone-and-bronze mausoleum architecture built into the body (arched crypt windows, fluted columns, funeral ironwork), a large green-tarnished bronze tolling bell on the roof, decayed.*

- **Front:** a mausoleum hearse boss: a 1930s heavy-duty hearse chassis with gothic stone-and-bronze mausoleum architecture built into the body (arched crypt windows, fluted columns, funeral ironwork), a large green-tarnished bronze tolling bell on the roof, decayed, front three-quarter view showing the hearse front, ornate grille from the statues, headlights, bronze trim. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** a mausoleum hearse boss: a 1930s heavy-duty hearse chassis with gothic stone-and-bronze mausoleum architecture built into the body (arched crypt windows, fluted columns, funeral ironwork), a large green-tarnished bronze tolling bell on the roof, decayed, direct left-side profile showing the long hearse body with crypt windows, fluted columns, roof bell. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** a mausoleum hearse boss: a 1930s heavy-duty hearse chassis with gothic stone-and-bronze mausoleum architecture built into the body (arched crypt windows, fluted columns, funeral ironwork), a large green-tarnished bronze tolling bell on the roof, decayed, rear three-quarter view showing the rear crypt doors of the hearse, ironwork, funeral ornamentation. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B6 — Undertow "Deadrise" (BOSS)

*An amphibious salvage dredge boss, a rust-bound heavy hull riding on pontoon-tracks, a hydraulic dredge grapple arm, a salvage winch, a diver's-hull observation cab with a round port, dripping wet barnacled riveted steel, drowned-harbor tone.*

- **Front:** an amphibious salvage dredge boss, a rust-bound heavy hull riding on pontoon-tracks, a hydraulic dredge grapple arm, a salvage winch, a diver's-hull observation cab with a round port, dripping wet barnacled riveted steel, drowned-harbor tone, front three-quarter view showing the diver's-hull observation cab with round port, dredge grapple arm, hull prow. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** an amphibious salvage dredge boss, a rust-bound heavy hull riding on pontoon-tracks, a hydraulic dredge grapple arm, a salvage winch, a diver's-hull observation cab with a round port, dripping wet barnacled riveted steel, drowned-harbor tone, direct left-side profile showing the full salvage hull on pontoon-tracks, grapple arm, winch, roof bell. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** an amphibious salvage dredge boss, a rust-bound heavy hull riding on pontoon-tracks, a hydraulic dredge grapple arm, a salvage winch, a diver's-hull observation cab with a round port, dripping wet barnacled riveted steel, drowned-harbor tone, rear three-quarter view showing the rear of the salvage hull, reinforced stern, winch drum, tracks. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B7 — Lodestone "Lodestone" (BOSS)

*A magnet scrapyard-crane boss, a giant round electromagnet hanging from an armored boom dragging cars and scrap, heavy-duty industrial tracks, an armored cab, a baler-crusher compactor intake, rusted scrapyard steel.*

- **Front:** a magnet scrapyard-crane boss, a giant round electromagnet hanging from an armored boom dragging cars and scrap, heavy-duty industrial tracks, an armored cab, a baler-crusher compactor intake, rusted scrapyard steel, front three-quarter view showing the armored cab, baler-crusher compactor intake, hooded headlights. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** a magnet scrapyard-crane boss, a giant round electromagnet hanging from an armored boom dragging cars and scrap, heavy-duty industrial tracks, an armored cab, a baler-crusher compactor intake, rusted scrapyard steel, direct left-side profile showing the full rig: armored cab, boom and giant electromagnet, industrial tracks. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** a magnet scrapyard-crane boss, a giant round electromagnet hanging from an armored boom dragging cars and scrap, heavy-duty industrial tracks, an armored cab, a baler-crusher compactor intake, rusted scrapyard steel, rear three-quarter view showing the rear compactor and traction unit, hydraulics, counterweight. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B8 — The Headliner "The Big Top" (BOSS)

*A twisted carnival parade-float boss, garish purple-and-gold decayed circus livery, an animatronic clown skull bust on the front, a calliope pipe-organ, strings of dim lights, marquee lettering, an oversized parade-float chassis, sinister.*

- **Front:** a twisted carnival parade-float boss, garish purple-and-gold decayed circus livery, an animatronic clown skull bust on the front, a calliope pipe-organ, strings of dim lights, marquee lettering, an oversized parade-float chassis, sinister, front three-quarter view showing the animatronic clown skull bust, marquee front, light strings, ornate grille. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** a twisted carnival parade-float boss, garish purple-and-gold decayed circus livery, an animatronic clown skull bust on the front, a calliope pipe-organ, strings of dim lights, marquee lettering, an oversized parade-float chassis, sinister, direct left-side profile showing the long parade-float body, calliope pipes, gilded trim, light strings. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** a twisted carnival parade-float boss, garish purple-and-gold decayed circus livery, an animatronic clown skull bust on the front, a calliope pipe-organ, strings of dim lights, marquee lettering, an oversized parade-float chassis, sinister, rear three-quarter view showing the rear of the float, calliope pipe cluster, gilded ornamentation. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B9 — The Alienist "Convulsion" (BOSS)

*A mobile ECT (electroshock) asylum truck boss, a boxy armored medical body, a treatment gurney rig with restraint straps, arcing electrode cables with electric-blue arcs, a single large lensed reflective headlight, an insulated coil bed, rusted institutional steel.*

- **Front:** a mobile ECT (electroshock) asylum truck boss, a boxy armored medical body, a treatment gurney rig with restraint straps, arcing electrode cables with electric-blue arcs, a single large lensed reflective headlight, an insulated coil bed, rusted institutional steel, front three-quarter view showing the cab face, single large lensed reflective headlight, conical shell, grille. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** a mobile ECT (electroshock) asylum truck boss, a boxy armored medical body, a treatment gurney rig with restraint straps, arcing electrode cables with electric-blue arcs, a single large lensed reflective headlight, an insulated coil bed, rusted institutional steel, direct left-side profile showing the boxy medical body, gurney rig and restraints, arc cables along the side. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** a mobile ECT (electroshock) asylum truck boss, a boxy armored medical body, a treatment gurney rig with restraint straps, arcing electrode cables with electric-blue arcs, a single large lensed reflective headlight, an insulated coil bed, rusted institutional steel, rear three-quarter view showing the rear with the insulated coil bed, electrode assembly, conical shell. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

## B10 — The Auditor "The Closing Entry" (FINAL BOSS)

*The final-boss shifting black car: a long low sleek 1940s-50s fastback sedan, a pure jet-black seamless body, a swept roofline, a glowing red slit at the front, minimal hidden ballistic ports, supernatural and imposing (swirling papers are a VFX layer, omit from the model).*

- **Front:** the final-boss shifting black car: a long low sleek 1940s-50s fastback sedan, a pure jet-black seamless body, a swept roofline, a glowing red slit at the front, minimal hidden ballistic ports, supernatural and imposing (swirling papers are a VFX layer, omit from the model), front three-quarter view showing the front with the glowing red slit, blacked-out windshield, sweeping fenders. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Side (left profile):** the final-boss shifting black car: a long low sleek 1940s-50s fastback sedan, a pure jet-black seamless body, a swept roofline, a glowing red slit at the front, minimal hidden ballistic ports, supernatural and imposing (swirling papers are a VFX layer, omit from the model), direct left-side profile showing the long low fastback profile, swept roofline, seamless black body. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.

- **Rear:** the final-boss shifting black car: a long low sleek 1940s-50s fastback sedan, a pure jet-black seamless body, a swept roofline, a glowing red slit at the front, minimal hidden ballistic ports, supernatural and imposing (swirling papers are a VFX layer, omit from the model), rear three-quarter view showing the fastback rear, hidden ports, smooth black decklid, tail. Single vehicle, centered and fully in frame, plain neutral mid-gray studio background, soft even lighting, no text, no labels, no callout lines, no logos, photorealistic PBR materials, game-ready, consistent design across all three views.
