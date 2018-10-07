
#pragma once

struct surfacephysicsparams_t
{
	float friction;
	float elasticity;
	float density;
	float thickness;
	float dampening;
};

struct surfaceaudioparams_t
{
	float reflectivity; // like elasticity, but how much sound should be reflected by this surface
	float hardnessFactor; // like elasticity, but only affects impact sound choices
	float roughnessFactor; // like friction, but only affects scrape sound choices
	float roughThreshold; // surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float hardThreshold; // surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float hardVelocityThreshold; // collision velocity > this causes "hard" impacts, < this causes "soft" impacts
	float highPitchOcclusion; //a value betweeen 0 and 100 where 0 is not occluded at all and 100 is silent (except for any additional reflected sound)
	float midPitchOcclusion;
	float lowPitchOcclusion;
};

struct surfacesoundnames_t
{
	unsigned short walkStepLeft;
	unsigned short walkStepRight;
	unsigned short	runStepLeft;
	unsigned short	runStepRight;
	unsigned short impactSoft;
	unsigned short impactHard;
	unsigned short scrapeSmooth;
	unsigned short scrapeRough;
	unsigned short bulletImpact;
	unsigned short rolling;
	unsigned short breakSound;
	unsigned short strainSound;
};

struct surfacegameprops_t
{
public:
	float maxSpeedFactor;
	float jumpFactor;
	float flPenetrationModifier;
	float flDamageModifier;
	unsigned short material;
	byte climbable;
};

struct surfacedata_t
{
	surfacephysicsparams_t physics;
	surfaceaudioparams_t audio;
	surfacesoundnames_t sounds;
	surfacegameprops_t game;
};

class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps(void) {}
	virtual int ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;
	virtual int SurfacePropCount(void) const = 0;
	virtual int GetSurfaceIndex(const char *pSurfacePropName) const = 0;
	virtual void GetPhysicsProperties(int surfaceDataIndex, float *density, float thickness, float friction, float elasticity) const = 0;
	virtual surfacedata_t* GetSurfaceData(int surfaceDataIndex) = 0;
	virtual const char GetString(unsigned short stringTableIndex) const = 0;
	virtual const char GetPropName(int surfaceDataIndex) const = 0;
	virtual void SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;
	virtual void GetPhysicsParameters(int surfaceDataIndex, surfacephysicsparams_t *pParamsOut) const = 0;
};
