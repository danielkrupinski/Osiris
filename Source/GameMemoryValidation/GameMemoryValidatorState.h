#pragma once

#include "MemoryValidationStatus.h"

struct GameMemoryValidatorState {
    MemoryValidationStatus soundChannelInfo1SizeStatus{MemoryValidationStatus::Unvalidated};
};
