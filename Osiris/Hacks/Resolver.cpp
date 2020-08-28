#include "Resolver.h"

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"
#include "../SDK/ConVar.h"
#include "../SDK/Surface.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/WeaponData.h"
#include "EnginePrediction.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GameEvent.h"
#include "../SDK/FrameStage.h"
#include "../SDK/Client.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/WeaponSystem.h"
#include "../SDK/WeaponData.h"
#include "../SDK/Vector.h"
#include "../GUI.h"
#include "../Helpers.h"
#include "../SDK/ModelInfo.h"
#include "Backtrack.h"
#include "../SDK/AnimState.h"
#include "../SDK/LocalPlayer.h"

#include <deque>

const int MAX_RECORDS = 128;

std::vector<Resolver::Record> Resolver::PlayerRecords(65);
Resolver::Record Resolver::invalid_record;




void Resolver::Update() {
    if (!localPlayer || !localPlayer->isAlive()) {
        return;
    }

    for (int i = 0; i < PlayerRecords.size(); i++) {

        auto record = &PlayerRecords.at(i);

        if (!record)
            continue;

        auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity->isDormant() || !entity->isAlive()) {
            if (record->FiredUpon || record->wasTargeted) {
                record->lastworkingshot = record->missedshots;
            }
            record->invalid = true;
            continue;
        }

        if (record->invalid == true) {
            record->missedshots = config->debug.animstatedebug.resolver.missedoffset;
            record->prevhealth = entity->health();
        }
        record->invalid = false;
        
        
        if (!record->FiredUpon) 
            continue;    

        else if (record->prevhealth == entity->health() || (config->debug.animstatedebug.resolver.goforkill)) {
            record->lastworkingshot = -1;
            record->missedshots = (record->missedshots >= 12 ? 0 : ++(record->missedshots));
            record->wasUpdated = false;
        }

        record->prevhealth = entity->health();

    }
}



