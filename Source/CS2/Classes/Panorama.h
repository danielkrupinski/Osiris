#pragma once

#include <cstdint>

#include <Platform/Macros/IsPlatform.h>

#include "CUtlVector.h"
#include "Vector.h"

namespace cs2
{

using CPanoramaSymbol = short;

struct PanelHandle {
    std::int32_t panelIndex = -1;
    std::uint32_t serialNumber;

    [[nodiscard]] bool isValid() const noexcept
    {
        return panelIndex != -1;
    }
};

struct CUIPanel;

struct CUIEngine {
#if IS_WIN64()
    using getPanelHandle = void (*)(CUIEngine* thisptr, PanelHandle* handle, CUIPanel* panel);
#elif IS_LINUX()
    using getPanelHandle = PanelHandle (*)(CUIEngine* thisptr, CUIPanel* panel);
#endif
    using getPanelPointer = CUIPanel* (*)(CUIEngine* thisptr, const PanelHandle* handle);
    using runScript = void (*)(CUIEngine* thisptr, CUIPanel* contextPanel, const char* scriptSource, const char* originFile, std::uint64_t line);
    using makeSymbol = CPanoramaSymbol (*)(CUIEngine* thisptr, int type, const char* text);
    using onDeletePanel = void (*)(CUIEngine* thisptr, PanelHandle* panelHandle);
};

struct CPanel2D {
    const void* vmt;
    CUIPanel* uiPanel;
};

struct CLabel : CPanel2D {
    using setTextInternal = void (*)(CLabel* thisptr, const char* value, int textType, bool trustedSource);
};

struct CPanelStyle {};

struct CUIPanel {
    const void* vmt;
    CPanel2D* clientPanel;

    using setParent = void (*)(CUIPanel* thisptr, CUIPanel* parent);
    using setVisible = void (*)(CUIPanel* thisptr, bool visible);
    using findChildInLayoutFile = CUIPanel* (*)(CUIPanel* thisptr, const char* childId);
    using getAttributeString = const char* (*)(CUIPanel* thisptr, CPanoramaSymbol attributeName, const char* defaultValue);
    using setAttributeString = void (*)(CUIPanel* thisptr, CPanoramaSymbol attributeName, const char* value);

    using childrenVector = CUtlVector<CUIPanel*>;
    using classesVector = CUtlVector<CPanoramaSymbol>;
};

struct CImagePanel : CPanel2D {
    using setImage = void (*)(CImagePanel* thisptr, const char* imageUrl);
};

struct CUILength {
    enum EUILengthTypes {
        k_EUILengthUnset,
        k_EUILengthLength,
        k_EUILengthPercent
    };

    float m_flValue;
    EUILengthTypes m_eType;
};

struct CTransform3D {
    const void* vmt;
};

struct CTransformTranslate3D : CTransform3D {
    CUILength m_x;
    CUILength m_y;
    CUILength m_z;
};

struct CTransformScale3D : CTransform3D {
    Vector m_VecScale;
};

}
