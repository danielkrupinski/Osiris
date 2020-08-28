#pragma once
#include "../SDK/Entity.h"
#include "../SDK/matrix3x4.h"
#include "../SDK/Vector.h"
#include "../Hacks/Backtrack.h";
#include "../Hacks/Resolver.h";
#include "../Hacks/Tickbase.h";

struct UserCmd;

namespace Animations
{
	void update(UserCmd*, bool& sendPacket) noexcept;
	void real() noexcept;
	void fake() noexcept;
	void players() noexcept;
	void setupResolver(Entity*, Tickbase::Tick ) noexcept;
	void setup(Entity*, Backtrack::Record) noexcept;
	void finishSetup(Entity*) noexcept;


	struct Players
	{
		Players() : once(false), chokedPackets(0), simtime(0) { }
		matrix3x4 matrix[256];
		std::array<float, 24> poses;
		std::array<AnimationLayer, 15> networked_layers;
		float abs;
		float simtime;
		int chokedPackets;
		bool once;
	};

	struct Backup
	{
		Vector origin;
		Vector max;
		Vector mins;
		int* boneCache;
		int countBones;
		bool hasBackup{ false };
	};

	struct Datas
	{
		bool sendPacket;
		bool lby;
		bool gotMatrix;
		float fakeAngle;
		Vector viewangles;
		matrix3x4 fakematrix[256];
		std::array<Players, 65> player;
		std::array<Backup, 65> backup;
		std::array<Backup, 65> backupResolver;
		std::array<Backup, 65> lastest;
	};
	extern Datas data;
} 