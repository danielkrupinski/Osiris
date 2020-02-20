#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/spectrum.h"
#include "imgui/style.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Reportbot.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"
#include "SDK/InputSystem.h"

#define U8(S) reinterpret_cast<const char *>(u8##S)

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

GUI::GUI() noexcept
{
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));

	// ImGui::StyleColorsDark();
	ImGui::Spectrum::StyleColorsSpectrum();
	ImGuiStyle &style = ImGui::GetStyle();

	style.ScrollbarSize = 9.0f;

	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts)))
	{
		const std::filesystem::path path{pathToFonts};
		CoTaskMemFree(pathToFonts);

		io.Fonts->AddFontFromFileTTF((path / "msyh.ttc").string().c_str(), 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	}
}

void GUI::render() noexcept
{
	if (!config.style.menuStyle)
	{
		renderMenuBar();
		renderAimbotWindow();
		renderAntiAimWindow();
		renderTriggerbotWindow();
		renderBacktrackWindow();
		renderGlowWindow();
		renderChamsWindow();
		renderEspWindow();
		renderVisualsWindow();
		renderSkinChangerWindow();
		renderSoundWindow();
		renderStyleWindow();
		renderMiscWindow();
		renderReportbotWindow();
		renderConfigWindow();
	}
	else
	{
		renderGuiStyle2();
	}
}

void GUI::updateColors() const noexcept
{
	switch (config.style.menuColors)
	{
	case 0:
		ImGui::StyleColorsDark();
		break;
	case 1:
		ImGui::StyleColorsLight();
		break;
	case 2:
		ImGui::StyleColorsClassic();
		break;
	case 3:
		ImGui::Spectrum::StyleColorsSpectrum();
		break;
	case 4:
		ImGui::CustomStyle::StyleAtropurpureus();
		break;
	}
}

void GUI::hotkey(int &key) noexcept
{
	key ? ImGui::Text("[ %s ]", interfaces.inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ key ]");

	if (!ImGui::IsItemHovered())
		return;

	ImGui::SetTooltip("Press any key to change keybind");
	ImGuiIO &io = ImGui::GetIO();
	for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
		if (ImGui::IsKeyPressed(i) && i != config.misc.menuKey)
			key = i != VK_ESCAPE ? i : 0;

	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config.misc.menuKey)
			key = i + (i > 1 ? 2 : 1);
}

void GUI::renderMenuBar() noexcept
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::MenuItem(U8("自瞄"), nullptr, &window.aimbot);
		ImGui::MenuItem(U8("陀螺"), nullptr, &window.antiAim);
		ImGui::MenuItem(U8("自动开枪"), nullptr, &window.triggerbot);
		ImGui::MenuItem(U8("回溯"), nullptr, &window.backtrack);
		ImGui::MenuItem(U8("实体发光"), nullptr, &window.glow);
		ImGui::MenuItem(U8("人物发光"), nullptr, &window.chams);
		ImGui::MenuItem(U8("透视与玩家信息(非发光)"), nullptr, &window.esp);
		ImGui::MenuItem(U8("视觉"), nullptr, &window.visuals);
		ImGui::MenuItem(U8("皮肤修改器"), nullptr, &window.skinChanger);
		ImGui::MenuItem(U8("声效修改"), nullptr, &window.sound);
		ImGui::MenuItem(U8("UI风格"), nullptr, &window.style);
		ImGui::MenuItem(U8("杂项"), nullptr, &window.misc);
		ImGui::MenuItem(U8("举报机器人"), nullptr, &window.reportbot);
		ImGui::MenuItem(U8("设置"), nullptr, &window.config);
		ImGui::EndMainMenuBar();
	}
}

