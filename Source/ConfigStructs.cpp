#include <cstdlib>
#include <cstring>
#include <iomanip>

#include "ConfigStructs.h"

void to_json(json& j, const Color4& o, const Color4& dummy)
{
    if (o.color != dummy.color) {
        std::ostringstream s;
        s << '#' << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(o.color[0] * 255) << std::setw(2) << static_cast<int>(o.color[1] * 255) << std::setw(2) << static_cast<int>(o.color[2] * 255);
        j["Color"] = s.str();
        j["Alpha"] = o.color[3];
    }
    WRITE("Rainbow", rainbow);
    WRITE("Rainbow Speed", rainbowSpeed);
}

void to_json(json& j, const KeyBind& o, const KeyBind& dummy)
{
    if (o != dummy)
        j = o.toString();
}

void to_json(json& j, const KeyBindToggle& o, const KeyBindToggle& dummy)
{
    if (o != dummy)
        j = o.toString();
}

void to_json(json& j, const ColorToggle& o, const ColorToggle& dummy)
{
    to_json(j, o.asColor4(), dummy.asColor4());
    WRITE("Enabled", enabled);
}

void to_json(json& j, const Color3& o, const Color3& dummy)
{
    WRITE("Color", color);
    WRITE("Rainbow", rainbow);
    WRITE("Rainbow Speed", rainbowSpeed);
}

void to_json(json& j, const ColorToggle3& o, const ColorToggle3& dummy)
{
    to_json(j, o.asColor3(), dummy.asColor3());
    WRITE("Enabled", enabled);
}

void to_json(json& j, const ColorToggleThickness& o, const ColorToggleThickness& dummy)
{
    to_json(j, o.asColorToggle(), dummy.asColorToggle());
    WRITE("Thickness", thickness);
}

void to_json(json& j, const HealthBar& o, const HealthBar& dummy)
{
    to_json(j, static_cast<const ColorToggle&>(o), dummy);
    WRITE("Type", type);
}

void read(const json& j, const char* key, bool& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::boolean)
        val.get_to(o);
}

void read(const json& j, const char* key, float& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::number_float)
        val.get_to(o);
}

void read(const json& j, const char* key, int& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.is_number_integer())
        val.get_to(o);
}

void read(const json& j, const char* key, WeaponId& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.is_number_integer())
        val.get_to(o);
}

void read(const json& j, const char* key, KeyBind& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.is_string())
        o = KeyBind{ val.get<std::string>().c_str() };
}

void read(const json& j, const char* key, char* o, std::size_t size) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.is_string()) {
        std::strncpy(o, val.get<std::string>().c_str(), size);
        o[size - 1] = '\0';
    }
}

void from_json(const json& j, Color4& c)
{
    if (j.contains("Color")) {
        const auto& val = j["Color"];
        // old format -> [1.0f, 0.0f, 0.0f, 1.0f]
        // new format -> #ff0000 + alpha as float
        if (val.type() == value_t::array && val.size() == c.color.size()) {
            for (std::size_t i = 0; i < val.size(); ++i) {
                if (!val[i].empty())
                    val[i].get_to(c.color[i]);
            }
        } else if (val.type() == value_t::string) {
            const auto str = val.get<std::string>();
            if (str.length() == 7 && str[0] == '#') {
                const auto color = std::strtol(str.c_str() + 1, nullptr, 16);
                c.color[0] = ((color >> 16) & 0xFF) / 255.0f;
                c.color[1] = ((color >> 8) & 0xFF) / 255.0f;
                c.color[2] = (color & 0xFF) / 255.0f;
            }
            read(j, "Alpha", c.color[3]);
        }
    }

    read(j, "Rainbow", c.rainbow);
    read(j, "Rainbow Speed", c.rainbowSpeed);
}

void from_json(const json& j, ColorToggle& ct)
{
    from_json(j, ct.asColor4());
    read(j, "Enabled", ct.enabled);
}

void from_json(const json& j, Color3& c)
{
    read(j, "Color", c.color);
    read(j, "Rainbow", c.rainbow);
    read(j, "Rainbow Speed", c.rainbowSpeed);
}

void from_json(const json& j, ColorToggle3& ct)
{
    from_json(j, ct.asColor3());
    read(j, "Enabled", ct.enabled);
}

void from_json(const json& j, HealthBar& o)
{
    from_json(j, static_cast<ColorToggle&>(o));
    read(j, "Type", o.type);
}

void from_json(const json& j, ColorToggleThickness& ctt)
{
    from_json(j, ctt.asColorToggle());

    read(j, "Thickness", ctt.thickness);
}
