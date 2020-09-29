#include <mutex>
#include <numeric>
#include <sstream>
#include <codecvt>
#include <locale>

#include "Memory.h"
#include "Debug.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "../Hacks/Misc.h"
#include "../SDK/ConVar.h"
#include "../SDK/Surface.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/WeaponData.h"
#include "../Hacks/EnginePrediction.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GameEvent.h"
#include "../SDK/FrameStage.h"
#include "../SDK/Client.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/WeaponSystem.h"
#include "../SDK/WeaponData.h"
#include "../GUI.h"
#include "../Helpers.h"
#include "../SDK/ModelInfo.h"
#include "../Hacks/Resolver.h"
#include "../Hacks/Backtrack.h"

#include <vector>


/* OUTPUT */
Debug::screen Debug::Screen;

void Debug::DrawBox(coords start, coords end) {
    //if ((start.y > end.y) || (end.y > Screen.Height) || (start.y < 0))
     //   return;
    //if ((start.x > end.x) || (end.x > Screen.Width) || (start.x < 0))
    //    return;

    interfaces->surface->setDrawColor(config->debug.box.color[0], config->debug.box.color[1], config->debug.box.color[2], 180);
    interfaces->surface->drawFilledRect(start.x, start.y, end.x, end.y);


}




void Debug::DrawGraphBox(coords start, coords end, float min_val, float max_val, float val, float ratio, std::wstring name) {

    interfaces->surface->setDrawColor(config->debug.box.color[0], config->debug.box.color[1], config->debug.box.color[2], 180);
    interfaces->surface->drawOutlinedRect(start.x, start.y, end.x, end.y);
    int print_val = val;
    if (val > max_val) {
        val = max_val;
    }
    else if (val < min_val) {
        val = min_val;
    }

    if (min_val < 1) {
        min_val = 1;
        max_val += (-1 * (0 - min_val));
        val += (-1 * (0 - min_val));
    }

    if (val < 1)
        return;

    int height = static_cast<int>((end.y - start.y) * (val / (max_val - min_val)));
    start.y = end.y - height;

    if ((start.y > end.y) || (end.y > Screen.Height) || (start.y < 0))
        return;
    if ((start.x > end.x) || (end.x > Screen.Width) || (start.x < 0))
        return;


    interfaces->surface->setDrawColor(config->debug.box.color[0], config->debug.box.color[1], config->debug.box.color[2], 255);
    interfaces->surface->drawFilledRect(start.x, start.y, end.x, end.y);

    std::wstring fps{ std::to_wstring(static_cast<int>(print_val))};
    const auto [fpsWidth, fpsHeight] = interfaces->surface->getTextSize(Surface::font, fps.c_str());
    interfaces->surface->setTextColor(config->debug.animStateMon.color);
    interfaces->surface->setTextPosition(start.x, (start.y - fpsHeight));
    interfaces->surface->printText(fps.c_str());

    interfaces->surface->setTextColor(config->debug.animStateMon.color);
    interfaces->surface->setTextPosition(start.x, end.y);
    interfaces->surface->printText(name.c_str());


}


bool Debug::SetupTextPos(std::vector <std::wstring>& Text) { // If true, can't draw anymore due to screen being full

	int pos_inc_h = 0;
	int pos_inc_w = 0;
    int textsize = 0;
    for (int i = 0; i < Text.size(); i++) {

        std::wstring Str = Text[i];
		const auto [text_size_w, text_size_h] = interfaces->surface->getTextSize(Surface::font, Str.c_str());
        textsize = text_size_h;
		pos_inc_h += text_size_h;
		pos_inc_w = text_size_w;

        if (Screen.CurrColumnMax < pos_inc_w)
            Screen.CurrColumnMax = pos_inc_w;
	}


	


	if ((pos_inc_h + Screen.CurrPosH) > Screen.Height) {
        Screen.CurrPosH = 5;
        Screen.CurrPosW += Screen.CurrColumnMax + 1;
        Screen.CurrColumnMax = 0;
	}


	if ((pos_inc_w + Screen.CurrPosW) > Screen.Width) {
		return true;
	}

    if (config->debug.box.enabled) {
        coords start;
        coords end;
        start.x = Screen.CurrPosW;
        start.y = Screen.CurrPosH + textsize;
        end.x = start.x + Screen.CurrColumnMax;
        end.y = start.y + pos_inc_h;
        Debug::DrawBox(start,end);
    }

	return false;

}

