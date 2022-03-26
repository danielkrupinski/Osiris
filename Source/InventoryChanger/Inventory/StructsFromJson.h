#pragma once

#include <JsonForward.h>

#include "Structs.h"

namespace inventory
{

[[nodiscard]] Glove gloveFromJson(const json& j);
[[nodiscard]] SouvenirPackage souvenirPackageFromJson(const json& j);

}
