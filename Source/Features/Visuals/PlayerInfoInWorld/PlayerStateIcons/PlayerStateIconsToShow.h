#pragma once

#include <Utils/TypeBitFlags.h>

struct DefuseIconPanel;
struct HostagePickupPanel;
struct HostageRescuePanel;
struct BlindedIconPanel;

using PlayerStateIconsToShow = TypeBitFlags<DefuseIconPanel, HostagePickupPanel, HostageRescuePanel, BlindedIconPanel>;
