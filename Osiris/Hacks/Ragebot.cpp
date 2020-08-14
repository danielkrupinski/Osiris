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
static bool HitChance(Vector angles, Entity* entity, Entity* weapon, int weaponIndex, UserCmd* cmd, const int chance) noexcept
{
	if (!chance)
		return true;

	int hitseed = 256;

	int iHit = 0;
	int iHitsNeed = (int)((float)hitseed * ((float)chance / 100.f));
	bool bHitchance = false;

	Vector forward, right, up;
	Math::AngleVectors(angles, &forward, &right, &up);

	weapon->UpdateAccuracyPenalty();

	for (auto i = 0; i < hitseed; ++i) {

		float RandomA = Math::RandomFloat(0.0f, 1.0f);
		float RandomB = 1.0f - RandomA * RandomA;
		RandomA = Math::RandomFloat(0.0f, M_PIF * 2.0f);
		RandomB *= weapon->getSpread() + weapon->getInaccuracy();
		float SpreadX1 = (cos(RandomA) * RandomB);
		float SpreadY1 = (sin(RandomA) * RandomB);
		float RandomC = Math::RandomFloat(0.0f, 1.0f);
		float RandomF = RandomF = 1.0f - RandomC * RandomC;
		RandomC = Math::RandomFloat(0.0f, M_PIF * 2.0f);
		RandomF *= weapon->getSpread();
		float SpreadX2 = (cos(RandomC) * RandomF);
		float SpreadY2 = (sin(RandomC) * RandomF);
		float fSpreadX = SpreadX1 + SpreadX2;
		float fSpreadY = SpreadY1 + SpreadY2;

		Vector vSpreadForward;
		vSpreadForward.x = forward.x + (fSpreadX * right.x) + (fSpreadY * up.x);
		vSpreadForward.y = forward.y + (fSpreadX * right.y) + (fSpreadY * up.y);
		vSpreadForward.z = forward.z + (fSpreadX * right.z) + (fSpreadY * up.z);
		vSpreadForward.NormalizeInPlace();

		Vector qaNewAngle;
		Math::VectorAngles(vSpreadForward, qaNewAngle);
		qaNewAngle.normalize();

		Vector vEnd;
		Math::AngleVectors(qaNewAngle, &vEnd);
		vEnd = localPlayer->getEyePosition() + (vEnd * 8192.f);

		if (Autowall->PenetrateWall(entity, vEnd, weaponIndex))
			iHit++;

		if ((int)(((float)iHit / 256.f) * 100.f) >= chance) {
			bHitchance = true;
			break;
		}
		if ((256.f - 1 - i + iHit) < iHitsNeed)
			break;
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

Vector GetHitBoxes(Entity* entity, Entity* weapon, int weaponIndex)noexcept
{
	if (!localPlayer->isAlive())
		return Vector{ 0.0f,0.0f,0.0f };
	if (weapon->isKnife() || weapon->isNade()) //check weapon
		return Vector{ 0.0f,0.0f,0.0f };

	int bestHitbox = -1;
	std::vector<int> hitboxes;

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

	float minDamage = config->ragebot[weaponIndex].WallDamage;
	Vector BestPoint{};

	matrix3x4 matrix[256];
	if (!entity->setupBones(matrix, 256, 256, memory->globalVars->currenttime))
		return Vector{};

	for (auto HitBoxID : hitboxes)
	{
		for (auto point : GetMultiplePointsForHitbox(entity, HitBoxID, matrix, weaponIndex))
		{
			float damage = Autowall->Damage(point);

			if (damage > minDamage)
			{
				bestHitbox = HitBoxID;
				minDamage = damage;
				BestPoint = point;

				if (minDamage >= entity->health())
					return BestPoint;
			}
		}
	}

	return BestPoint;
}

void Ragebot::run(UserCmd* cmd)noexcept
{
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
	Vector AimPoint{};
	for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
		auto entity = interfaces->entityList->getEntity(i);
		if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()
			|| !entity->isOtherEnemy(localPlayer.get()) && !config->ragebot[weaponIndex].friendlyFire || entity->gunGameImmunity())
			continue;
		Vector Hitboxes = GetHitBoxes(entity, activeWeapon, weaponIndex);
		if (Hitboxes != Vector{ 0.f,0.f,0.f })
		{
			AimPoint = Hitboxes;
			Target = entity;
		}


	}

	if (Target) //if has taget
	{
		Vector Angle = Math::CalcAngle(localPlayer->getEyePosition(), AimPoint);
		static float MinimumVelocity = 0.0f;
		MinimumVelocity = localPlayer->getActiveWeapon()->getWeaponData()->maxSpeedAlt * .34f;

		if (localPlayer->velocity().length() >= MinimumVelocity && config->ragebot[weaponIndex].autoStop && (localPlayer->flags() & PlayerFlags::ONGROUND))
			Autostop(cmd); //Auto Stop

		if (activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime() && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
		{
			cmd->buttons |= UserCmd::IN_ATTACK2; //Auto Scope
			//return;
		}
#define M_Left  0x1 //M_LEFT
		if (cmd->buttons & UserCmd::IN_ATTACK && GetAsyncKeyState(M_Left)) //if localPlayer is using mouse to shoot
			return;
		auto canShoot = activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime();

		//No recoil
		Angle -= (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat());
		if (HitChance(Angle, Target, activeWeapon, weaponIndex, cmd, config->ragebot[weaponIndex].hitChance) && canShoot)
		{
			cmd->viewangles = Angle; //Set Angles

			if (!config->ragebot[weaponIndex].slient) 
				interfaces->engine->setViewAngles(cmd->viewangles);

			cmd->buttons |= UserCmd::IN_ATTACK; //shoot
		}
	}
}




