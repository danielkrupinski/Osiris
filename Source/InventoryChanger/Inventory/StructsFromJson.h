#pragma once

#include <array>

#include <JsonForward.h>

#include "Structs.h"

namespace inventory_changer::inventory
{

[[nodiscard]] Gloves glovesFromJson(const json& j);
[[nodiscard]] SouvenirPackage souvenirPackageFromJson(const json& j);
[[nodiscard]] Music musicFromJson(const json& j);
[[nodiscard]] ServiceMedal serviceMedalFromJson(const json& j);
[[nodiscard]] Graffiti graffitiFromJson(const json& j);
[[nodiscard]] std::array<Skin::Sticker, 5> skinStickersFromJson(const json& j);
[[nodiscard]] Skin skinFromJson(const json& j);
[[nodiscard]] std::array<Agent::Patch, 5> agentPatchesFromJson(const json& j);
[[nodiscard]] Agent agentFromJson(const json& j);
[[nodiscard]] StorageUnit storageUnitFromJson(const json& j);

}