void Debug::Draw_Text(std::vector<std::wstring>& Text) {


    //std::wstring lolstr = { L"CurrPosH " + std::to_wstring(Screen.CurrPosH) + L" CurrPosW " + std::to_wstring(Screen.CurrPosW) };
    //interfaces->surface->setTextPosition(110, 110);
    //interfaces->surface->printText(lolstr.c_str());

    if (SetupTextPos(Text)) {
        return;
    }


    for (int i = 0; i < Text.size(); i++) {

        std::wstring Str = Text[i];

		const auto [text_size_w, text_size_h] = interfaces->surface->getTextSize(Surface::font, Str.c_str());
		Screen.CurrPosH += text_size_h;
		interfaces->surface->setTextPosition(Screen.CurrPosW, Screen.CurrPosH);
		interfaces->surface->printText(Str.c_str());
	}
}

void Debug::AnimStateMonitor() noexcept
{


    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {


        auto entity = interfaces->entityList->getEntity(i);

        if ((i != config->debug.entityid) && (config->debug.entityid != -1))
            continue;

        if (!entity || entity->isDormant() || !entity->isAlive() || !entity->isPlayer())
            continue;

        auto AnimState = entity->getAnimstate();
        if (!AnimState)
            continue;


        for (int b = 0; b < entity->getAnimationLayerCount(); b++) {

            if (config->debug.overlay > 15)
                config->debug.overlay = 0;
            else if (config->debug.overlay < 0)
                config->debug.overlay = 15;

            if ((config->debug.overlay > 0) && (config->debug.overlay != b))
                continue;

            auto AnimLayer = entity->getAnimationLayer(b);

            if (!AnimLayer)
                continue;

            auto model = entity->getModel();
            if (!model)
                continue;

            auto studiohdr = interfaces->modelInfo->getStudioModel(model);
            if (!studiohdr)
                continue;

            int Act = entity->getSequenceActivity(AnimLayer->sequence);

            if (Act == 0 || Act == -1)
                continue;


            if (config->debug.overlayall) {
                if (Act != 979)
                    continue;
            }


            if (config->debug.weight) {
                if (!(AnimLayer->weight >= 1))
                    continue;
            }


            interfaces->surface->setTextColor(config->debug.animStateMon.color);

            std::wstring AnimStateStr;

            AnimActs animact;

            AnimStateStr = (animact.getEnum_Array()[Act] + L" " + std::to_wstring(Act));

            //std::wstring playername{ L"PlayerName: " + std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true)) };
            std::wstring clientblend{ L"Client Blend: " + std::to_wstring(AnimLayer->clientblend)};
            std::wstring dispatchedsrc{ L"Dispatch Source: " + std::to_wstring(AnimLayer->dispatchedsrc) };
            std::wstring dispatcheddst{ L"Dispatch Source: " + std::to_wstring(AnimLayer->dispatcheddst) };
            std::wstring blendin{ L"blendin: " + std::to_wstring(AnimLayer->blendin) };
            std::wstring weight{ L"Weight: " + std::to_wstring(AnimLayer->weight) };
            std::wstring weightdelta{ L"Weight Delta Rate: " + std::to_wstring(AnimLayer->weightdeltarate) };
            std::wstring cycle{ L"Cycle: " + std::to_wstring(AnimLayer->cycle) };
            std::wstring prevcycle{ L"Previous Cycle " + std::to_wstring(AnimLayer->prevcycle) };
            std::wstring overlay{ L"Overlay: " + std::to_wstring(b) };
            std::wstring state{ L"AnimState: " + AnimStateStr };
            std::wstring order{ L"Order: " + std::to_wstring(AnimLayer->order) };
            std::wstring playbackrate{ L"Playback Rate: " + std::to_wstring(AnimLayer->playbackRate) };





            std::vector<std::wstring> strings{ clientblend, dispatchedsrc, dispatcheddst, blendin, cycle, prevcycle, weight, weightdelta, overlay, state, order, playbackrate };

            if (config->debug.AnimExtras) {
                std::wstring currPitch{ L"Current Pitch: " + std::to_wstring(AnimState->m_flPitch) };
                std::wstring currLBY{ L"Current LBY: " + std::to_wstring(AnimState->m_flGoalFeetYaw) };
                std::wstring currEyeYaw{ L"Current Eye Yaw: " + std::to_wstring(entity->eyeAngles().y) };
                std::wstring speed2D{ L"Speed 2D: " + std::to_wstring(AnimState->speed_2d) };
                std::wstring currLBYFEETDelta{ L"LBY/Feet Delta: " + std::to_wstring(AnimState->m_flGoalFeetYaw - entity->eyeAngles().y) };
            }

            if (config->debug.ResolverRecords) {
                auto record = &Resolver::PlayerRecords.at(entity->index());
                if (!record || record->invalid == true)
                    return;

                if (config->debug.TargetOnly) {
                    if (!record->wasTargeted)
                        continue;
                }

                std::wstring Resolver{ L"RESOLVER INFO: " };
                std::wstring wasTargeted{ L"Was Targeted: " + std::to_wstring(record->wasTargeted) };
                std::wstring FiredUpon{ L"Fired Upon: " + std::to_wstring(record->FiredUpon) };
                std::wstring WasUpdated{ L"Was Updated: " + std::to_wstring(record->wasUpdated) };
                std::wstring missedshots{ L"Missed Shots: " + std::to_wstring(record->missedshots) };
                std::wstring lastworking{ L"Last Working Shot: " + std::to_wstring(record->lastworkingshot) };
                std::wstring PrevEye{ L"Previous Eye Angles (Y): " + std::to_wstring(record->PreviousEyeAngle) };
                std::wstring PrevHealth{ L"Previous Entity Health: " + std::to_wstring(record->prevhealth) };
                std::wstring PrevDesync{ L"Previous Desync Angle: " + std::to_wstring(record->PreviousDesyncAng) };

                std::wstring invalid{ L"Invalid: " + std::to_wstring(record->invalid) };

                strings.push_back(Resolver);
                strings.push_back(wasTargeted);
                strings.push_back(FiredUpon);
                strings.push_back(WasUpdated);
                strings.push_back(missedshots);
                strings.push_back(lastworking);
                strings.push_back(PrevEye);
                strings.push_back(PrevHealth);
                strings.push_back(PrevDesync);
                strings.push_back(invalid);

            }

            //std::wstring lolstr = L"YOMAMA ";
            //interfaces->surface->setTextPosition(100, 105);
            // interfaces->surface->printText(lolstr.c_str());

            Draw_Text(strings);


        }

    }
}

