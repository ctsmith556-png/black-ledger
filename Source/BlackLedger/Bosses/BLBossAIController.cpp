// Black Ledger - boss combat driver

#include "BLBossAIController.h"

ABLBossAIController::ABLBossAIController()
{
	EngagementRange = 100000.f;   // the Collector always knows where you are
	MinFightRange = 0.f;          // never peels - he IS the collision hazard
	OrbitAngleDeg = 40.f;         // shallow orbit: mostly bears down on you
	PursueRange = 3500.f;
	DisengageChance = 0.f;        // a Collector does not get distracted
	bSeekPickups = false;         // no errands
	FireRange = 9000.f;           // the vessel-mounted gun reaches further
}