void GUI::renderAimbotWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.aimbot)
			return;
		ImGui::SetNextWindowSize({600.0f, 0.0f});
		ImGui::Begin(U8("自瞄"), &window.aimbot, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("全部\0手枪\0重型武器\0冲锋枪\0步枪\0"));
	ImGui::PopID();
	ImGui::SameLine();
	static int currentWeapon{0};
	ImGui::PushID(1);

	switch (currentCategory)
	{
	case 0:
		currentWeapon = 0;
		ImGui::NewLine();
		break;
	case 1:
	{
		static int currentPistol{0};
		static const char *pistols[]{
			U8("全部"), U8("格洛克18"), U8("P2000"),
			U8("USP-S"), U8("双持贝瑞塔"), U8("P250"),
			U8("Tec-9"), U8("FN57"), U8("CZ-75自动手枪"),
			U8("沙漠之鹰"), U8("左轮手枪")};

		ImGui::Combo("", &currentPistol, [](void *data, int idx, const char **out_text) {
			if (config.aimbot[idx ? idx : 35].enabled)
			{
				static std::string name;
				name = pistols[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = pistols[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(pistols));

		currentWeapon = currentPistol ? currentPistol : 35;
		break;
	}
	case 2:
	{
		static int currentHeavy{0};
		static const char *heavies[]{
			U8("所有"), U8("新星"), U8("XM1014"),
			U8("截短霰弹枪"), U8("MAG-7"), U8("M249"),
			U8("内格夫")};

		ImGui::Combo("", &currentHeavy, [](void *data, int idx, const char **out_text) {
			if (config.aimbot[idx ? idx + 10 : 36].enabled)
			{
				static std::string name;
				name = heavies[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = heavies[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(heavies));

		currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
		break;
	}
	case 3:
	{
		static int currentSmg{0};
		static const char *smgs[]{
			U8("全部"), U8("Mac-10"), U8("MP9"),
			U8("MP7"), U8("MP5-SD"), U8("UMP-45"),
			U8("P90"), U8("PP野牛")};

		ImGui::Combo("", &currentSmg, [](void *data, int idx, const char **out_text) {
			if (config.aimbot[idx ? idx + 16 : 37].enabled)
			{
				static std::string name;
				name = smgs[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = smgs[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(smgs));

		currentWeapon = currentSmg ? currentSmg + 16 : 37;
		break;
	}
	case 4:
	{
		static int currentRifle{0};
		static const char *rifles[]{
			U8("全部"), U8("加利尔 AR"), U8("法玛斯"),
			U8("AK-47"), U8("M4A4"), U8("M4A1-S"),
			U8("SSG-08"), U8("SG-553"), U8("AUG"),
			U8("AWP"), U8("G3SG1"), U8("SCAR-20")};

		ImGui::Combo("", &currentRifle, [](void *data, int idx, const char **out_text) {
			if (config.aimbot[idx ? idx + 23 : 38].enabled)
			{
				static std::string name;
				name = rifles[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = rifles[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(rifles));

		currentWeapon = currentRifle ? currentRifle + 23 : 38;
		break;
	}
	}
	ImGui::PopID();
	ImGui::SameLine();
	ImGui::Checkbox(U8("开启"), &config.aimbot[currentWeapon].enabled);
	ImGui::Separator();
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 220.0f);
	ImGui::Checkbox(U8("按键"), &config.aimbot[currentWeapon].onKey);
	ImGui::SameLine();
	hotkey(config.aimbot[currentWeapon].key);
	ImGui::SameLine();
	ImGui::PushID(2);
	ImGui::PushItemWidth(70.0f);
	ImGui::Combo("", &config.aimbot[currentWeapon].keyMode, "Hold\0Toggle\0");
	ImGui::PopItemWidth();
	ImGui::PopID();
	ImGui::Checkbox(U8("自瞄锁定"), &config.aimbot[currentWeapon].aimlock);
	ImGui::Checkbox(U8("安全模式"), &config.aimbot[currentWeapon].silent);
	ImGui::Checkbox(U8("无视队友"), &config.aimbot[currentWeapon].friendlyFire);
	ImGui::Checkbox(U8("可视瞄准"), &config.aimbot[currentWeapon].visibleOnly);
	ImGui::Checkbox(U8("只在开镜时启动"), &config.aimbot[currentWeapon].scopedOnly);
	ImGui::Checkbox(U8("无视闪光"), &config.aimbot[currentWeapon].ignoreFlash);
	ImGui::Checkbox(U8("无视闪光"), &config.aimbot[currentWeapon].ignoreSmoke);
	ImGui::Checkbox(U8("自动开枪"), &config.aimbot[currentWeapon].autoShot);
	ImGui::Checkbox(U8("自动开镜"), &config.aimbot[currentWeapon].autoScope);
	ImGui::Checkbox(U8("后坐力"), &config.aimbot[currentWeapon].recoilbasedFov);
	ImGui::Combo(U8("瞄准位置"), &config.aimbot[currentWeapon].bone,
				 U8("距离准星最近\0最高伤害\0头\0脖子\0胸骨\0胸\0腹部\0下体(JB)\0"));
	ImGui::NextColumn();
	ImGui::PushItemWidth(240.0f);
	ImGui::SliderFloat(U8("视角"), &config.aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", 2.5f);
	ImGui::SliderFloat(U8("平滑度"), &config.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
	ImGui::SliderFloat(U8("最大X后坐力"), &config.aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat(U8("最大Y后坐力"), &config.aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat(U8("最大瞄准误差"), &config.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
	ImGui::SliderFloat(U8("最大开枪误差"), &config.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
	ImGui::InputInt(U8("最小伤害"), &config.aimbot[currentWeapon].minDamage);
	config.aimbot[currentWeapon].minDamage = std::clamp(config.aimbot[currentWeapon].minDamage, 0, 250);
	ImGui::Checkbox(U8("瞬间击杀"), &config.aimbot[currentWeapon].killshot);
	ImGui::Checkbox(U8("穿墙开枪"), &config.aimbot[currentWeapon].betweenShots);
	ImGui::Columns(1);
	if (!contentOnly)
		ImGui::End();
}

void GUI::renderAntiAimWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.antiAim)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("陀螺"), &window.antiAim, windowFlags);
	}
	ImGui::Checkbox(U8("开启"), &config.antiAim.enabled);
	ImGui::Checkbox("##pitch", &config.antiAim.pitch);
	ImGui::SameLine();
	ImGui::SliderFloat(U8("倾斜度 (Pitch)"), &config.antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
	ImGui::Checkbox(U8("偏移度 (Yaw)"), &config.antiAim.yaw);
	if (!contentOnly)
		ImGui::End();
}

void GUI::renderTriggerbotWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.triggerbot)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("自动开枪"), &window.triggerbot, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("全部\0手枪\0重型武器\0冲锋枪\0步枪\0电击枪\0"));
	ImGui::PopID();
	ImGui::SameLine();
	static int currentWeapon{0};
	ImGui::PushID(1);
	switch (currentCategory)
	{
	case 0:
		currentWeapon = 0;
		ImGui::NewLine();
		break;
	case 5:
		currentWeapon = 39;
		ImGui::NewLine();
		break;

	case 1:
	{
		static int currentPistol{0};
		static const char *pistols[]{
			U8("全部"), U8("格洛克18"), U8("P2000"),
			U8("USP-S"), U8("双持贝瑞塔"), U8("P250"),
			U8("Tec-9"), U8("FN57"), U8("CZ-75自动手枪"),
			U8("沙漠之鹰"), U8("左轮手枪")};

		ImGui::Combo("", &currentPistol, [](void *data, int idx, const char **out_text) {
			if (config.triggerbot[idx ? idx : 35].enabled)
			{
				static std::string name;
				name = pistols[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = pistols[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(pistols));

		currentWeapon = currentPistol ? currentPistol : 35;
		break;
	}
	case 2:
	{
		static int currentHeavy{0};
		static const char *heavies[]{
			U8("所有"), U8("新星"), U8("XM1014"),
			U8("截短霰弹枪"), U8("MAG-7"), U8("M249"),
			U8("内格夫")};

		ImGui::Combo("", &currentHeavy, [](void *data, int idx, const char **out_text) {
			if (config.triggerbot[idx ? idx + 10 : 36].enabled)
			{
				static std::string name;
				name = heavies[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = heavies[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(heavies));

		currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
		break;
	}
	case 3:
	{
		static int currentSmg{0};
		static const char *smgs[]{
			U8("全部"), U8("Mac-10"), U8("MP9"),
			U8("MP7"), U8("MP5-SD"), U8("UMP-45"),
			U8("P90"), U8("PP野牛")};

		ImGui::Combo("", &currentSmg, [](void *data, int idx, const char **out_text) {
			if (config.triggerbot[idx ? idx + 16 : 37].enabled)
			{
				static std::string name;
				name = smgs[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = smgs[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(smgs));

		currentWeapon = currentSmg ? currentSmg + 16 : 37;
		break;
	}
	case 4:
	{
		static int currentRifle{0};
		static const char *rifles[]{
			U8("全部"), U8("加利尔 AR"), U8("法玛斯"),
			U8("AK-47"), U8("M4A4"), U8("M4A1-S"),
			U8("SSG-08"), U8("SG-553"), U8("AUG"),
			U8("AWP"), U8("G3SG1"), U8("SCAR-20")};

		ImGui::Combo("", &currentRifle, [](void *data, int idx, const char **out_text) {
			if (config.triggerbot[idx ? idx + 23 : 38].enabled)
			{
				static std::string name;
				name = rifles[idx];
				*out_text = name.append(" *").c_str();
			}
			else
			{
				*out_text = rifles[idx];
			}
			return true;
		},
					 nullptr, IM_ARRAYSIZE(rifles));

		currentWeapon = currentRifle ? currentRifle + 23 : 38;
		break;
	}
	}
	ImGui::PopID();
	ImGui::SameLine();
	ImGui::Checkbox(U8("开启"), &config.triggerbot[currentWeapon].enabled);
	ImGui::Separator();
	ImGui::Checkbox(U8("按键"), &config.triggerbot[currentWeapon].onKey);
	ImGui::SameLine();
	hotkey(config.triggerbot[currentWeapon].key);
	ImGui::Checkbox(U8("无视队友"), &config.triggerbot[currentWeapon].friendlyFire);
	ImGui::Checkbox(U8("只在开镜时启动"), &config.triggerbot[currentWeapon].scopedOnly);
	ImGui::Checkbox(U8("无视闪光"), &config.triggerbot[currentWeapon].ignoreFlash);
	ImGui::Checkbox(U8("无视烟雾"), &config.triggerbot[currentWeapon].ignoreSmoke);
	ImGui::SetNextItemWidth(85.0f);
	ImGui::Combo(U8("判断部位"), &config.triggerbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
	ImGui::PushItemWidth(220.0f);
	ImGui::SliderInt(U8("延迟开枪"), &config.triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
	ImGui::InputInt(U8("最小伤害"), &config.triggerbot[currentWeapon].minDamage);
	config.triggerbot[currentWeapon].minDamage = std::clamp(config.triggerbot[currentWeapon].minDamage, 0, 250);
	ImGui::Checkbox(U8("瞬间击杀"), &config.triggerbot[currentWeapon].killshot);

	if (!contentOnly)
		ImGui::End();
}

void GUI::renderBacktrackWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.backtrack)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("回溯"), &window.backtrack, windowFlags);
	}
	ImGui::Checkbox(U8("开启"), &config.backtrack.enabled);
	ImGui::Checkbox(U8("无视烟雾"), &config.backtrack.ignoreSmoke);
	ImGui::Checkbox(U8("后坐力视角"), &config.backtrack.recoilBasedFov);
	ImGui::PushItemWidth(220.0f);
	ImGui::SliderInt(U8("延迟"), &config.backtrack.timeLimit, 1, 200, "%d ms");
	ImGui::PopItemWidth();
	if (!contentOnly)
		ImGui::End();
}

void GUI::renderGlowWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.glow)
			return;
		ImGui::SetNextWindowSize({450.0f, 0.0f});
		ImGui::Begin(U8("实体发光"), &window.glow, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("队友\0敌人\0正在下包的实体\0正在拆包的实体\0本地玩家\0所有武器\0C4\0安装了的C4\0鸡\0拆包工具\0投掷物\0人质\0尸体\0"));
	ImGui::PopID();
	static int currentItem{0};
	if (currentCategory <= 3)
	{
		ImGui::SameLine();
		static int currentType{0};
		ImGui::PushID(1);
		ImGui::Combo("", &currentType, U8("全部\0可视的\0墙后的\0"));
		ImGui::PopID();
		currentItem = currentCategory * 3 + currentType;
	}
	else
	{
		currentItem = currentCategory + 8;
	}

	ImGui::SameLine();
	ImGui::Checkbox(U8("开启"), &config.glow[currentItem].enabled);
	ImGui::Separator();
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 150.0f);
	ImGui::Checkbox(U8("血量"), &config.glow[currentItem].healthBased);

	ImGuiCustom::colorPicker(U8("颜色"), config.glow[currentItem].color.color, nullptr, &config.glow[currentItem].color.rainbow, &config.glow[currentItem].color.rainbowSpeed);

	ImGui::NextColumn();
	ImGui::PushItemWidth(220.0f);
	ImGui::SliderFloat(U8("粗细"), &config.glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat(U8("透明度"), &config.glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
	ImGui::SliderInt(U8("样式"), &config.glow[currentItem].style, 0, 3);
	ImGui::Columns(1);
	if (!contentOnly)
		ImGui::End();
}

void GUI::renderChamsWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.chams)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("人物发光"), &window.chams, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("队友\0敌人\0正在下包的人\0正在拆包的人\0本地玩家\0武器\0手\0回溯\0袖子\0"));
	ImGui::PopID();
	static int currentItem{0};

	if (currentCategory <= 3)
	{
		ImGui::SameLine();
		static int currentType{0};
		ImGui::PushID(1);
		ImGui::Combo("", &currentType, U8("全部\0可视的\0墙后的\0"));
		ImGui::PopID();
		currentItem = currentCategory * 3 + currentType;
	}
	else
	{
		currentItem = currentCategory + 8;
	}

	ImGui::SameLine();
	static int material = 1;

	if (ImGui::ArrowButton("##left", ImGuiDir_Left) && material > 1)
		--material;
	ImGui::SameLine();
	ImGui::Text("%d", material);
	ImGui::SameLine();
	if (ImGui::ArrowButton("##right", ImGuiDir_Right) && material < int(config.chams[0].materials.size()))
		++material;

	ImGui::SameLine();
	auto &chams{config.chams[currentItem].materials[material - 1]};

	ImGui::Checkbox(U8("开启"), &chams.enabled);
	ImGui::Separator();
	ImGui::Checkbox(U8("血量"), &chams.healthBased);
	ImGui::Checkbox(U8("人物闪烁"), &chams.blinking);
	ImGui::Combo(U8("模型材料"), &chams.material, U8("普通\0扁平\0活的\0白金色\0玻璃\0铬黄色\0透明色\0白银色\0黄金色\0塑料\0"));
	ImGui::Checkbox(U8("网格框架渲染"), &chams.wireframe);
	ImGuiCustom::colorPicker(U8("颜色"), chams.color.color, nullptr, &chams.color.rainbow, &chams.color.rainbowSpeed);
	ImGui::SetNextItemWidth(220.0f);
	ImGui::SliderFloat(U8("透明度"), &chams.alpha, 0.0f, 1.0f, "%.2f");

	if (!contentOnly)
	{
		ImGui::End();
	}
}

void GUI::renderEspWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.esp)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("透视与玩家信息（非发光）"), &window.esp, windowFlags);
	}

	static int currentCategory = 0;
	static int currentItem = 0;

	if (ImGui::ListBoxHeader("##", {125.0f, 300.0f}))
	{
		static constexpr const char *players[]{"All", "Visible", "Occluded"};
		ImGui::Text(U8("痛击我的队友"));
		ImGui::Indent();
		ImGui::PushID("Allies");

		for (int i = 0; i < IM_ARRAYSIZE(players); i++)
		{
			bool isSelected = currentCategory == 0 && currentItem == i;

			if ((i == 0 || !config.esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected))
			{
				currentItem = i;
				currentCategory = 0;
			}
		}

		ImGui::PopID();
		ImGui::Unindent();
		ImGui::Text(U8("暴击我的敌人"));
		ImGui::Indent();
		ImGui::PushID("Enemies");

		for (int i = 0; i < IM_ARRAYSIZE(players); i++)
		{
			bool isSelected = currentCategory == 1 && currentItem == i;

			if ((i == 0 || !config.esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected))
			{
				currentItem = i;
				currentCategory = 1;
			}
		}

		ImGui::PopID();
		ImGui::Unindent();
		if (bool isSelected = currentCategory == 2; ImGui::Selectable(U8("武器"), isSelected))  
		currentCategory = 2;
		ImGui::Text(U8("爆裂物"));
		ImGui::Indent();
		ImGui::PushID("Projectiles");
		static  const char *projectiles[]{U8("闪光弹"), U8("高爆手雷"), U8("遥控炸弹"),U8("弹跳地雷"), U8("诱饵雷"), U8("匪的火"), U8("战术侦测手雷"), U8("烟雾弹"), U8("雪球")};
		
		for (int i = 0; i < IM_ARRAYSIZE(projectiles); i++)
		{
			bool isSelected = currentCategory == 3 && currentItem == i;

			if (ImGui::Selectable(projectiles[i], isSelected))
			{
				currentItem = i;
				currentCategory = 3;
			}
		}

		ImGui::PopID();
		ImGui::Unindent();

		ImGui::Text(U8("吃鸡"));
		ImGui::Indent();
		ImGui::PushID("Danger Zone");
		static  const char *dangerZone[]{U8("哨兵"), U8("无人机"), U8("现金"), U8("一堆现金"), U8("手枪箱子"), U8("灯箱"), U8("重型箱子"), U8("炸药箱"), U8("道具箱"), U8("全甲"), U8("半甲"), U8("防弹头盔"), U8("降落伞"), U8("公文包"), U8("平板"), U8("兔子跳"), U8("弹药箱"), U8("雷达干扰")};

		for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++)
		{
			bool isSelected = currentCategory == 4 && currentItem == i;

			if (ImGui::Selectable(dangerZone[i], isSelected))
			{
				currentItem = i;
				currentCategory = 4;
			}
		}

		ImGui::PopID();
		ImGui::ListBoxFooter();
	}
	ImGui::SameLine();
	if (ImGui::BeginChild("##child", {400.0f, 0.0f}))
	{
		switch (currentCategory)
		{
		case 0:
		case 1:
		{
			int selected = currentCategory * 3 + currentItem;
			ImGui::Checkbox(U8("启动"), &config.esp.players[selected].enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("front", &config.esp.players[selected].font, 1, 294);
			config.esp.players[selected].font = std::clamp(config.esp.players[selected].font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("连线"), config.esp.players[selected].snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("人物方框"), config.esp.players[selected].box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.players[selected].boxType, "2D\0"
																	"2D corners\0"
																	"3D\0"
																	"3D corners\0");
			ImGuiCustom::colorPicker(U8("眼睛"), config.esp.players[selected].eyeTraces);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("血量"), config.esp.players[selected].health);
			ImGuiCustom::colorPicker(U8("脑瓜袋"), config.esp.players[selected].headDot);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("血量bar"), config.esp.players[selected].healthBar);
			ImGuiCustom::colorPicker(U8("名字"), config.esp.players[selected].name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("护甲"), config.esp.players[selected].armor);
			ImGuiCustom::colorPicker(U8("钱"), config.esp.players[selected].money);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("护甲bar"), config.esp.players[selected].armorBar);
			ImGuiCustom::colorPicker(U8("轮廓"), config.esp.players[selected].outline);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("距离"), config.esp.players[selected].distance);
			ImGuiCustom::colorPicker(U8("武器"),config.esp.players[selected].activeWeapon);
			ImGui::SameLine(spacing);
			ImGui::Checkbox(U8("透死人"), &config.esp.players[selected].deadesp);
			ImGui::SliderFloat(U8("最大距离"), &config.esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
			break;
		}
		case 2:
		{
			ImGui::Checkbox(U8("启动"), &config.esp.weapon.enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("Font", &config.esp.weapon.font, 1, 294);
			config.esp.weapon.font = std::clamp(config.esp.weapon.font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("连线"), config.esp.weapon.snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("方框"), config.esp.weapon.box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.weapon.boxType, "2D\0"
														 "2D corners\0"
														 "3D\0"
														 "3D corners\0");
			ImGuiCustom::colorPicker(U8("名字"), config.esp.weapon.name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("轮廓"), config.esp.weapon.outline);
			ImGuiCustom::colorPicker(U8("距离"), config.esp.weapon.distance);
			ImGui::SliderFloat(U8("最大距离"), &config.esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
			break;
		}
		case 3:
		{
			ImGui::Checkbox(U8("启动"), &config.esp.projectiles[currentItem].enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("Font", &config.esp.projectiles[currentItem].font, 1, 294);
			config.esp.projectiles[currentItem].font = std::clamp(config.esp.projectiles[currentItem].font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("连线"), config.esp.projectiles[currentItem].snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("方框"), config.esp.projectiles[currentItem].box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.projectiles[currentItem].boxType, "2D\0"
																		   "2D corners\0"
																		   "3D\0"
																		   "3D corners\0");
			ImGuiCustom::colorPicker(U8("名字"), config.esp.projectiles[currentItem].name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("轮廓"), config.esp.projectiles[currentItem].outline);
			ImGuiCustom::colorPicker(U8("距离"), config.esp.projectiles[currentItem].distance);
			ImGui::SliderFloat(U8("最大距离"), &config.esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
			break;
		}
		case 4:
		{
			int selected = currentItem;
			ImGui::Checkbox(U8("启动"), &config.esp.dangerZone[selected].enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("Font", &config.esp.dangerZone[selected].font, 1, 294);
			config.esp.dangerZone[selected].font = std::clamp(config.esp.dangerZone[selected].font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("连线"), config.esp.dangerZone[selected].snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("方框"), config.esp.dangerZone[selected].box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.dangerZone[selected].boxType, "2D\0"
																	   "2D corners\0"
																	   "3D\0"
																	   "3D corners\0");
			ImGuiCustom::colorPicker(U8("名字"), config.esp.dangerZone[selected].name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("轮廓"), config.esp.dangerZone[selected].outline);
			ImGuiCustom::colorPicker(U8("距离"), config.esp.dangerZone[selected].distance);
			ImGui::SliderFloat(U8("最大距离"), &config.esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
			break;
		}
		}

		ImGui::EndChild();
	}

	if (!contentOnly)
		ImGui::End();
}

void GUI::renderVisualsWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.visuals)
			return;
		ImGui::SetNextWindowSize({680.0f, 0.0f});
		ImGui::Begin(U8("可视化"), &window.visuals, windowFlags);
	}
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 280.0f);
	ImGui::Combo(U8("匪の人物模型"), &config.visuals.playerModelT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
	ImGui::Combo(U8("警の人物模型"), &config.visuals.playerModelCT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
	ImGui::Checkbox(U8("禁用后处理"), &config.visuals.disablePostProcessing);
	ImGui::Checkbox(U8("操你妈给爷飞（尸体螺旋上天）"), &config.visuals.inverseRagdollGravity);
	ImGui::Checkbox(U8("没有雾"), &config.visuals.noFog);
	ImGui::Checkbox(U8("关闭3D天空"), &config.visuals.no3dSky);
	ImGui::Checkbox(U8("无后座"), &config.visuals.noAimPunch);
	ImGui::Checkbox(U8("看起来没有后座（然并卵）"), &config.visuals.noViewPunch);
	ImGui::Checkbox(U8("剁手（隐藏手）"), &config.visuals.noHands);
	ImGui::Checkbox(U8("脱衣服（隐藏袖子）"), &config.visuals.noSleeves);
	ImGui::Checkbox(U8("空气枪（隐藏武器）"), &config.visuals.noWeapons);
	ImGui::Checkbox(U8("透烟"), &config.visuals.noSmoke);
	ImGui::Checkbox(U8("莎普爱思（无模糊）"), &config.visuals.noBlur);
	ImGui::Checkbox(U8("莎普爱思滴一滴开镜黑边立不见！奥里给！"), &config.visuals.noScopeOverlay);
	ImGui::Checkbox(U8("牛吃草(没有草)"), &config.visuals.noGrass);
	ImGui::Checkbox(U8("没有阴影"), &config.visuals.noShadows);
	ImGui::Checkbox(U8("网格显示烟雾"), &config.visuals.wireframeSmoke);
	ImGui::NextColumn();
	ImGui::Checkbox(U8("心有多带视野就有多带"), &config.visuals.zoom);
	ImGui::SameLine();
	hotkey(config.visuals.zoomKey);
	ImGui::Checkbox(U8("第三人称"), &config.visuals.thirdperson);
	ImGui::SameLine();
	hotkey(config.visuals.thirdpersonKey);
	ImGui::PushItemWidth(290.0f);
	ImGui::PushID(0);
	ImGui::SliderInt("", &config.visuals.thirdpersonDistance, 0, 1000, U8("第三人称距离: %d"));
	ImGui::PopID();
	ImGui::PushID(1);
	ImGui::SliderInt("", &config.visuals.viewmodelFov, -60, 60, U8("鸡巴有多长手臂就有多长: %d"));
	ImGui::PopID();
	ImGui::PushID(2);
	ImGui::SliderInt("", &config.visuals.fov, -60, 60, U8("视野大小: %d"));
	ImGui::PopID();
	ImGui::PushID(3);
	ImGui::SliderInt("", &config.visuals.farZ, 0, 2000, U8("渲染距离: %d"));
	ImGui::PopID();
	ImGui::PushID(4);
	ImGui::SliderInt("", &config.visuals.flashReduction, 0, 100, U8("闪光去除程度: %d%%"));
	ImGui::PopID();
	ImGui::PushID(5);
	ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 1.0f, U8("游戏亮度: %.2f"));
	ImGui::PopID();
	ImGui::PopItemWidth();
	ImGui::Combo(U8("天空模板"), &config.visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
	ImGuiCustom::colorPicker(U8("这世界啥色的啊"), config.visuals.world);
	ImGuiCustom::colorPicker(U8("这天咋变色了呢"), config.visuals.sky);
	ImGui::Checkbox(U8("沙鹰转转转"), &config.visuals.deagleSpinner);
	ImGui::Combo(U8("屏幕效果"), &config.visuals.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
	ImGui::Combo(U8("击中效果"), &config.visuals.hitEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
	ImGui::SliderFloat(U8("击中效果时间"), &config.visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
	ImGui::Combo(U8("命中标记"), &config.visuals.hitMarker, "None\0Default (Cross)\0");
	ImGui::SliderFloat(U8("命中标记时间"), &config.visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
	ImGui::Checkbox(U8("颜色校正"), &config.visuals.colorCorrection.enabled);
	ImGui::SameLine();
	bool ccPopup = ImGui::Button("Edit");

	if (ccPopup)
		ImGui::OpenPopup("##popup");

	if (ImGui::BeginPopup("##popup"))
	{
		ImGui::VSliderFloat("##1", {40.0f, 160.0f}, &config.visuals.colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f");
		ImGui::SameLine();
		ImGui::VSliderFloat("##2", {40.0f, 160.0f}, &config.visuals.colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f");
		ImGui::SameLine();
		ImGui::VSliderFloat("##3", {40.0f, 160.0f}, &config.visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f");
		ImGui::SameLine();
		ImGui::VSliderFloat("##4", {40.0f, 160.0f}, &config.visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f");
		ImGui::SameLine();
		ImGui::VSliderFloat("##5", {40.0f, 160.0f}, &config.visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f");
		ImGui::SameLine();
		ImGui::VSliderFloat("##6", {40.0f, 160.0f}, &config.visuals.colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f");
		ImGui::SameLine();
		ImGui::VSliderFloat("##7", {40.0f, 160.0f}, &config.visuals.colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f");
		ImGui::SameLine();
		ImGui::EndPopup();
	}
	ImGui::Columns(1);

	if (!contentOnly)
		ImGui::End();
}

void GUI::renderSkinChangerWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.skinChanger)
			return;
		ImGui::SetNextWindowSize({700.0f, 0.0f});
		ImGui::Begin(U8("更换皮肤"), &window.skinChanger, windowFlags);
	}

	static auto itemIndex = 0;

	ImGui::PushItemWidth(110.0f);
	ImGui::Combo("##1", &itemIndex, [](void *data, int idx, const char **out_text) {
		*out_text = game_data::weapon_names[idx].name;
		return true;
	},
				 nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
	ImGui::PopItemWidth();

	auto &selected_entry = config.skinChanger[itemIndex];
	selected_entry.definition_vector_index = itemIndex;

	{
		ImGui::SameLine();
		ImGui::Checkbox(U8("开启"), &selected_entry.enabled);
		ImGui::Separator();
		ImGui::Columns(2, nullptr, false);
		ImGui::InputInt("Seed", &selected_entry.seed);
		ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
		ImGui::SliderFloat("wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

		ImGui::Combo(U8("皮肤"), &selected_entry.paint_kit_vector_index, [](void *data, int idx, const char **out_text) {
			*out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
			return true;
		},
					 nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

		ImGui::Combo(U8("磨损"), &selected_entry.entity_quality_vector_index, [](void *data, int idx, const char **out_text) {
			*out_text = game_data::quality_names[idx].name;
			return true;
		},
					 nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

		if (itemIndex == 0)
		{
			ImGui::Combo(U8("刀"), &selected_entry.definition_override_vector_index, [](void *data, int idx, const char **out_text) {
				*out_text = game_data::knife_names[idx].name;
				return true;
			},
						 nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
		}
		else if (itemIndex == 1)
		{
			ImGui::Combo(U8("手套"), &selected_entry.definition_override_vector_index, [](void *data, int idx, const char **out_text) {
				*out_text = game_data::glove_names[idx].name;
				return true;
			},
						 nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
		}
		else
		{
			static auto unused_value = 0;
			selected_entry.definition_override_vector_index = 0;
			ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
		}

		ImGui::InputText(U8("改名"), selected_entry.custom_name, 32);
	}

	ImGui::NextColumn();

	{
		ImGui::PushID("sticker");

		static auto selectedStickerSlot = 0;

		ImGui::PushItemWidth(-1);

		ImGui::ListBox("", &selectedStickerSlot, [](void *data, int idx, const char **out_text) {
			static char elementName[64];
			auto kit_vector_index = config.skinChanger[itemIndex].stickers[idx].kit_vector_index;
			sprintf_s(elementName, "#%d (%s)", idx + 1, SkinChanger::stickerKits[kit_vector_index].name.c_str());
			*out_text = elementName;
			return true;
		},
					   nullptr, 5, 5);

		ImGui::PopItemWidth();

		auto &selected_sticker = selected_entry.stickers[selectedStickerSlot];

		ImGui::Combo(U8("贴纸"), &selected_sticker.kit_vector_index, [](void *data, int idx, const char **out_text) {
			*out_text = SkinChanger::stickerKits[idx].name.c_str();
			return true;
		},
					 nullptr, SkinChanger::stickerKits.size(), 10);

		ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
		ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.0f);
		ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.0f, 360.0f);

		ImGui::PopID();
	}
	selected_entry.update();

	ImGui::Columns(1);

	ImGui::Separator();

	if (ImGui::Button("Update", {130.0f, 30.0f}))
		SkinChanger::scheduleHudUpdate();

	ImGui::TextUnformatted("nSkinz by namazso");

	if (!contentOnly)
		ImGui::End();
}

void GUI::renderSoundWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.sound)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("听觉效果"), &window.sound, windowFlags);
	}
	ImGui::SliderInt(U8("鸡叫"), &config.sound.chickenVolume, 0, 200, "%d%%");

	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::Combo("", &currentCategory, U8("自己\0队友\0敌人\0"));
	ImGui::PopItemWidth();
	ImGui::SliderInt(U8("主音量"), &config.sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
	ImGui::SliderInt(U8("爆头声效音量"), &config.sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
	ImGui::SliderInt(U8("枪声"), &config.sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
	ImGui::SliderInt(U8("脚步声"), &config.sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");

	if (!contentOnly)
		ImGui::End();
}

void GUI::renderStyleWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.style)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("UI风格"), &window.style, windowFlags);
	}

	ImGui::PushItemWidth(150.0f);
	if (ImGui::Combo(U8("菜单风格"), &config.style.menuStyle, "Classic\0One window\0"))
		window = {};
	if (ImGui::Combo(U8("菜单颜色"), &config.style.menuColors, "Dark\0Light\0Classic\0Adobe\0Atropurpureus\0Custom\0"))
		updateColors();
	ImGui::PopItemWidth();

	if (config.style.menuColors == 5)
	{
		ImGuiStyle &style = ImGui::GetStyle();
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			if (i && i & 3)
				ImGui::SameLine(220.0f * (i & 3));

			ImGuiCustom::colorPicker(ImGui::GetStyleColorName(i), (float *)&style.Colors[i]);
		}
	}

	if (!contentOnly)
		ImGui::End();
}

void GUI::renderMiscWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.misc)
			return;
		ImGui::SetNextWindowSize({580.0f, 0.0f});
		ImGui::Begin(U8("杂项"), &window.misc, windowFlags);
	}
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 230.0f);
	ImGui::TextUnformatted(U8("菜单呼出热键"));
	ImGui::SameLine();
	hotkey(config.misc.menuKey);

	ImGui::Checkbox(U8("挂机防踢"), &config.misc.antiAfkKick);
	ImGui::Checkbox(U8("自动扫射"), &config.misc.autoStrafe);
	ImGui::Checkbox("Bhop", &config.misc.bunnyHop);
	ImGui::Checkbox(U8("快速行走"), &config.misc.fastDuck);
	ImGui::Checkbox(U8("太空步"), &config.misc.moonwalk);
	ImGui::Checkbox(U8("Edge跳"), &config.misc.edgejump);
	ImGui::SameLine();
	hotkey(config.misc.edgejumpkey);
	ImGui::Checkbox(U8("静步"), &config.misc.slowwalk);
	ImGui::SameLine();
	hotkey(config.misc.slowwalkKey);
	ImGui::Checkbox(U8("盲狙十字准星"), &config.misc.sniperCrosshair);
	ImGui::Checkbox(U8("反冲十字准星"), &config.misc.recoilCrosshair);
	ImGui::Checkbox(U8("手枪连射") , &config.misc.autoPistol);
	ImGui::Checkbox(U8("自动换单"), &config.misc.autoReload);
	ImGui::Checkbox(U8("AC自动鸡（自动同意游戏请求）"), &config.misc.autoAccept);
	ImGui::Checkbox(U8("雷达透视"), &config.misc.radarHack);
	ImGui::Checkbox(U8("显示段位"), &config.misc.revealRanks);
	ImGui::Checkbox(U8("显示金钱"), &config.misc.revealMoney);
	ImGui::Checkbox(U8("显示嫌疑人"), &config.misc.revealSuspect); //这什么几把
	ImGuiCustom::colorPicker(U8("观战者名单"), config.misc.spectatorList);
	ImGuiCustom::colorPicker(U8("水印"), config.misc.watermark);
	ImGui::Checkbox(U8("修复动画详细等级"), &config.misc.fixAnimationLOD);
	ImGui::Checkbox(U8("修复骨骼矩阵"), &config.misc.fixBoneMatrix);
	ImGui::Checkbox(U8("固定移动"), &config.misc.fixMovement);
	ImGui::Checkbox(U8("禁用模型遮挡"), &config.misc.disableModelOcclusion);
	ImGui::SliderFloat(U8("长宽比"), &config.misc.aspectratio, 0.0f, 5.0f, "%.2f");
	ImGui::NextColumn();
	ImGui::Checkbox(U8("隐藏HUD"), &config.misc.disablePanoramablur);
	ImGui::Checkbox(U8("动画氏族标记"), &config.misc.animatedClanTag);
	ImGui::Checkbox(U8("显示时间"), &config.misc.clocktag);
	ImGui::Checkbox(U8("自定义steam小组标签"), &config.misc.customClanTag);
	ImGui::SameLine();
	ImGui::PushItemWidth(120.0f);
	ImGui::PushID(0);
	if (ImGui::InputText("", &config.misc.clanTag))
		Misc::updateClanTag(true);
	ImGui::PopID();
	ImGui::Checkbox(U8("击杀消息"), &config.misc.killMessage);
	ImGui::SameLine();
	ImGui::PushItemWidth(120.0f);
	ImGui::PushID(1);
	ImGui::InputText("", &config.misc.killMessageString);
	ImGui::PopID();
	ImGui::Checkbox(U8("嫖别人名字"), &config.misc.nameStealer);
	ImGui::PushID(2);
	ImGui::InputText("", &config.misc.voteText);
	ImGui::PopID();
	ImGui::SameLine();
	if (ImGui::Button(U8("设置假投票")))
		Misc::fakeVote(true);

	ImGui::PushID(3);
	ImGui::SetNextItemWidth(100.0f);
	ImGui::Combo("", &config.misc.banColor, "White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0");
	ImGui::PopID();
	ImGui::SameLine();
	ImGui::PushID(4);
	ImGui::InputText("", &config.misc.banText);
	ImGui::PopID();
	ImGui::SameLine();
	if (ImGui::Button(U8("设置假封禁")))
		Misc::fakeBan(true);
	ImGui::Checkbox(U8("快速下包冲冲冲"), &config.misc.fastPlant);
	ImGuiCustom::colorPicker(U8("C4倒计时"), config.misc.bombTimer);
	ImGui::Checkbox(U8("快速换弹冲冲冲"), &config.misc.quickReload);
	ImGui::Checkbox(U8("自动捏左轮"), &config.misc.prepareRevolver);
	ImGui::SameLine();
	hotkey(config.misc.prepareRevolverKey);
	ImGui::Combo(U8("命中音效"), &config.misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0");
	ImGui::SetNextItemWidth(90.0f);
	ImGui::InputInt(U8("阻塞的数据包"), &config.misc.chokedPackets, 1, 5);
	config.misc.chokedPackets = std::clamp(config.misc.chokedPackets, 0, 64);
	ImGui::SameLine();
	hotkey(config.misc.chokedPacketsKey);
	ImGui::Text("Quick healthshot");
	ImGui::SameLine();
	hotkey(config.misc.quickHealthshotKey);
	ImGui::Checkbox(U8("手榴弹预测"), &config.misc.nadePredict);
	ImGui::Checkbox(U8("固定平板信号"), &config.misc.fixTabletSignal);
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat(U8("最大角度增量"), &config.misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
	ImGui::Checkbox(U8("假优先"), &config.misc.fakePrime);

	if (ImGui::Button(U8("脱钩")))
		hooks.restore();

	ImGui::Columns(1);
	if (!contentOnly)
		ImGui::End();
}

void GUI::renderReportbotWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.reportbot)
			return;
		ImGui::SetNextWindowSize({0.0f, 0.0f});
		ImGui::Begin(U8("举报机器人"), &window.reportbot, windowFlags);
	}
	ImGui::Checkbox(U8("启动"), &config.reportbot.enabled);
	ImGui::SameLine(0.0f, 50.0f);
	if (ImGui::Button(U8("重载")))
		Reportbot::reset();
	ImGui::Separator();
	ImGui::Combo(U8("目标"), &config.reportbot.target, "Enemies\0Allies\0All\0");
	ImGui::InputInt(U8("延迟（秒）"), &config.reportbot.delay, 1);
	config.reportbot.delay = (std::max)(config.reportbot.delay, 1);
	ImGui::Checkbox(U8("嘴臭"), &config.reportbot.textAbuse);
	ImGui::Checkbox("Griefing", &config.reportbot.griefing);
	ImGui::Checkbox(U8("透视"), &config.reportbot.wallhack);
	ImGui::Checkbox(U8("自瞄"), &config.reportbot.aimbot);
	ImGui::Checkbox(U8("其他作弊行为"), &config.reportbot.other);

	if (!contentOnly)
		ImGui::End();
}

void GUI::renderConfigWindow(bool contentOnly) noexcept
{
	if (!contentOnly)
	{
		if (!window.config)
			return;
		ImGui::SetNextWindowSize({290.0f, 190.0f});
		ImGui::Begin(U8("设置"), &window.config, windowFlags);
	}

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 170.0f);

	ImGui::PushItemWidth(160.0f);

	constexpr auto &configItems = config.getConfigs();
	static int currentConfig = -1;

	if (static_cast<size_t>(currentConfig) >= configItems.size())
		currentConfig = -1;

	static std::string buffer;

	if (ImGui::ListBox("", &currentConfig, [](void *data, int idx, const char **out_text) {
			auto &vector = *static_cast<std::vector<std::string> *>(data);
			*out_text = vector[idx].c_str();
			return true;
		},
					   &configItems, configItems.size(), 5) &&
		currentConfig != -1)
		buffer = configItems[currentConfig];

	ImGui::PushID(0);
	if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (currentConfig != -1)
			config.rename(currentConfig, buffer.c_str());
	}
	ImGui::PopID();
	ImGui::NextColumn();

	ImGui::PushItemWidth(100.0f);

	if (ImGui::Button(U8("新建配置文件"), {100.0f, 25.0f}))
		config.add(buffer.c_str());

	if (ImGui::Button(U8("重置配置文件"), {100.0f, 25.0f}))
		ImGui::OpenPopup("Config to reset");

	if (ImGui::BeginPopup("Config to reset"))
	{
		static constexpr const char *names[]{"Whole", "Aimbot", "Triggerbot", "Backtrack", "Anti aim", "Glow", "Chams", "Esp", "Visuals", "Skin changer", "Sound", "Style", "Misc", "Reportbot"};
		for (int i = 0; i < IM_ARRAYSIZE(names); i++)
		{
			if (i == 1)
				ImGui::Separator();

			if (ImGui::Selectable(names[i]))
			{
				switch (i)
				{
				case 0:
					config.reset();
					updateColors();
					Misc::updateClanTag(true);
					SkinChanger::scheduleHudUpdate();
					break;
				case 1:
					config.aimbot = {};
					break;
				case 2:
					config.triggerbot = {};
					break;
				case 3:
					config.backtrack = {};
					break;
				case 4:
					config.antiAim = {};
					break;
				case 5:
					config.glow = {};
					break;
				case 6:
					config.chams = {};
					break;
				case 7:
					config.esp = {};
					break;
				case 8:
					config.visuals = {};
					break;
				case 9:
					config.skinChanger = {};
					SkinChanger::scheduleHudUpdate();
					break;
				case 10:
					config.sound = {};
					break;
				case 11:
					config.style = {};
					updateColors();
					break;
				case 12:
					config.misc = {};
					Misc::updateClanTag(true);
					break;
				case 13:
					config.reportbot = {};
					break;
				}
			}
		}
		ImGui::EndPopup();
	}
	if (currentConfig != -1)
	{
		if (ImGui::Button(U8("加载选中配置文件"), {100.0f, 25.0f}))
		{
			config.load(currentConfig);
			updateColors();
			SkinChanger::scheduleHudUpdate();
			Misc::updateClanTag(true);
		}
		if (ImGui::Button(U8("保存"), {100.0f, 25.0f}))
			config.save(currentConfig);
		if (ImGui::Button(U8("删除"), {100.0f, 25.0f}))
			config.remove(currentConfig);
	}
	ImGui::Columns(1);
	if (!contentOnly)
		ImGui::End();
}

void GUI::renderGuiStyle2() noexcept
{
	ImGui::SetNextWindowSize({600.0f, 0.0f});
	ImGui::Begin("Osiris", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
	{
		if (ImGui::BeginTabItem(U8("自瞄")))
		{
			renderAimbotWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("陀螺")))
		{
			renderAntiAimWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("自动开枪")))
		{
			renderTriggerbotWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("回溯")))
		{
			renderBacktrackWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("实体发光")))
		{
			renderGlowWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("人物发光")))
		{
			renderChamsWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("透视与玩家信息（非发光）")))
		{
			renderEspWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("视觉")))
		{
			renderVisualsWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("皮肤修改")))
		{
			renderSkinChangerWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("听觉效果")))
		{
			renderSoundWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("UI风格")))
		{
			renderStyleWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("杂项")))
		{
			renderMiscWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("举报机器人")))
		{
			renderReportbotWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("设置")))
		{
			renderConfigWindow(true);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}
