#pragma once

struct ColorA {
    std::array<float, 4> color{ 1.0f, 1.0f, 1.0f, 1.0f };
    bool rainbow = false;
    float rainbowSpeed = 0.6f;

    auto operator==(const ColorA& c) const
    {
        return color == c.color
            && rainbow == c.rainbow
            && rainbowSpeed == c.rainbowSpeed;
    }
};

struct ColorToggle : ColorA {
    bool enabled = false;

    auto operator==(const ColorToggle& ct) const
    {
        return static_cast<const ColorA&>(*this) == static_cast<const ColorA&>(ct)
            && enabled == ct.enabled;
    }
};

struct ColorToggleThickness : ColorToggle {
    ColorToggleThickness() = default;
    ColorToggleThickness(float thickness) : thickness{ thickness } { }
    float thickness = 1.0f;

    auto operator==(const ColorToggleThickness& ctt) const
    {
        return static_cast<const ColorToggle&>(*this) == static_cast<const ColorToggle&>(ctt)
            && thickness == ctt.thickness;
    }
};

struct ColorToggleRounding : ColorToggle {
    float rounding = 0.0f;

    auto operator==(const ColorToggleRounding& ctr) const
    {
        return static_cast<const ColorToggle&>(*this) == static_cast<const ColorToggle&>(ctr)
            && rounding == ctr.rounding;
    }
};

struct ColorToggleThicknessRounding : ColorToggleRounding {
    float thickness = 1.0f;

    auto operator==(const ColorToggleThicknessRounding& cttr) const
    {
        return static_cast<const ColorToggleRounding&>(*this) == static_cast<const ColorToggleRounding&>(cttr)
            && thickness == cttr.thickness;
    }
};

struct Font {
    int index = 0; // do not save
    std::string name;

    auto operator==(const Font& f) const
    {
        return index == f.index
            && name == f.name;
    }
};

struct Snapline : ColorToggleThickness {
    enum Type {
        Bottom = 0,
        Top,
        Crosshair
    };

    int type = Bottom;

    auto operator==(const Snapline& s) const
    {
        return static_cast<const ColorToggleThickness&>(*this) == static_cast<const ColorToggleThickness&>(s)
            && type == s.type;
    }
};

struct Box : ColorToggleThicknessRounding {
    enum Type {
        _2d = 0,
        _2dCorners,
        _3d,
        _3dCorners
    };

    int type = _2d;
    std::array<float, 3> scale{ 0.25f, 0.25f, 0.25f };

    auto operator==(const Box& b) const
    {
        return static_cast<const ColorToggleThicknessRounding&>(*this) == static_cast<const ColorToggleThicknessRounding&>(b)
            && type == b.type
            && scale == b.scale;
    }
};

struct Shared {
    bool enabled = false;
    Font font;
    Snapline snapline;
    Box box;
    ColorToggle name;
    float textCullDistance = 0.0f;

    auto operator==(const Shared& s) const
    {
        return enabled == s.enabled
            && font == s.font
            && snapline == s.snapline
            && box == s.box
            && name == s.name
            && textCullDistance == s.textCullDistance;
    }
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
    ColorToggleThickness skeleton;

    auto operator==(const Player& p) const
    {
        return static_cast<const Shared&>(*this) == static_cast<const Shared&>(p)
            && weapon == p.weapon
            && flashDuration == p.flashDuration
            && audibleOnly == p.audibleOnly
            && spottedOnly == p.spottedOnly
            && skeleton == p.skeleton;
    }

    auto& operator=(const Shared& s)
    {
        static_cast<Shared&>(*this) = s;
        return *this;
    }
};

struct Weapon : Shared {
    ColorToggle ammo;

    auto operator==(const Weapon& w) const
    {
        return static_cast<const Shared&>(*this) == static_cast<const Shared&>(w)
            && ammo == w.ammo;
    }

    auto& operator=(const Shared& s)
    {
        static_cast<Shared&>(*this) = s;
        return *this;
    }
};

struct Trail : ColorToggleThickness {
    enum Type {
        Line = 0,
        Circles,
        FilledCircles
    };

    int type = Line;
    float time = 2.0f;

    auto operator==(const Trail& t) const
    {
        return static_cast<const ColorToggleThickness&>(*this) == static_cast<const ColorToggleThickness&>(t)
            && type == t.type
            && time == t.time;
    }
};

struct Trails {
    bool enabled = false;

    Trail localPlayer;
    Trail allies;
    Trail enemies;

    auto operator==(const Trails& t) const
    {
        return enabled == t.enabled
            && localPlayer == t.localPlayer
            && allies == t.allies
            && enemies == t.enemies;
    }
};

struct Projectile : Shared {
    Trails trails;

    auto operator==(const Projectile& p) const
    {
        return static_cast<const Shared&>(*this) == static_cast<const Shared&>(p)
            && trails == p.trails;
    }

    auto& operator=(const Shared& s)
    {
        static_cast<Shared&>(*this) = s;
        return *this;
    }
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

    auto operator==(const PurchaseList& pl) const
    {
        return enabled == pl.enabled
            && onlyDuringFreezeTime == pl.onlyDuringFreezeTime
            && showPrices == pl.showPrices
            && noTitleBar == pl.noTitleBar
            && mode == pl.mode;
    }
};
