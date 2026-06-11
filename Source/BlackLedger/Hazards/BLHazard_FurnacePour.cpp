// Black Ledger - the Mill furnace pour

#include "BLHazard_FurnacePour.h"

ABLHazard_FurnacePour::ABLHazard_FurnacePour()
{
	// Bible 5.1 numbers; the pour zone is the dashed ring around the pit
	CooldownSeconds = 90.f;
	TelegraphSeconds = 5.f;
	ActiveSeconds = 4.f;
	ZoneRadius = 1500.f;
	DamagePerSecond = 55.f;   // standing in a pour is a fast, readable mistake
}
