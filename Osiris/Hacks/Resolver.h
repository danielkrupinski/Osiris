#pragma once

#include "../SDK/Entity.h"
#include <deque>
#include <vector>


namespace Resolver {


	void BasicResolver(Entity* entity, int missed_shots);
    void UpdateTargeted(Entity* entity, bool targeted);
    void Update();


    /* 
      
       TODO: On game event bullet_impact, trace a ray from user to point, and see if it intersects/touches a player. If so, resolver miss, otherwise
       spread miss.
       
       void MissedDueToSpread(Gameevent &event);
    */

    struct Record {
        float prevSimTime = 0.0f;
        int prevhealth = 0;
        int lastworkingshot = -1;
        int missedshots = 0;
        bool wasTargeted = false;
        bool invalid = true;
        bool FiredUpon = false;
        bool autowall = false;
        float PreviousEyeAngle = 0.0f;
        float eyeAnglesOnUpdate = 0.0f;
        float PreviousDesyncAng = 0.0f;


        bool wasUpdated = false;
    };

    extern Record invalid_record;
    //extern std::vector<Record> PlayerRecords(65);
    extern std::vector<Record> PlayerRecords;


}