#pragma once

#include <array>

#include <JsonForward.h>

#include "Structs.h"

namespace inventory
{

[[nodiscard]] Glove gloveFromJson(const json& j);
[[nodiscard]] SouvenirPackage souvenirPackageFromJson(const json& j);
[[nodiscard]] Music musicFromJson(const json& j);
[[nodiscard]] ServiceMedal serviceMedalFromJson(const json& j);
[[nodiscard]] Graffiti graffitiFromJson(const json& j);
[[nodiscard]] std::array<Skin::Sticker, 5> skinStickersFromJson(const json& j);
[[nodiscard]] Skin skinFromJson(const json& j);
[[nodiscard]] std::array<Agent::Patch, 5> agentPatchesFromJson(const json& j);

}
