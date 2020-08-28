#include "../Interfaces.h"
#include "Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/Vector.h"
#include "../SDK/matrix3x4.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/GlobalVars.h"
#include "Multipoints.h"

namespace Multipoints
{
    bool retrieveAll(Entity* entity, float multiPointsExpansion, Vector(&multiPoints)[Multipoints::HITBOX_MAX][Multipoints::MULTIPOINTS_MAX])
    {
        if (!localPlayer)
            return false;

        if (!entity)
            return false;

        static matrix3x4 bones[256];

        if (!entity->setupBones(bones, ARRAYSIZE(bones), 256, memory->globalVars->currenttime))
            return false;

        const Model* model = entity->getModel();

        if (!model)
            return false;

        StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

        if (!hdr)
            return false;

        int hitBoxSet = entity->hitboxSet();

        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;

        for (int hitBox = Multipoints::HITBOX_START; hitBox < (std::min) (hitBoxSetPtr->numHitboxes, (decltype(hitBoxSetPtr->numHitboxes))Multipoints::HITBOX_MAX); hitBox++)
        {
            StudioBbox* box = hitBoxSetPtr->getHitbox(hitBox);

            if (box)
            {
                Vector min = box->bbMin;
                Vector max = box->bbMax;

                if (box->capsuleRadius > 0.0f)
                {
                    min -= Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };
                    max += Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };
                }

                static Vector locations[Multipoints::MULTIPOINTS_MAX];

                locations[Multipoints::MULTIPOINTS_CENTER] = (min + max) * 0.5f;

                locations[1] = Vector{ min.x, min.y, min.z, };
                locations[2] = Vector{ min.x, max.y, min.z, };
                locations[3] = Vector{ max.x, max.y, min.z, };
                locations[4] = Vector{ max.x, min.y, min.z, };
                locations[5] = Vector{ max.x, max.y, max.z, };
                locations[6] = Vector{ min.x, max.y, max.z, };
                locations[7] = Vector{ min.x, min.y, max.z, };
                locations[8] = Vector{ max.x, min.y, max.z, };

                for (int multiPoint = Multipoints::MULTIPOINTS_START; multiPoint < Multipoints::MULTIPOINTS_MAX; multiPoint++)
                {
                    if (multiPoint > Multipoints::MULTIPOINTS_START)
                    {
                        locations[multiPoint] = (((locations[multiPoint] + locations[Multipoints::MULTIPOINTS_CENTER]) * 0.5f) + locations[multiPoint]) * 0.5f;
                    }

                    multiPoints[hitBox][multiPoint] = locations[multiPoint].transform(bones[box->bone]);
                }
            }
        }

        return true;
    }

    bool retrieveOne(Entity* entity, float multiPointsExpansion, Vector(&multiPoints)[Multipoints::MULTIPOINTS_MAX], int desiredHitBox)
    {
        if (desiredHitBox < Multipoints::HITBOX_START || desiredHitBox > Multipoints::HITBOX_LAST_ENTRY)
            return false;

        if (!localPlayer)
            return false;

        if (!entity)
            return false;

        static matrix3x4 bones[256];

        if (!entity->setupBones(bones, ARRAYSIZE(bones), 256, memory->globalVars->currenttime))
            return false;

        const Model* model = entity->getModel();

        if (!model)
            return false;

        StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

        if (!hdr)
            return false;

        int hitBoxSet = entity->hitboxSet();

        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;

        StudioBbox* box = hitBoxSetPtr->getHitbox(desiredHitBox);

        if (!box)
            return false;

        Vector min = box->bbMin;
        Vector max = box->bbMax;

        if (box->capsuleRadius > 0.0f)
        {
            min -= Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };
            max += Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };
        }

        static Vector locations[Multipoints::MULTIPOINTS_MAX];

        locations[Multipoints::MULTIPOINTS_CENTER] = (min + max) * 0.5f;

        locations[1] = Vector{ min.x, min.y, min.z, };
        locations[2] = Vector{ min.x, max.y, min.z, };
        locations[3] = Vector{ max.x, max.y, min.z, };
        locations[4] = Vector{ max.x, min.y, min.z, };
        locations[5] = Vector{ max.x, max.y, max.z, };
        locations[6] = Vector{ min.x, max.y, max.z, };
        locations[7] = Vector{ min.x, min.y, max.z, };
        locations[8] = Vector{ max.x, min.y, max.z, };

        for (int multiPoint = Multipoints::MULTIPOINTS_START; multiPoint < Multipoints::MULTIPOINTS_MAX; multiPoint++)
        {
            if (multiPoint > Multipoints::MULTIPOINTS_START)
            {
                locations[multiPoint] = (((locations[multiPoint] + locations[Multipoints::MULTIPOINTS_CENTER]) * 0.5f) + locations[multiPoint]) * 0.5f;
            }

            multiPoints[multiPoint] = locations[multiPoint].transform(bones[box->bone]);
        }

        return true;
    };
};

