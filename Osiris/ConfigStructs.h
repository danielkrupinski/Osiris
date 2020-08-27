#pragma once

#pragma pack(push, 1)
struct ColorA {
    std::array<float, 4> color{ 1.0f, 1.0f, 1.0f, 1.0f };
    float rainbowSpeed = 0.6f;
    bool rainbow = false;
};
#pragma pack(pop)

struct ColorToggle : ColorA {
    bool enabled = false;
};

struct ColorToggleThickness : ColorToggle {
    ColorToggleThickness() = default;
    ColorToggleThickness(float thickness) : thickness{ thickness } { }
    float thickness = 1.0f;
};

struct ColorToggleRounding : ColorToggle {
    float rounding = 0.0f;
};

struct ColorToggleThicknessRounding : ColorToggleRounding {
    float thickness = 1.0f;
};

struct Font {
    int index = 0; // do not save
    std::string name;
};

struct Snapline : ColorToggleThickness {
    enum Type {
        Bottom = 0,
        Top,
        Crosshair
    };

    int type = Bottom;
};

struct Box : ColorToggleRounding {
    enum Type {
        _2d = 0,
        _2dCorners,
        _3d,
        _3dCorners
    };

    int type = _2d;
    std::array<float, 3> scale{ 0.25f, 0.25f, 0.25f };
	ColorToggle fill{ 1.0f, 1.0f, 1.0f, 0.4f };
};

struct Shared {
    bool enabled = false;
    Font font;
    Snapline snapline;
    Box box;
    ColorToggle name;
    float textCullDistance = 0.0f;
};

struct Bar : ColorToggleRounding {

};

struct Player : Shared {
    Player() : Shared{}
    {
        box.type = Box::_2dCorners;
    }

    ColorToggle weapon;
    ColorToggle flashDuration;
    bool audibleOnly = false;
    bool spottedOnly = false;
	   bool healthBar = false;
    ColorToggleThickness skeleton;
    Box headBox;

    using Shared::operator=;
};

struct Weapon : Shared {
    ColorToggle ammo;

    using Shared::operator=;
};

struct Trail : ColorToggleThickness {
    enum Type {
        Line = 0,
        Circles,
        FilledCircles
    };

    int type = Line;
    float time = 2.0f;
};

struct Trails {
    bool enabled = false;

    Trail localPlayer;
    Trail allies;
    Trail enemies;
};



struct Projectile : Shared {
    Trails trails;

    using Shared::operator=;
};

struct PurchaseList {
    bool enabled = false;
    bool onlyDuringFreezeTime = false;
    bool showPrices = false;
    bool noTitleBar = false;

    enum Mode {
        Details = 0,
        Summary
    };
    int mode = Details;
};

struct ImGuiStruct
{
    bool enabled{ false };
    bool noBackGround{ false };
    bool noTittleBar{ false };
    
};

struct StatusBar : ImGuiStruct
{
	bool ShowPlayerRealViewAngles{ false };
	bool ShowPlayerStatus{ false };
	bool ShowGameGlobalVars{ false };

};
