#include "Ragebot.h"
#include "Autowall.h"

#include "../Interfaces.h"
#include "../Config.h"

#include "../SDK/Math.h"
#include "../SDK/Entity.h"
#include "../SDK/Engine.h"
#include "../SDK/ConVar.h"
#include "../SDK/UserCmd.h"
#include "../SDK/ModelInfo.h"

//this working
static bool HitChance(Vector angles, Entity* entity, Entity* weapon, int weaponIndex, UserCmd* cmd, const int chance, int &outHitchance, Vector &firstWallToPen, int& raysHit, Vector& bestVector) noexcept
{
	raysHit = 0;
	outHitchance = 0;
	
	if (!chance)
		return true;

	int hitseed = 512;

	int iHit = 0;
	int iHitsNeed = (int)((float)hitseed * ((float)chance / 100.f));
	bool bHitchance = false;

	Vector forward, right, up;
	Math::AngleVectors(angles, &forward, &right, &up);

	bestVector = {0, 0, 0};
	float bestChance = 0;

	weapon->UpdateAccuracyPenalty();

	for (auto i = 0; i < hitseed; ++i) {

		float RandomA = 0.6; //Math::RandomFloat(0.0f, 1.0f); // random 1 /* COMMENTED THIS OUT, DOENST EVEN MATTER WHAT YOU USE ANYWAYS DIFFERENT ON SERVER SIDE */
		float RandomB = 1.0f - RandomA * RandomA; 
		RandomA = 0.3; //Math::RandomFloat(0.0f, M_PIF * 2.0f); // random 2 /* COMMENTED THIS OUT, DOENST EVEN MATTER WHAT YOU USE ANYWAYS DIFFERENT ON SERVER SIDE */
		RandomB *= weapon->getSpread() + weapon->getInaccuracy();
		float SpreadX1 = (cos(RandomA) * RandomB);
		float SpreadY1 = (sin(RandomA) * RandomB);
		float RandomC = 0.2; //Math::RandomFloat(0.0f, 1.0f); // random 3
		float RandomF = RandomF = 1.0f - RandomC * RandomC;
		RandomC = 3.14159265 * 1.5f; //Math::RandomFloat(0.0f, M_PIF * 2.0f); // random 4 /* COMMENTED THIS OUT, DOENST EVEN MATTER WHAT YOU USE ANYWAYS DIFFERENT ON SERVER SIDE */
		RandomF *= weapon->getSpread();
		float SpreadX2 = (cos(RandomC) * RandomF);
		float SpreadY2 = (sin(RandomC) * RandomF);
		float fSpreadX = SpreadX1 + SpreadX2;
		float fSpreadY = SpreadY1 + SpreadY2;

		Vector vSpreadForward;
		vSpreadForward.x = forward.x + (fSpreadX * right.x) + (fSpreadY * up.x);
		vSpreadForward.y = forward.y + (fSpreadX * right.y) + (fSpreadY * up.y);
		vSpreadForward.z = forward.z + (fSpreadX * right.z) + (fSpreadY * up.z);

		Vector rawNew = {angles.x + (fSpreadX * right.x) + (fSpreadY * up.x), angles.y + (fSpreadX * right.y) + (fSpreadY * up.y), angles.z + (fSpreadX * right.z) + (fSpreadY * up.z)};
		
		vSpreadForward.NormalizeInPlace();

		Vector qaNewAngle;
		Math::VectorAngles(vSpreadForward, qaNewAngle);
		qaNewAngle.normalize();

		Vector vEnd;
		Math::AngleVectors(qaNewAngle, &vEnd);
		vEnd = localPlayer->getEyePosition() + (vEnd * 8192.f);

		if (Autowall->PenetrateWall(entity, vEnd, weaponIndex, firstWallToPen))
		{
			iHit++;
		}

		float currentHitChance = (static_cast<float>(iHit) / static_cast<float>(hitseed)) * 100.f;
		int percentChance = static_cast<int>(currentHitChance);
		
		outHitchance = percentChance;
		raysHit = iHit;
		
		if (percentChance >= 2 && config->ragebot[weaponIndex].keyForceShotEnabled && config->ragebot[weaponIndex].keyForceShot > 0 && GetAsyncKeyState(config->ragebot[weaponIndex].keyForceShot))
		{
			bHitchance = true;
			break;
		}

		if (percentChance >= chance) {
			bHitchance = true;

			if (currentHitChance > bestChance)
			{
				bestChance = currentHitChance;
				bestVector = rawNew;
			}
		}
		
		if ((static_cast<float>(hitseed) - 1 - i + iHit) < iHitsNeed)
		{
			break;	
		}
	}
	
	return bHitchance;
}

