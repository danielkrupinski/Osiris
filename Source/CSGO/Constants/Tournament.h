#pragma once

#include <cstdint>

namespace csgo
{

enum class Tournament : std::uint8_t {
    DreamHack2013 = 1,
    EmsOneKatowice2014 = 3,
    EslOneCologne2014,
    DreamHack2014,
    EslOneKatowice2015,
    EslOneCologne2015,
    DreamHackClujNapoka2015,
    MlgColumbus2016,
    EslOneCologne2016,
    EleagueAtlanta2017,
    PglKrakow2017,
    EleagueBoston2018,
    FaceitLondon2018,
    IemKatowice2019,
    PglStockholm2021 = 18,
    PglAntwerp2022,
    IemRio2022
};

}