std::vector<std::wstring> Debug::formatRecord(Backtrack::Record record, Entity* entity, int index) {

    /*

    struct Record {
        Vector head;
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[256];
        int PreviousAct;
        int prevhealth = 100;
        matrix3x4 prevResolvedMatrix[256];
        int missedshots = 0;
        bool wasTargeted = false;
    };
    std::wstring weightdelta{ L"Weight Delta Rate: " + std::to_wstring(AnimLayer->weightdeltarate) };

    */
    std::vector<std::wstring> strings;

 //   std::wstring playername{ L"PlayerName: " + std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true)) };
    std::wstring bindex{ L"Index: " + std::to_wstring(index)};
    std::wstring simtime{ L"Simulation Time: " + std::to_wstring(record.simulationTime) };
    std::wstring prevhealth{ L"Previous Health: " + std::to_wstring(record.prevhealth) };
    std::wstring missedshots{ L"Missed Shots: " + std::to_wstring(record.missedshots) };
    std::wstring wasTargeted{ L"Was Targeted?: " + std::to_wstring(record.wasTargeted) };
    std::wstring wasUpdated{ L"Was Updated?: " + std::to_wstring(record.wasUpdated) };
    std::wstring resolvedAngles{ L"Resolved Angle: " + std::to_wstring(record.resolvedaddition) };

    strings = { bindex, simtime, prevhealth, missedshots, wasTargeted, wasUpdated, resolvedAngles };
    return strings;



}

void Debug::BacktrackMonitor() noexcept {

    /*
    backtrack{
        bool enabled{false};
        ColorToggle color;
        int entityid{0};
        bool newesttick{false};
    }
    
    */
    if ((interfaces->engine->getMaxClients() < config->debug.backtrack.entityid) || (0 > config->debug.backtrack.entityid))
        return;

    auto entity = interfaces->entityList->getEntity(config->debug.backtrack.entityid);

    if (!entity && !(config->debug.backtrack.findactive))
        return;
}

void Debug::DrawDesyncInfo() {
	interfaces->surface->setTextColor(config->debug.desync_info.color);
	
	if (!localPlayer)
		return;

	std::wstring MaxAng{ L"Max Possible Desync Angle: " + std::to_wstring(localPlayer->getMaxDesyncAngle()) };
	std::wstring CurrYaw{ L"Current Player Yaw: "  };
	std::wstring CurrPitch{ L"Current Player Pitch: " };





}

