#pragma once

namespace csgo
{

enum class EconItemFlags {
    PurchasedFromStore = (1 << 1),
    NonEconomy = (1 << 3),
    InXrayScanner = (1 << 4)
};

}
