#pragma once

#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/GameEvent.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/ViewRenderBeams.h>
#include <ConfigStructs.h>

class BulletTracers {
public:
    BulletTracers(csgo::ViewRenderBeams viewRenderBeams, csgo::EntityList entityList, csgo::Engine engine, csgo::GlobalVars* globalVars)
        : viewRenderBeams{ viewRenderBeams }, entityList{ entityList }, engine{ engine }, globalVars{ globalVars }
    {
    }

    bool enabled;
    Color4 color;

    void onBulletImpact(const csgo::GameEvent& event)
    {
        if (!enabled)
            return;

        if (!localPlayer)
            return;

        if (event.getInt("userid") != localPlayer.get().getUserId(engine))
            return;

        auto beamInfo = createBeamInfo(event);
        if (!beamInfo.has_value())
            return;

        if (const auto beam = viewRenderBeams.createBeamPoints(*beamInfo)) {
            constexpr auto FBEAM_FOREVER = 0x4000;
            beam->flags &= ~FBEAM_FOREVER;
            beam->die = globalVars->currenttime + 2.0f;
        }
    }

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Enabled", enabled).def(false);
        configurator("Color", color);
    }

private:
    [[nodiscard]] std::optional<csgo::BeamInfo> createBeamInfo(const csgo::GameEvent& event)
    {
        csgo::BeamInfo beamInfo;

        const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getActiveWeapon());
        if (activeWeapon.getPOD() == nullptr)
            return {};

        if (!localPlayer.get().shouldDraw()) {
            const auto viewModel = csgo::Entity::from(retSpoofGadgets->client, entityList.getEntityFromHandle(localPlayer.get().viewModel()));
            if (viewModel.getPOD() == nullptr)
                return {};

            if (!viewModel.getAttachment(activeWeapon.getMuzzleAttachmentIndex1stPerson(viewModel.getPOD()), beamInfo.start))
                return {};
        } else {
            const auto worldModel = csgo::Entity::from(retSpoofGadgets->client, entityList.getEntityFromHandle(activeWeapon.weaponWorldModel()));
            if (worldModel.getPOD() == nullptr)
                return {};

            if (!worldModel.getAttachment(activeWeapon.getMuzzleAttachmentIndex3rdPerson(), beamInfo.start))
                return {};
        }

        beamInfo.end.x = event.getFloat("x");
        beamInfo.end.y = event.getFloat("y");
        beamInfo.end.z = event.getFloat("z");

        beamInfo.modelName = "sprites/physbeam.vmt";
        beamInfo.modelIndex = -1;
        beamInfo.haloName = nullptr;
        beamInfo.haloIndex = -1;

        beamInfo.red = 255.0f * color.color[0];
        beamInfo.green = 255.0f * color.color[1];
        beamInfo.blue = 255.0f * color.color[2];
        beamInfo.brightness = 255.0f * color.color[3];

        beamInfo.type = 0;
        beamInfo.life = 0.0f;
        beamInfo.amplitude = 0.0f;
        beamInfo.segments = -1;
        beamInfo.renderable = true;
        beamInfo.speed = 0.2f;
        beamInfo.startFrame = 0;
        beamInfo.frameRate = 0.0f;
        beamInfo.width = 2.0f;
        beamInfo.endWidth = 2.0f;
        beamInfo.flags = 0x40;
        beamInfo.fadeLength = 20.0f;

        return beamInfo;
    }

    csgo::ViewRenderBeams viewRenderBeams;
    csgo::EntityList entityList;
    csgo::Engine engine;
    csgo::GlobalVars* globalVars;
};
