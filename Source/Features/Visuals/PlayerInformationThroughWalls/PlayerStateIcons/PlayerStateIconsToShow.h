#pragma once

#include <Utils/TypeBitFlags.h>

struct DefuseIconPanel;
struct HostagePickupPanel;
struct HostageRescuePanel;

using PlayerStateIconsToShow = TypeBitFlags<DefuseIconPanel, HostagePickupPanel, HostageRescuePanel>;
