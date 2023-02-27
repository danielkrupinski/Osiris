#pragma once

#include <array>

#include <Config/ResetConfigurator.h>
#include <CSGO/Constants/ConVarNames.h>
#include <CSGO/Constants/FrameStage.h>
#include <CSGO/Cvar.h>
#include <CSGO/Functions.h>
#include <CSGO/PODs/ConVar.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include <RetSpoof/FunctionInvoker.h>

class SkyboxChanger {
public:
    bool enabled;
    int skybox;

    static constexpr std::array skyboxList{ "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_hr_aztec" };

    SkyboxChanger(csgo::Cvar cvar, FunctionInvoker<csgo::R_LoadNamedSkys> loadSky) : cvar{ cvar }, loadSky{ loadSky }
    {
    }

    void run(csgo::FrameStage stage) noexcept
    {
        if (stage != csgo::FrameStage::RENDER_START && stage != csgo::FrameStage::RENDER_END)
            return;

        if (!loadSky)
            return;

        if (stage == csgo::FrameStage::RENDER_START && enabled && static_cast<std::size_t>(skybox) < skyboxList.size()) {
            loadSky(skyboxList[skybox]);
        } else {
            static const auto sv_skyname = cvar.findVar(csgo::sv_skyname);
            loadSky(sv_skyname->string);
        }
    }

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Enabled", enabled).def(false);
        configurator("Skybox", skybox)
            .def(0)
            .loadString([this](std::string_view s) {
                if (const auto it = std::ranges::find(skyboxList, s); it != skyboxList.end())
                    skybox = static_cast<int>(std::distance(skyboxList.begin(), it));
            })
            .save([this]() { return skyboxList[skybox]; });
    }

private:
    csgo::Cvar cvar;
    FunctionInvoker<csgo::R_LoadNamedSkys> loadSky;
};
