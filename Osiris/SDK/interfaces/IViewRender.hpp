#pragma once

enum DrawFlags_t
{
    DF_RENDER_REFRACTION = 0x1,
    DF_RENDER_REFLECTION = 0x2,
    DF_CLIP_Z = 0x4,
    DF_CLIP_BELOW = 0x8,
    DF_RENDER_UNDERWATER = 0x10,
    DF_RENDER_ABOVEWATER = 0x20,
    DF_RENDER_WATER = 0x40,
    DF_UNUSED1 = 0x100,
    DF_WATERHEIGHT = 0x200,
    DF_UNUSED2 = 0x400,
    DF_DRAWSKYBOX = 0x800,
    DF_FUDGE_UP = 0x1000,
    DF_DRAW_ENTITITES = 0x2000,
    DF_UNUSED3 = 0x4000,
    DF_UNUSED4 = 0x8000,
    DF_UNUSED5 = 0x10000,
    DF_SAVEGAMESCREENSHOT = 0x20000,
    DF_CLIP_SKYBOX = 0x40000,
    DF_SHADOW_DEPTH_MAP = 0x100000	// Currently rendering a shadow depth map
};


//-----------------------------------------------------------------------------
// Purpose: View setup and rendering
//-----------------------------------------------------------------------------
class CViewSetup;
class C_BaseEntity;
struct vrect_t;
class C_BaseViewModel;
class IMaterial;

class IViewRender
{
public:
    virtual void                Init(void) = 0;
    virtual void                LevelInit(void) = 0;
    virtual void                LevelShutdown(void) = 0;
    virtual void                Shutdown(void) = 0;
    virtual void                OnRenderStart() = 0;
    virtual	void                Render(vrect_t *rect) = 0;
    virtual void                RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;
    virtual int                 GetDrawFlags() = 0;
    virtual void                StartPitchDrift(void) = 0;
    virtual void                StopPitchDrift(void) = 0;
    virtual void*               GetFrustum() = 0;
    virtual bool                ShouldDrawBrushModels(void) = 0;
    virtual const CViewSetup*   GetPlayerViewSetup(void) const = 0;
    virtual const CViewSetup*   GetViewSetup(void) const = 0;
    virtual void                DisableVis(void) = 0;
    virtual int                 BuildWorldListsNumber() const = 0;
    virtual void                SetCheapWaterStartDistance(float flCheapWaterStartDistance) = 0;
    virtual void                SetCheapWaterEndDistance(float flCheapWaterEndDistance) = 0;
    virtual void                GetWaterLODParams(float &flCheapWaterStartDistance, float &flCheapWaterEndDistance) = 0;
    virtual void                DriftPitch(void) = 0;
    virtual void                SetScreenOverlayMaterial(IMaterial *pMaterial) = 0;
    virtual IMaterial*          GetScreenOverlayMaterial() = 0;
    virtual void                WriteSaveGameScreenshot(const char *pFilename) = 0;
    virtual void                WriteSaveGameScreenshotOfSize(const char *pFilename, int width, int height) = 0;
    virtual void                QueueOverlayRenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;
    virtual float               GetZNear() = 0;
    virtual float               GetZFar() = 0;
    virtual void                GetScreenFadeDistances(float *min, float *max) = 0;
    virtual C_BaseEntity*       GetCurrentlyDrawingEntity() = 0;
    virtual void                SetCurrentlyDrawingEntity(C_BaseEntity *pEnt) = 0;
};