void Debug::run(){


	interfaces->surface->setTextFont(Surface::font);
	const auto [screenWidth, screenHeight] = interfaces->surface->getScreenSize();

    Screen.Width = screenWidth;
	Screen.Height = screenHeight;
    Screen.CurrPosW = 0;
    Screen.CurrPosH = 5;
    Screen.CurrColumnMax = 0;

	if (config->debug.desync_info.enabled)
		DrawDesyncInfo();

    if (config->debug.animStateMon.enabled)
        AnimStateMonitor();

    if (config->debug.backtrack.color.enabled)
        BacktrackMonitor();



    if (config->debug.FPSBar) {
        coords start, end;
        std::wstring BASESTRING = L"AAAA";
        const auto [text_size_w, text_size_h] = interfaces->surface->getTextSize(Surface::font, BASESTRING.c_str());
        start.y = (text_size_h * 3);
        start.x = (Screen.Width - text_size_w) - 5;
        end.y = (text_size_h * 13);
        end.x = Screen.Width - 5;

        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime;
        DrawGraphBox(start, end, 1.0f, 150.0f, 1.0f / frameRate, 0, L"FPS");

        float latency = 0.0f;
        if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
            latency = networkChannel->getLatency(0);

        end.x = start.x - 3;
        start.x = (start.x - 3) - text_size_w;
        DrawGraphBox(start, end, 1.0f, 200.0f, latency * 1000, 0, L"PING");
    }

    if (config->debug.indicators.enabled) {
        if (config->debug.indicators.baim) {

            if (!localPlayer || !localPlayer->isAlive())
                return;

            const auto activeWeapon = localPlayer->getActiveWeapon();
            if (!activeWeapon || !activeWeapon->clip())
                return;

            auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
            if (!weaponIndex)
                return;

            if (config->ragebot[weaponIndex].Baim && (GetAsyncKeyState(config->ragebot[weaponIndex].keyMode))){
                    std::wstring BASESTRING = L"Baim Enabled";
                    const auto [text_size_w, text_size_h] = interfaces->surface->getTextSize(Surface::font, BASESTRING.c_str());
                    interfaces->surface->setTextColor(config->debug.animStateMon.color);
                    interfaces->surface->setTextPosition((Screen.Width - text_size_w) - 5, ((Screen.Height / 2) -(text_size_h / 2) - text_size_h));
                    interfaces->surface->printText(BASESTRING.c_str());

            }
            if (config->debug.indicators.resolver) {
                std::wstring BASESTRING = L"Resolver Triggered";
                const auto [text_size_w, text_size_h] = interfaces->surface->getTextSize(Surface::font, BASESTRING.c_str());
                interfaces->surface->setTextColor(config->debug.animStateMon.color);
                interfaces->surface->setTextPosition((Screen.Width - text_size_w) - 5, ((Screen.Height / 2) - (text_size_h / 2) - text_size_h));
                interfaces->surface->printText(BASESTRING.c_str());
            }
        }


    }
    return;
}


/* INPUT */

void Debug::AnimStateModifier() noexcept
{
    if (!config->debug.AnimModifier)
        return;



    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {



            auto entity = interfaces->entityList->getEntity(i);

            //if ((i != config->debug.entityid) && (config->debug.entityid != -1))
                //continue;

            if (!entity || entity->isDormant() || !entity->isAlive() || !entity->isPlayer())
                continue;

            if (entity == localPlayer.get())
                continue;

            auto AnimState = entity->getAnimstate();
            if (!AnimState)
                continue;

            if (config->debug.animstatedebug.manual) {
                entity->eyeAngles().x = config->debug.Pitch;
                //entity->eyeAngles().y = config->debug.Yaw;
                //AnimState->m_flGoalFeetYaw = config->debug.GoalFeetYaw;
            }

            if (config->debug.animstatedebug.resolver.enabled) {
                Resolver::BasicResolver(entity, config->debug.animstatedebug.resolver.missed_shots);
            }

            //AnimState = entity->getAnimstate();
           // if (!AnimState)
            //    continue;



            //AnimState->m_iLastClientSideAnimationUpdateFramecount -= 1;


            //Vector NewAnim{ config->debug.Pitch , config->debug.Yaw, 0.0f };
            //entity->setAbsAngle(NewAnim);

           // entity->UpdateState(AnimState, NewAnim);

            


        

    }


}