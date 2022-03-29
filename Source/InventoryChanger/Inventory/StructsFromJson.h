#pragma once

#include <JsonForward.h>

#include "Structs.h"

namespace inventory
{

[[nodiscard]] Glove gloveFromJson(const json& j);
[[nodiscard]] SouvenirPackage souvenirPackageFromJson(const json& j);
[[nodiscard]] Music musicFromJson(const json& j);
[[nodiscard]] ServiceMedal serviceMedalFromJson(const json& j);
[[nodiscard]] Graffiti graffitiFromJson(const json& j);

}
