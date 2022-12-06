#pragma once

#include <array>

#include <Config/ResetConfigurator.h>
#include <RetSpoof/FastcallFunctionInvoker.h>
#include <SDK/Constants/ConVarNames.h>
#include <SDK/Constants/FrameStage.h>
#include <SDK/Cvar.h>
#include <SDK/PODs/ConVar.h>

class SkyboxChanger {
public:
    int skybox;

    static constexpr std::array skyboxList{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_hr_aztec" };

    SkyboxChanger(Cvar cvar, FastcallFunctionInvoker<void, const char*> loadSky) : cvar{ cvar }, loadSky{ loadSky }
    {
        ResetConfigurator configurator;
        configure(configurator);
    }

    void run(csgo::FrameStage stage) noexcept
    {
        if (stage != csgo::FrameStage::RENDER_START && stage != csgo::FrameStage::RENDER_END)
            return;

        if (!loadSky)
            return;

        if (stage == csgo::FrameStage::RENDER_START && skybox > 0 && static_cast<std::size_t>(skybox) < skyboxList.size()) {
            loadSky(skyboxList[skybox]);
        } else {
            static const auto sv_skyname = cvar.findVar(csgo::sv_skyname);
            loadSky(sv_skyname->string);
        }
    }

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Skybox", skybox).def(0);
    }

private:
    Cvar cvar;
    FastcallFunctionInvoker<void, const char*> loadSky;
};

[[nodiscard]] inline SkyboxChanger createSkyboxChanger(Cvar cvar, const helpers::PatternFinder& enginePatternFinder)
{
#if IS_WIN32()
    return SkyboxChanger{ cvar, FastcallFunctionInvoker<void, const char*>{ retSpoofGadgets->engine, enginePatternFinder("\xE8????\x84\xC0\x74\x2D\xA1").add(1).relativeToAbsolute().get() } };
#elif IS_LINUX()
    return SkyboxChanger{ cvar, FastcallFunctionInvoker<void, const char*>{ retSpoofGadgets->engine, enginePatternFinder("\xE8????\x84\xC0\x74\xAB").add(1).relativeToAbsolute().get() } };
#endif
}