void Resolver::BasicResolver(Entity* entity, int missed_shots) {

   // config->debug.indicators.resolver = false;
    if (!localPlayer || !localPlayer->isAlive() || !entity || entity->isDormant() || !entity->isAlive())
        return;

    if (PlayerRecords.empty())
        return;

    auto record = &PlayerRecords.at(entity->index());

    if (!record || record->invalid)
        return;

    if (entity->getVelocity().length2D() > 3.0f) {
        record->PreviousEyeAngle = entity->eyeAngles().y;
        return;
    }


    /*
    const auto Brecord = &Backtrack::records[entity->index()];

    if (!(Brecord && Vrecord->size() && Backtrack::valid(Vrecord->front().simulationTime)))
        return;
    */

    /*

	auto Animstate = entity->getAnimstate();

	if (!Animstate)
		return;
    


    int currAct = 1;

    if (entity->getVelocity().length2D() > 1.0f)
        return;

    for (int b = 0; b < entity->getAnimationLayerCount(); b++) {
        auto AnimLayer = entity->getAnimationLayer(b);
        currAct = entity->getSequenceActivity(AnimLayer->sequence);

        //if (currAct == ACT_CSGO_FIRE_PRIMARY && (AnimLayer->weight > (0.0f)) && (AnimLayer->cycle < .8f))
         //   return;, 
          
        if ((currAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST) || (currAct == ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING)) {
            break;
        }
    };

    float DesyncAng = Animstate->m_flGoalFeetYaw;
    //float eyeAnglesY = entity->eyeAngles().y;


   //record->front().prevAngles = entity->eyeAngles().y;

    if ((record->size() > 1) && Backtrack::valid(record->at(1).simulationTime)) {
        if (record->at(1).wasUpdated) {
                entity->eyeAngles().y = record->front().resolvedaddition;
                Animstate->m_flGoalFeetYaw = record->front().resolvedaddition;
                entity->UpdateState(Animstate, entity->eyeAngles());
                return;
        }

    }

    if (!currAct)
        return;

    if ((currAct != ACT_CSGO_IDLE_TURN_BALANCEADJUST) && (currAct != ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING) && !(config->debug.animstatedebug.resolver.overRide)) {
        return;
    }


    if (  (   !(record->front().PreviousAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST) && !(record->front().PreviousAct == ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING)  ) && !(config->debug.animstatedebug.resolver.overRide)) {
        return;
    }
    
    

    //if ((record->front().PreviousAct == 0) || (!record->front().wasTargeted))
    //    return;

   //record->front().PreviousAct = 0;
    */

    float DesyncAng = 0;

    auto Animstate = entity->getAnimstate();

    if (!Animstate)
        return;

    if (!record->FiredUpon || !record->wasTargeted) {

        entity->UpdateState(Animstate, entity->eyeAngles());
        if ((record->wasUpdated == false) && (entity->eyeAngles().y != record->PreviousEyeAngle)) {
            //record->PreviousEyeAngle = entity->eyeAngles().y;
            record->eyeAnglesOnUpdate = entity->eyeAngles().y;
            record->prevSimTime = entity->simulationTime();
            record->PreviousEyeAngle = entity->eyeAngles().y + record->PreviousDesyncAng;
            record->wasUpdated = true;
        } 


        
        if ((record->wasUpdated == true) && (entity->eyeAngles().y != record->PreviousEyeAngle) && (record->prevSimTime != entity->simulationTime())){
            //record->PreviousEyeAngle = entity->eyeAngles().y;
            /*
            record->eyeAnglesOnUpdate = entity->eyeAngles().y;
            record->PreviousEyeAngle = entity->eyeAngles().y + record->PreviousDesyncAng;
            record->prevSimTime = entity->simulationTime();
            */
        }
        
        entity->eyeAngles().y = record->PreviousEyeAngle;
        entity->UpdateState(Animstate, entity->eyeAngles());

        return;
    }
    
    //if (record->wasUpdated == true)
    //   return;

    entity->UpdateState(Animstate, entity->eyeAngles());

    //config->debug.indicators.resolver = true;

    int missed = record->missedshots;

  //  if (config->debug.animstatedebug.resolver.missed_shots > 0)
      //  missed = config->debug.animstatedebug.resolver.missed_shots;

    if (record->lastworkingshot != -1)
        missed = record->lastworkingshot;


    switch (missed) {
        case 1:
            DesyncAng += 25.0f;
            break;
        case 2:
            DesyncAng -= 25.0f;
            break;
        case 3:
            DesyncAng += 58.0f;
            break;
        case 4:
            DesyncAng -= 58.0f;
            break;
        case 5:
            DesyncAng = 70;
            break;
        case 6:
            DesyncAng = -70;
            break;
        case 7:
            DesyncAng = Animstate->m_flGoalFeetYaw;
            break;
        case 8:
            DesyncAng += -116.0f;
            break;
        case 9:
            DesyncAng += +116.0f;
            break;
        case 10:
            DesyncAng = DesyncAng + (entity->getMaxDesyncAngle() * -1);
            break;
        case 11:
            DesyncAng = DesyncAng + entity->getMaxDesyncAngle();
            break;
        case 12:
            DesyncAng = -15;
        case 13:
            DesyncAng = 15;
        default:
            DesyncAng = Animstate->m_flGoalFeetYaw;
            break;
    }



    entity->eyeAngles().y += DesyncAng;
    Animstate->m_flGoalFeetYaw += DesyncAng;
    entity->UpdateState(Animstate, entity->eyeAngles());
    record->PreviousEyeAngle = entity->eyeAngles().y;

    if (record->PreviousEyeAngle > 180) {
        record->PreviousEyeAngle -= 180;
    }
    else if (record->PreviousEyeAngle < -180) {
        record->PreviousEyeAngle += 180;
    }

    record->PreviousDesyncAng = DesyncAng;
    record->wasUpdated = true;
    record->FiredUpon = false;
    //record->front().resolvedaddition = entity->eyeAngles().y;
    //Animstate->m_flEyeYaw = Animstate->m_flCurrentFeetYaw;
    //Animstate->m_flGoalFeetYaw = 0.0f;
    //entity->eyeAngles().x = Animstate->m_flPitch;

    return;

}