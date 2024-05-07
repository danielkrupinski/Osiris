#pragma once

#include <Utils/TypeBitFlags.h>

struct DefuseIconPanel;
struct HostagePickupPanel;

using PlayerStateIconsToShow = TypeBitFlags<DefuseIconPanel, HostagePickupPanel>;