void Ragebot::Autostop(UserCmd* cmd) noexcept
{
	if (!localPlayer || !localPlayer->isAlive())
		return;

	Vector Velocity = localPlayer->velocity();

	if (Velocity.length2D() == 0)
		return;

	static float Speed = 450.f;

	Vector Direction;
	Vector RealView;
	Math::VectorAngles(Velocity, Direction);
	interfaces->engine->getViewAngles(RealView);
	Direction.y = RealView.y - Direction.y;

	Vector Forward;
	Math::AngleVectors(Direction, &Forward);
	Vector NegativeDirection = Forward * -Speed;

	cmd->forwardmove = NegativeDirection.x;
	cmd->sidemove = NegativeDirection.y;
}

std::vector<Vector> GetMultiplePointsForHitbox(Entity* entity, int iHitbox, matrix3x4 BoneMatrix[256], int weaponIndex) noexcept
{

	std::vector<Vector> vecArray;
	auto VectorTransform_Wrapper = [](const Vector& in1, const matrix3x4& in2, Vector& out)
	{
		auto VectorTransform = [](const float* in1, const matrix3x4& in2, float* out)
		{
			auto DotProducts = [](const float* v1, const float* v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};

	if (localPlayer && localPlayer->isAlive()) {
		auto hitbox = interfaces->modelInfo->getStudioModel(entity->getModel())->getHitboxSet(0)->getHitbox(iHitbox);

		Vector max;
		Vector min;
		VectorTransform_Wrapper(hitbox->bbMax, BoneMatrix[hitbox->bone], max);
		VectorTransform_Wrapper(hitbox->bbMin, BoneMatrix[hitbox->bone], min);

		auto center = (min + max) * 0.5f;

		Vector CurrentAngles = Math::CalcAngle(center, localPlayer->getEyePosition());

		Vector Forward;
		Math::AngleVectors(CurrentAngles, &Forward);

		Vector Right = Forward.Cross(Vector{ 0.0f,0.0f,0.1f });
		Vector Left = Vector{ -Right.x, -Right.y, Right.z };

		Vector Top = Vector{ 0, 0, 1 };
		Vector Bot = Vector{ 0, 0, -1 };

		switch (iHitbox) {
		case 0:
			for (auto i = 0; i < 4; ++i)
				vecArray.emplace_back(center);

			vecArray[1] += Top * (hitbox->capsuleRadius * config->ragebot[weaponIndex].pointChance);
			vecArray[2] += Right * (hitbox->capsuleRadius * config->ragebot[weaponIndex].pointChance);
			vecArray[3] += Left * (hitbox->capsuleRadius * config->ragebot[weaponIndex].pointChance);
			break;

		default:

			for (auto i = 0; i < 3; ++i)
				vecArray.emplace_back(center);

			vecArray[1] += Right * (hitbox->capsuleRadius * config->ragebot[weaponIndex].bodyChance);
			vecArray[2] += Left * (hitbox->capsuleRadius * config->ragebot[weaponIndex].bodyChance);
			break;
		}
		return vecArray;
	}
	return std::vector<Vector>{};
}

std::vector<VectorAndDamage> GetHitBoxes(Entity* entity, Entity* weapon, int weaponIndex, int &bestDmgFound, Vector &firstWallToPen, int& sumDmg, int &bestSingleDamage)noexcept
{
	std::vector<VectorAndDamage> possibleVectors;
	sumDmg = 0;
	bestSingleDamage = 0;
	
	if (!localPlayer)
		return possibleVectors;
	
	bestDmgFound = 0;
	
	if (!localPlayer->isAlive())
		return possibleVectors;
	if (weapon->isKnife() || weapon->isNade())
		return possibleVectors;

	int bestHitbox = -1;
	std::vector<int> hitboxes;

	if (!config->ragebot[weaponIndex].Baim)
	{
		if (config->ragebot[weaponIndex].BonesBools[0])
		hitboxes.push_back(HitBoxes::HITBOX_HEAD);
		if (config->ragebot[weaponIndex].BonesBools[1])
			hitboxes.push_back(HitBoxes::HITBOX_NECK);
		if (config->ragebot[weaponIndex].BonesBools[2])
			hitboxes.push_back(HitBoxes::HITBOX_UPPER_CHEST);
		if (config->ragebot[weaponIndex].BonesBools[3]) {
			hitboxes.push_back(HITBOX_LOWER_CHEST);
			hitboxes.push_back(HITBOX_BODY);
			hitboxes.push_back(HITBOX_THORAX);
		}
		if (config->ragebot[weaponIndex].BonesBools[4])
			hitboxes.push_back(HitBoxes::HITBOX_PELVIS); //PELVIS

		if (config->ragebot[weaponIndex].BonesBools[5])//hands
		{
			hitboxes.push_back(HitBoxes::HITBOX_RIGHT_HAND);
			hitboxes.push_back(HitBoxes::HITBOX_RIGHT_FOREARM);
			hitboxes.push_back(HitBoxes::HITBOX_RIGHT_UPPER_ARM);
			hitboxes.push_back(HitBoxes::HITBOX_LEFT_HAND);
			hitboxes.push_back(HitBoxes::HITBOX_LEFT_FOREARM);
			hitboxes.push_back(HitBoxes::HITBOX_LEFT_UPPER_ARM);
		}
		if (config->ragebot[weaponIndex].BonesBools[6]) 
		{
			hitboxes.push_back(HitBoxes::HITBOX_LEFT_THIGH);
			hitboxes.push_back(HitBoxes::HITBOX_LEFT_CALF);
			hitboxes.push_back(HitBoxes::HITBOX_RIGHT_CALF);
			hitboxes.push_back(HitBoxes::HITBOX_RIGHT_THIGH);
		}
		if (config->ragebot[weaponIndex].BonesBools[7])
		{
			hitboxes.push_back(HitBoxes::HITBOX_LEFT_FOOT);
			hitboxes.push_back(HitBoxes::HITBOX_RIGHT_FOOT);
		}
	}
	else
	{
		hitboxes.push_back(HitBoxes::HITBOX_LOWER_CHEST);
		hitboxes.push_back(HitBoxes::HITBOX_BODY);
		hitboxes.push_back(HitBoxes::HITBOX_THORAX);
		hitboxes.push_back(HitBoxes::HITBOX_PELVIS);
	}

	float minDamage = config->ragebot[weaponIndex].WallDamage;
	
	Vector BestPoint{};

	matrix3x4 matrix[256];
	if (!entity->setupBones(matrix, 256, 256, memory->globalVars->currenttime))
		return possibleVectors;

	minDamage = std::min(minDamage, static_cast<float>(entity->health()));

	if (config->ragebot[weaponIndex].keyForceShotEnabled && config->ragebot[weaponIndex].keyForceShot > 0 && GetAsyncKeyState(config->ragebot[weaponIndex].keyForceShot))
	{
		minDamage = 1;
	}

	for (int HitBoxID : hitboxes)
	{
		for (Vector point : GetMultiplePointsForHitbox(entity, HitBoxID, matrix, weaponIndex))
		{
			float damage = Autowall->Damage(point, firstWallToPen);

			if (damage > bestDmgFound)
			{
				bestDmgFound = damage;
			}
			
			if (damage >= minDamage)
			{
				bestHitbox = HitBoxID;
				minDamage = damage;
				BestPoint = point;

				sumDmg += damage;
				bestSingleDamage = damage;
				possibleVectors.push_back({point, damage});
			}
		}
	}

	return possibleVectors;
}

void Ragebot::run(UserCmd* cmd, int &bestDamage, int &bestHitchance, Vector& wallbangVector)noexcept
{
	Vector firstWallToPen;
	
	if (!localPlayer || !localPlayer->isAlive() || localPlayer->nextAttack() > memory->globalVars->serverTime() || localPlayer->isDefusing() || localPlayer->waitForNoAttack())
		return;

	auto activeWeapon = localPlayer->getActiveWeapon();
	if (!activeWeapon || !activeWeapon->clip())
		return;

	if (localPlayer->shotsFired() > 0 && !activeWeapon->isFullAuto())
		return;

	auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
	if (!weaponIndex)
		return;

	auto weaponClass = getWeaponClass(activeWeapon->itemDefinitionIndex2());
	if (!config->ragebot[weaponIndex].enabled)
		weaponIndex = weaponClass;

	if (!config->ragebot[weaponIndex].enabled)
		weaponIndex = 0;

	if (!config->ragebot[weaponIndex].betweenShots && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
		return;

	if (!config->ragebot[weaponIndex].enabled)
		return;

	if (config->ragebot[weaponIndex].onKey) {
		if (!config->aimbot[weaponIndex].keyMode) {
			if (!GetAsyncKeyState(config->ragebot[weaponIndex].key))
				return;
		}
		else {
			static bool toggle = true;
			if (GetAsyncKeyState(config->ragebot[weaponIndex].key) & 1)
				toggle = !toggle;
			if (!toggle)
				return;
		}
	}

	if (activeWeapon->isKnife() || activeWeapon->isNade()) 
		return;

	Entity* Target{};
	std::vector<VectorAndDamage> AimPoints;

	int bestCandidateDmg = 0;
	
	for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
		Entity* entity = interfaces->entityList->getEntity(i);
		
		if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get()) && !config->ragebot[weaponIndex].friendlyFire || entity->gunGameImmunity())
		{
			continue;	
		}

		int sumDmg = 0;
		int bestDmg = 0;
		
		std::vector<VectorAndDamage> Hitboxes = GetHitBoxes(entity, activeWeapon, weaponIndex, bestDamage, wallbangVector, sumDmg, bestDmg);
		
		if (!Hitboxes.empty())
		{
			if (bestDmg > bestCandidateDmg)
			{
				AimPoints = Hitboxes;
				Target = entity;
			}
		}
	}

	if (Target && Target->isAlive() && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime()) //if has taget
	{
		int bestDmg = 0;
		int bestChance = 0;
		Vector bestAngle = {0, 0, 0};
		Vector bestModifiedAngle {0, 0, 0};

		static float MinimumVelocity = 0.0f;
		MinimumVelocity = localPlayer->getActiveWeapon()->getWeaponData()->maxSpeedAlt * 0.34f;

		if (localPlayer->velocity().length() >= MinimumVelocity && config->ragebot[weaponIndex].autoStop && (localPlayer->flags() & PlayerFlags::ONGROUND))
		{
			Autostop(cmd);
		}
		
		if (activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime() && activeWeapon->isSniperRifle() && !localPlayer->isScoped() && config->ragebot[weaponIndex].autoScope)
		{
			cmd->buttons |= UserCmd::IN_ATTACK2; 
		}
			
		for (auto const& aimPoint: AimPoints) {
			int raysHit = 0;
			Vector Angle = Math::CalcAngle(localPlayer->getEyePosition(), aimPoint.vec);

			//No recoil
			Angle -= (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat());
			Vector bestVector {0, 0,0};
			
			if (HitChance(Angle, Target, activeWeapon, weaponIndex, cmd, config->ragebot[weaponIndex].hitChance, bestHitchance, firstWallToPen, raysHit, bestVector))
			{
				if (aimPoint.damage > bestDmg)
				{
					bestDmg = aimPoint.damage;
					bestChance = bestHitchance;
					bestAngle = Angle;
					bestModifiedAngle = bestVector;
				}
			}
		}

		if (bestDmg > 0 && (config->ragebot[weaponIndex].autoShot || cmd->buttons & UserCmd::IN_ATTACK))
		{
			cmd->viewangles = bestModifiedAngle;//bestAngle;

			if (!config->ragebot[weaponIndex].slient)
			{
				interfaces->engine->setViewAngles(cmd->viewangles);
			}
			
			cmd->buttons |= UserCmd::IN_ATTACK; //shoot
		}
		
	}
}




