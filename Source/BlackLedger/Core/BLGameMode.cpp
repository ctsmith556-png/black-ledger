// Black Ledger - game mode

#include "BLGameMode.h"
#include "Vehicles/BLCombatVehicle.h"

ABLGameMode::ABLGameMode()
{
	DefaultPawnClass = ABLCombatVehicle::StaticClass();
}
