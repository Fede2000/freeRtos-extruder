#include "Extruder.h"
#include <AccelStepper.h>
#include "configuration.h"


AccelStepper extruder1(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
int ESet = 1000;