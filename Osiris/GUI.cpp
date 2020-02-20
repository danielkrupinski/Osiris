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
		ImGui::MenuItem(U8("����"), nullptr, &window.aimbot);
		ImGui::MenuItem(U8("����"), nullptr, &window.antiAim);
		ImGui::MenuItem(U8("�Զ���ǹ"), nullptr, &window.triggerbot);
		ImGui::MenuItem(U8("����"), nullptr, &window.backtrack);
		ImGui::MenuItem(U8("ʵ�巢��"), nullptr, &window.glow);
		ImGui::MenuItem(U8("���﷢��"), nullptr, &window.chams);
		ImGui::MenuItem(U8("͸���������Ϣ(�Ƿ���)"), nullptr, &window.esp);
		ImGui::MenuItem(U8("�Ӿ�"), nullptr, &window.visuals);
		ImGui::MenuItem(U8("Ƥ���޸���"), nullptr, &window.skinChanger);
		ImGui::MenuItem(U8("��Ч�޸�"), nullptr, &window.sound);
		ImGui::MenuItem(U8("UI���"), nullptr, &window.style);
		ImGui::MenuItem(U8("����"), nullptr, &window.misc);
		ImGui::MenuItem(U8("�ٱ�������"), nullptr, &window.reportbot);
		ImGui::MenuItem(U8("����"), nullptr, &window.config);
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
		ImGui::Begin(U8("����"), &window.aimbot, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("ȫ��\0��ǹ\0��������\0���ǹ\0��ǹ\0"));
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
			U8("ȫ��"), U8("�����18"), U8("P2000"),
			U8("USP-S"), U8("˫�ֱ�����"), U8("P250"),
			U8("Tec-9"), U8("FN57"), U8("CZ-75�Զ���ǹ"),
			U8("ɳĮ֮ӥ"), U8("������ǹ")};

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
			U8("����"), U8("����"), U8("XM1014"),
			U8("�ض�����ǹ"), U8("MAG-7"), U8("M249"),
			U8("�ڸ��")};

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
			U8("ȫ��"), U8("Mac-10"), U8("MP9"),
			U8("MP7"), U8("MP5-SD"), U8("UMP-45"),
			U8("P90"), U8("PPҰţ")};

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
			U8("ȫ��"), U8("������ AR"), U8("����˹"),
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
	ImGui::Checkbox(U8("����"), &config.aimbot[currentWeapon].enabled);
	ImGui::Separator();
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 220.0f);
	ImGui::Checkbox(U8("����"), &config.aimbot[currentWeapon].onKey);
	ImGui::SameLine();
	hotkey(config.aimbot[currentWeapon].key);
	ImGui::SameLine();
	ImGui::PushID(2);
	ImGui::PushItemWidth(70.0f);
	ImGui::Combo("", &config.aimbot[currentWeapon].keyMode, "Hold\0Toggle\0");
	ImGui::PopItemWidth();
	ImGui::PopID();
	ImGui::Checkbox(U8("��������"), &config.aimbot[currentWeapon].aimlock);
	ImGui::Checkbox(U8("��ȫģʽ"), &config.aimbot[currentWeapon].silent);
	ImGui::Checkbox(U8("���Ӷ���"), &config.aimbot[currentWeapon].friendlyFire);
	ImGui::Checkbox(U8("������׼"), &config.aimbot[currentWeapon].visibleOnly);
	ImGui::Checkbox(U8("ֻ�ڿ���ʱ����"), &config.aimbot[currentWeapon].scopedOnly);
	ImGui::Checkbox(U8("��������"), &config.aimbot[currentWeapon].ignoreFlash);
	ImGui::Checkbox(U8("��������"), &config.aimbot[currentWeapon].ignoreSmoke);
	ImGui::Checkbox(U8("�Զ���ǹ"), &config.aimbot[currentWeapon].autoShot);
	ImGui::Checkbox(U8("�Զ�����"), &config.aimbot[currentWeapon].autoScope);
	ImGui::Checkbox(U8("������"), &config.aimbot[currentWeapon].recoilbasedFov);
	ImGui::Combo(U8("��׼λ��"), &config.aimbot[currentWeapon].bone,
				 U8("����׼�����\0����˺�\0ͷ\0����\0�ع�\0��\0����\0����(JB)\0"));
	ImGui::NextColumn();
	ImGui::PushItemWidth(240.0f);
	ImGui::SliderFloat(U8("�ӽ�"), &config.aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", 2.5f);
	ImGui::SliderFloat(U8("ƽ����"), &config.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
	ImGui::SliderFloat(U8("���X������"), &config.aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat(U8("���Y������"), &config.aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat(U8("�����׼���"), &config.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
	ImGui::SliderFloat(U8("���ǹ���"), &config.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
	ImGui::InputInt(U8("��С�˺�"), &config.aimbot[currentWeapon].minDamage);
	config.aimbot[currentWeapon].minDamage = std::clamp(config.aimbot[currentWeapon].minDamage, 0, 250);
	ImGui::Checkbox(U8("˲���ɱ"), &config.aimbot[currentWeapon].killshot);
	ImGui::Checkbox(U8("��ǽ��ǹ"), &config.aimbot[currentWeapon].betweenShots);
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
		ImGui::Begin(U8("����"), &window.antiAim, windowFlags);
	}
	ImGui::Checkbox(U8("����"), &config.antiAim.enabled);
	ImGui::Checkbox("##pitch", &config.antiAim.pitch);
	ImGui::SameLine();
	ImGui::SliderFloat(U8("��б�� (Pitch)"), &config.antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
	ImGui::Checkbox(U8("ƫ�ƶ� (Yaw)"), &config.antiAim.yaw);
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
		ImGui::Begin(U8("�Զ���ǹ"), &window.triggerbot, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("ȫ��\0��ǹ\0��������\0���ǹ\0��ǹ\0���ǹ\0"));
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
			U8("ȫ��"), U8("�����18"), U8("P2000"),
			U8("USP-S"), U8("˫�ֱ�����"), U8("P250"),
			U8("Tec-9"), U8("FN57"), U8("CZ-75�Զ���ǹ"),
			U8("ɳĮ֮ӥ"), U8("������ǹ")};

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
			U8("����"), U8("����"), U8("XM1014"),
			U8("�ض�����ǹ"), U8("MAG-7"), U8("M249"),
			U8("�ڸ��")};

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
			U8("ȫ��"), U8("Mac-10"), U8("MP9"),
			U8("MP7"), U8("MP5-SD"), U8("UMP-45"),
			U8("P90"), U8("PPҰţ")};

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
			U8("ȫ��"), U8("������ AR"), U8("����˹"),
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
	ImGui::Checkbox(U8("����"), &config.triggerbot[currentWeapon].enabled);
	ImGui::Separator();
	ImGui::Checkbox(U8("����"), &config.triggerbot[currentWeapon].onKey);
	ImGui::SameLine();
	hotkey(config.triggerbot[currentWeapon].key);
	ImGui::Checkbox(U8("���Ӷ���"), &config.triggerbot[currentWeapon].friendlyFire);
	ImGui::Checkbox(U8("ֻ�ڿ���ʱ����"), &config.triggerbot[currentWeapon].scopedOnly);
	ImGui::Checkbox(U8("��������"), &config.triggerbot[currentWeapon].ignoreFlash);
	ImGui::Checkbox(U8("��������"), &config.triggerbot[currentWeapon].ignoreSmoke);
	ImGui::SetNextItemWidth(85.0f);
	ImGui::Combo(U8("�жϲ�λ"), &config.triggerbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
	ImGui::PushItemWidth(220.0f);
	ImGui::SliderInt(U8("�ӳٿ�ǹ"), &config.triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
	ImGui::InputInt(U8("��С�˺�"), &config.triggerbot[currentWeapon].minDamage);
	config.triggerbot[currentWeapon].minDamage = std::clamp(config.triggerbot[currentWeapon].minDamage, 0, 250);
	ImGui::Checkbox(U8("˲���ɱ"), &config.triggerbot[currentWeapon].killshot);

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
		ImGui::Begin(U8("����"), &window.backtrack, windowFlags);
	}
	ImGui::Checkbox(U8("����"), &config.backtrack.enabled);
	ImGui::Checkbox(U8("��������"), &config.backtrack.ignoreSmoke);
	ImGui::Checkbox(U8("�������ӽ�"), &config.backtrack.recoilBasedFov);
	ImGui::PushItemWidth(220.0f);
	ImGui::SliderInt(U8("�ӳ�"), &config.backtrack.timeLimit, 1, 200, "%d ms");
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
		ImGui::Begin(U8("ʵ�巢��"), &window.glow, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("����\0����\0�����°���ʵ��\0���ڲ����ʵ��\0�������\0��������\0C4\0��װ�˵�C4\0��\0�������\0Ͷ����\0����\0ʬ��\0"));
	ImGui::PopID();
	static int currentItem{0};
	if (currentCategory <= 3)
	{
		ImGui::SameLine();
		static int currentType{0};
		ImGui::PushID(1);
		ImGui::Combo("", &currentType, U8("ȫ��\0���ӵ�\0ǽ���\0"));
		ImGui::PopID();
		currentItem = currentCategory * 3 + currentType;
	}
	else
	{
		currentItem = currentCategory + 8;
	}

	ImGui::SameLine();
	ImGui::Checkbox(U8("����"), &config.glow[currentItem].enabled);
	ImGui::Separator();
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 150.0f);
	ImGui::Checkbox(U8("Ѫ��"), &config.glow[currentItem].healthBased);

	ImGuiCustom::colorPicker(U8("��ɫ"), config.glow[currentItem].color.color, nullptr, &config.glow[currentItem].color.rainbow, &config.glow[currentItem].color.rainbowSpeed);

	ImGui::NextColumn();
	ImGui::PushItemWidth(220.0f);
	ImGui::SliderFloat(U8("��ϸ"), &config.glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat(U8("͸����"), &config.glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
	ImGui::SliderInt(U8("��ʽ"), &config.glow[currentItem].style, 0, 3);
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
		ImGui::Begin(U8("���﷢��"), &window.chams, windowFlags);
	}
	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, U8("����\0����\0�����°�����\0���ڲ������\0�������\0����\0��\0����\0����\0"));
	ImGui::PopID();
	static int currentItem{0};

	if (currentCategory <= 3)
	{
		ImGui::SameLine();
		static int currentType{0};
		ImGui::PushID(1);
		ImGui::Combo("", &currentType, U8("ȫ��\0���ӵ�\0ǽ���\0"));
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

	ImGui::Checkbox(U8("����"), &chams.enabled);
	ImGui::Separator();
	ImGui::Checkbox(U8("Ѫ��"), &chams.healthBased);
	ImGui::Checkbox(U8("������˸"), &chams.blinking);
	ImGui::Combo(U8("ģ�Ͳ���"), &chams.material, U8("��ͨ\0��ƽ\0���\0�׽�ɫ\0����\0����ɫ\0͸��ɫ\0����ɫ\0�ƽ�ɫ\0����\0"));
	ImGui::Checkbox(U8("��������Ⱦ"), &chams.wireframe);
	ImGuiCustom::colorPicker(U8("��ɫ"), chams.color.color, nullptr, &chams.color.rainbow, &chams.color.rainbowSpeed);
	ImGui::SetNextItemWidth(220.0f);
	ImGui::SliderFloat(U8("͸����"), &chams.alpha, 0.0f, 1.0f, "%.2f");

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
		ImGui::Begin(U8("͸���������Ϣ���Ƿ��⣩"), &window.esp, windowFlags);
	}

	static int currentCategory = 0;
	static int currentItem = 0;

	if (ImGui::ListBoxHeader("##", {125.0f, 300.0f}))
	{
		static constexpr const char *players[]{"All", "Visible", "Occluded"};
		ImGui::Text(U8("ʹ���ҵĶ���"));
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
		ImGui::Text(U8("�����ҵĵ���"));
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
		if (bool isSelected = currentCategory == 2; ImGui::Selectable(U8("����"), isSelected))  
		currentCategory = 2;
		ImGui::Text(U8("������"));
		ImGui::Indent();
		ImGui::PushID("Projectiles");
		static  const char *projectiles[]{U8("���ⵯ"), U8("�߱�����"), U8("ң��ը��"),U8("��������"), U8("�ն���"), U8("�˵Ļ�"), U8("ս���������"), U8("����"), U8("ѩ��")};
		
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

		ImGui::Text(U8("�Լ�"));
		ImGui::Indent();
		ImGui::PushID("Danger Zone");
		static  const char *dangerZone[]{U8("�ڱ�"), U8("���˻�"), U8("�ֽ�"), U8("һ���ֽ�"), U8("��ǹ����"), U8("����"), U8("��������"), U8("ըҩ��"), U8("������"), U8("ȫ��"), U8("���"), U8("����ͷ��"), U8("����ɡ"), U8("���İ�"), U8("ƽ��"), U8("������"), U8("��ҩ��"), U8("�״����")};

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
			ImGui::Checkbox(U8("����"), &config.esp.players[selected].enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("front", &config.esp.players[selected].font, 1, 294);
			config.esp.players[selected].font = std::clamp(config.esp.players[selected].font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("����"), config.esp.players[selected].snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("���﷽��"), config.esp.players[selected].box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.players[selected].boxType, "2D\0"
																	"2D corners\0"
																	"3D\0"
																	"3D corners\0");
			ImGuiCustom::colorPicker(U8("�۾�"), config.esp.players[selected].eyeTraces);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("Ѫ��"), config.esp.players[selected].health);
			ImGuiCustom::colorPicker(U8("�Թϴ�"), config.esp.players[selected].headDot);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("Ѫ��bar"), config.esp.players[selected].healthBar);
			ImGuiCustom::colorPicker(U8("����"), config.esp.players[selected].name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.players[selected].armor);
			ImGuiCustom::colorPicker(U8("Ǯ"), config.esp.players[selected].money);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����bar"), config.esp.players[selected].armorBar);
			ImGuiCustom::colorPicker(U8("����"), config.esp.players[selected].outline);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.players[selected].distance);
			ImGuiCustom::colorPicker(U8("����"),config.esp.players[selected].activeWeapon);
			ImGui::SameLine(spacing);
			ImGui::Checkbox(U8("͸����"), &config.esp.players[selected].deadesp);
			ImGui::SliderFloat(U8("������"), &config.esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
			break;
		}
		case 2:
		{
			ImGui::Checkbox(U8("����"), &config.esp.weapon.enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("Font", &config.esp.weapon.font, 1, 294);
			config.esp.weapon.font = std::clamp(config.esp.weapon.font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("����"), config.esp.weapon.snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.weapon.box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.weapon.boxType, "2D\0"
														 "2D corners\0"
														 "3D\0"
														 "3D corners\0");
			ImGuiCustom::colorPicker(U8("����"), config.esp.weapon.name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.weapon.outline);
			ImGuiCustom::colorPicker(U8("����"), config.esp.weapon.distance);
			ImGui::SliderFloat(U8("������"), &config.esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
			break;
		}
		case 3:
		{
			ImGui::Checkbox(U8("����"), &config.esp.projectiles[currentItem].enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("Font", &config.esp.projectiles[currentItem].font, 1, 294);
			config.esp.projectiles[currentItem].font = std::clamp(config.esp.projectiles[currentItem].font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("����"), config.esp.projectiles[currentItem].snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.projectiles[currentItem].box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.projectiles[currentItem].boxType, "2D\0"
																		   "2D corners\0"
																		   "3D\0"
																		   "3D corners\0");
			ImGuiCustom::colorPicker(U8("����"), config.esp.projectiles[currentItem].name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.projectiles[currentItem].outline);
			ImGuiCustom::colorPicker(U8("����"), config.esp.projectiles[currentItem].distance);
			ImGui::SliderFloat(U8("������"), &config.esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
			break;
		}
		case 4:
		{
			int selected = currentItem;
			ImGui::Checkbox(U8("����"), &config.esp.dangerZone[selected].enabled);
			ImGui::SameLine(0.0f, 50.0f);
			ImGui::SetNextItemWidth(85.0f);
			ImGui::InputInt("Font", &config.esp.dangerZone[selected].font, 1, 294);
			config.esp.dangerZone[selected].font = std::clamp(config.esp.dangerZone[selected].font, 1, 294);

			ImGui::Separator();

			constexpr auto spacing{200.0f};
			ImGuiCustom::colorPicker(U8("����"), config.esp.dangerZone[selected].snaplines);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.dangerZone[selected].box);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo("", &config.esp.dangerZone[selected].boxType, "2D\0"
																	   "2D corners\0"
																	   "3D\0"
																	   "3D corners\0");
			ImGuiCustom::colorPicker(U8("����"), config.esp.dangerZone[selected].name);
			ImGui::SameLine(spacing);
			ImGuiCustom::colorPicker(U8("����"), config.esp.dangerZone[selected].outline);
			ImGuiCustom::colorPicker(U8("����"), config.esp.dangerZone[selected].distance);
			ImGui::SliderFloat(U8("������"), &config.esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
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
		ImGui::Begin(U8("���ӻ�"), &window.visuals, windowFlags);
	}
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 280.0f);
	ImGui::Combo(U8("�ˤ�����ģ��"), &config.visuals.playerModelT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
	ImGui::Combo(U8("��������ģ��"), &config.visuals.playerModelCT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
	ImGui::Checkbox(U8("���ú���"), &config.visuals.disablePostProcessing);
	ImGui::Checkbox(U8("�������ү�ɣ�ʬ���������죩"), &config.visuals.inverseRagdollGravity);
	ImGui::Checkbox(U8("û����"), &config.visuals.noFog);
	ImGui::Checkbox(U8("�ر�3D���"), &config.visuals.no3dSky);
	ImGui::Checkbox(U8("�޺���"), &config.visuals.noAimPunch);
	ImGui::Checkbox(U8("������û�к�����Ȼ���ѣ�"), &config.visuals.noViewPunch);
	ImGui::Checkbox(U8("���֣������֣�"), &config.visuals.noHands);
	ImGui::Checkbox(U8("���·����������ӣ�"), &config.visuals.noSleeves);
	ImGui::Checkbox(U8("����ǹ������������"), &config.visuals.noWeapons);
	ImGui::Checkbox(U8("͸��"), &config.visuals.noSmoke);
	ImGui::Checkbox(U8("ɯ�հ�˼����ģ����"), &config.visuals.noBlur);
	ImGui::Checkbox(U8("ɯ�հ�˼��һ�ο����ڱ����������������"), &config.visuals.noScopeOverlay);
	ImGui::Checkbox(U8("ţ�Բ�(û�в�)"), &config.visuals.noGrass);
	ImGui::Checkbox(U8("û����Ӱ"), &config.visuals.noShadows);
	ImGui::Checkbox(U8("������ʾ����"), &config.visuals.wireframeSmoke);
	ImGui::NextColumn();
	ImGui::Checkbox(U8("���ж����Ұ���ж��"), &config.visuals.zoom);
	ImGui::SameLine();
	hotkey(config.visuals.zoomKey);
	ImGui::Checkbox(U8("�����˳�"), &config.visuals.thirdperson);
	ImGui::SameLine();
	hotkey(config.visuals.thirdpersonKey);
	ImGui::PushItemWidth(290.0f);
	ImGui::PushID(0);
	ImGui::SliderInt("", &config.visuals.thirdpersonDistance, 0, 1000, U8("�����˳ƾ���: %d"));
	ImGui::PopID();
	ImGui::PushID(1);
	ImGui::SliderInt("", &config.visuals.viewmodelFov, -60, 60, U8("�����ж೤�ֱ۾��ж೤: %d"));
	ImGui::PopID();
	ImGui::PushID(2);
	ImGui::SliderInt("", &config.visuals.fov, -60, 60, U8("��Ұ��С: %d"));
	ImGui::PopID();
	ImGui::PushID(3);
	ImGui::SliderInt("", &config.visuals.farZ, 0, 2000, U8("��Ⱦ����: %d"));
	ImGui::PopID();
	ImGui::PushID(4);
	ImGui::SliderInt("", &config.visuals.flashReduction, 0, 100, U8("����ȥ���̶�: %d%%"));
	ImGui::PopID();
	ImGui::PushID(5);
	ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 1.0f, U8("��Ϸ����: %.2f"));
	ImGui::PopID();
	ImGui::PopItemWidth();
	ImGui::Combo(U8("���ģ��"), &config.visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
	ImGuiCustom::colorPicker(U8("������ɶɫ�İ�"), config.visuals.world);
	ImGuiCustom::colorPicker(U8("����զ��ɫ����"), config.visuals.sky);
	ImGui::Checkbox(U8("ɳӥתתת"), &config.visuals.deagleSpinner);
	ImGui::Combo(U8("��ĻЧ��"), &config.visuals.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
	ImGui::Combo(U8("����Ч��"), &config.visuals.hitEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
	ImGui::SliderFloat(U8("����Ч��ʱ��"), &config.visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
	ImGui::Combo(U8("���б��"), &config.visuals.hitMarker, "None\0Default (Cross)\0");
	ImGui::SliderFloat(U8("���б��ʱ��"), &config.visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
	ImGui::Checkbox(U8("��ɫУ��"), &config.visuals.colorCorrection.enabled);
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
		ImGui::Begin(U8("����Ƥ��"), &window.skinChanger, windowFlags);
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
		ImGui::Checkbox(U8("����"), &selected_entry.enabled);
		ImGui::Separator();
		ImGui::Columns(2, nullptr, false);
		ImGui::InputInt("Seed", &selected_entry.seed);
		ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
		ImGui::SliderFloat("wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

		ImGui::Combo(U8("Ƥ��"), &selected_entry.paint_kit_vector_index, [](void *data, int idx, const char **out_text) {
			*out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
			return true;
		},
					 nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

		ImGui::Combo(U8("ĥ��"), &selected_entry.entity_quality_vector_index, [](void *data, int idx, const char **out_text) {
			*out_text = game_data::quality_names[idx].name;
			return true;
		},
					 nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

		if (itemIndex == 0)
		{
			ImGui::Combo(U8("��"), &selected_entry.definition_override_vector_index, [](void *data, int idx, const char **out_text) {
				*out_text = game_data::knife_names[idx].name;
				return true;
			},
						 nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
		}
		else if (itemIndex == 1)
		{
			ImGui::Combo(U8("����"), &selected_entry.definition_override_vector_index, [](void *data, int idx, const char **out_text) {
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

		ImGui::InputText(U8("����"), selected_entry.custom_name, 32);
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

		ImGui::Combo(U8("��ֽ"), &selected_sticker.kit_vector_index, [](void *data, int idx, const char **out_text) {
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
		ImGui::Begin(U8("����Ч��"), &window.sound, windowFlags);
	}
	ImGui::SliderInt(U8("����"), &config.sound.chickenVolume, 0, 200, "%d%%");

	static int currentCategory{0};
	ImGui::PushItemWidth(110.0f);
	ImGui::Combo("", &currentCategory, U8("�Լ�\0����\0����\0"));
	ImGui::PopItemWidth();
	ImGui::SliderInt(U8("������"), &config.sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
	ImGui::SliderInt(U8("��ͷ��Ч����"), &config.sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
	ImGui::SliderInt(U8("ǹ��"), &config.sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
	ImGui::SliderInt(U8("�Ų���"), &config.sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");

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
		ImGui::Begin(U8("UI���"), &window.style, windowFlags);
	}

	ImGui::PushItemWidth(150.0f);
	if (ImGui::Combo(U8("�˵����"), &config.style.menuStyle, "Classic\0One window\0"))
		window = {};
	if (ImGui::Combo(U8("�˵���ɫ"), &config.style.menuColors, "Dark\0Light\0Classic\0Adobe\0Atropurpureus\0Custom\0"))
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
		ImGui::Begin(U8("����"), &window.misc, windowFlags);
	}
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 230.0f);
	ImGui::TextUnformatted(U8("�˵������ȼ�"));
	ImGui::SameLine();
	hotkey(config.misc.menuKey);

	ImGui::Checkbox(U8("�һ�����"), &config.misc.antiAfkKick);
	ImGui::Checkbox(U8("�Զ�ɨ��"), &config.misc.autoStrafe);
	ImGui::Checkbox("Bhop", &config.misc.bunnyHop);
	ImGui::Checkbox(U8("��������"), &config.misc.fastDuck);
	ImGui::Checkbox(U8("̫�ղ�"), &config.misc.moonwalk);
	ImGui::Checkbox(U8("Edge��"), &config.misc.edgejump);
	ImGui::SameLine();
	hotkey(config.misc.edgejumpkey);
	ImGui::Checkbox(U8("����"), &config.misc.slowwalk);
	ImGui::SameLine();
	hotkey(config.misc.slowwalkKey);
	ImGui::Checkbox(U8("ä��ʮ��׼��"), &config.misc.sniperCrosshair);
	ImGui::Checkbox(U8("����ʮ��׼��"), &config.misc.recoilCrosshair);
	ImGui::Checkbox(U8("��ǹ����") , &config.misc.autoPistol);
	ImGui::Checkbox(U8("�Զ�����"), &config.misc.autoReload);
	ImGui::Checkbox(U8("AC�Զ������Զ�ͬ����Ϸ����"), &config.misc.autoAccept);
	ImGui::Checkbox(U8("�״�͸��"), &config.misc.radarHack);
	ImGui::Checkbox(U8("��ʾ��λ"), &config.misc.revealRanks);
	ImGui::Checkbox(U8("��ʾ��Ǯ"), &config.misc.revealMoney);
	ImGui::Checkbox(U8("��ʾ������"), &config.misc.revealSuspect); //��ʲô����
	ImGuiCustom::colorPicker(U8("��ս������"), config.misc.spectatorList);
	ImGuiCustom::colorPicker(U8("ˮӡ"), config.misc.watermark);
	ImGui::Checkbox(U8("�޸�������ϸ�ȼ�"), &config.misc.fixAnimationLOD);
	ImGui::Checkbox(U8("�޸���������"), &config.misc.fixBoneMatrix);
	ImGui::Checkbox(U8("�̶��ƶ�"), &config.misc.fixMovement);
	ImGui::Checkbox(U8("����ģ���ڵ�"), &config.misc.disableModelOcclusion);
	ImGui::SliderFloat(U8("�����"), &config.misc.aspectratio, 0.0f, 5.0f, "%.2f");
	ImGui::NextColumn();
	ImGui::Checkbox(U8("����HUD"), &config.misc.disablePanoramablur);
	ImGui::Checkbox(U8("����������"), &config.misc.animatedClanTag);
	ImGui::Checkbox(U8("��ʾʱ��"), &config.misc.clocktag);
	ImGui::Checkbox(U8("�Զ���steamС���ǩ"), &config.misc.customClanTag);
	ImGui::SameLine();
	ImGui::PushItemWidth(120.0f);
	ImGui::PushID(0);
	if (ImGui::InputText("", &config.misc.clanTag))
		Misc::updateClanTag(true);
	ImGui::PopID();
	ImGui::Checkbox(U8("��ɱ��Ϣ"), &config.misc.killMessage);
	ImGui::SameLine();
	ImGui::PushItemWidth(120.0f);
	ImGui::PushID(1);
	ImGui::InputText("", &config.misc.killMessageString);
	ImGui::PopID();
	ImGui::Checkbox(U8("�α�������"), &config.misc.nameStealer);
	ImGui::PushID(2);
	ImGui::InputText("", &config.misc.voteText);
	ImGui::PopID();
	ImGui::SameLine();
	if (ImGui::Button(U8("���ü�ͶƱ")))
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
	if (ImGui::Button(U8("���üٷ��")))
		Misc::fakeBan(true);
	ImGui::Checkbox(U8("�����°�����"), &config.misc.fastPlant);
	ImGuiCustom::colorPicker(U8("C4����ʱ"), config.misc.bombTimer);
	ImGui::Checkbox(U8("���ٻ�������"), &config.misc.quickReload);
	ImGui::Checkbox(U8("�Զ�������"), &config.misc.prepareRevolver);
	ImGui::SameLine();
	hotkey(config.misc.prepareRevolverKey);
	ImGui::Combo(U8("������Ч"), &config.misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0");
	ImGui::SetNextItemWidth(90.0f);
	ImGui::InputInt(U8("���������ݰ�"), &config.misc.chokedPackets, 1, 5);
	config.misc.chokedPackets = std::clamp(config.misc.chokedPackets, 0, 64);
	ImGui::SameLine();
	hotkey(config.misc.chokedPacketsKey);
	ImGui::Text("Quick healthshot");
	ImGui::SameLine();
	hotkey(config.misc.quickHealthshotKey);
	ImGui::Checkbox(U8("����Ԥ��"), &config.misc.nadePredict);
	ImGui::Checkbox(U8("�̶�ƽ���ź�"), &config.misc.fixTabletSignal);
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat(U8("���Ƕ�����"), &config.misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
	ImGui::Checkbox(U8("������"), &config.misc.fakePrime);

	if (ImGui::Button(U8("�ѹ�")))
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
		ImGui::Begin(U8("�ٱ�������"), &window.reportbot, windowFlags);
	}
	ImGui::Checkbox(U8("����"), &config.reportbot.enabled);
	ImGui::SameLine(0.0f, 50.0f);
	if (ImGui::Button(U8("����")))
		Reportbot::reset();
	ImGui::Separator();
	ImGui::Combo(U8("Ŀ��"), &config.reportbot.target, "Enemies\0Allies\0All\0");
	ImGui::InputInt(U8("�ӳ٣��룩"), &config.reportbot.delay, 1);
	config.reportbot.delay = (std::max)(config.reportbot.delay, 1);
	ImGui::Checkbox(U8("���"), &config.reportbot.textAbuse);
	ImGui::Checkbox("Griefing", &config.reportbot.griefing);
	ImGui::Checkbox(U8("͸��"), &config.reportbot.wallhack);
	ImGui::Checkbox(U8("����"), &config.reportbot.aimbot);
	ImGui::Checkbox(U8("����������Ϊ"), &config.reportbot.other);

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
		ImGui::Begin(U8("����"), &window.config, windowFlags);
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

	if (ImGui::Button(U8("�½������ļ�"), {100.0f, 25.0f}))
		config.add(buffer.c_str());

	if (ImGui::Button(U8("���������ļ�"), {100.0f, 25.0f}))
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
		if (ImGui::Button(U8("����ѡ�������ļ�"), {100.0f, 25.0f}))
		{
			config.load(currentConfig);
			updateColors();
			SkinChanger::scheduleHudUpdate();
			Misc::updateClanTag(true);
		}
		if (ImGui::Button(U8("����"), {100.0f, 25.0f}))
			config.save(currentConfig);
		if (ImGui::Button(U8("ɾ��"), {100.0f, 25.0f}))
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
		if (ImGui::BeginTabItem(U8("����")))
		{
			renderAimbotWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("����")))
		{
			renderAntiAimWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("�Զ���ǹ")))
		{
			renderTriggerbotWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("����")))
		{
			renderBacktrackWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("ʵ�巢��")))
		{
			renderGlowWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("���﷢��")))
		{
			renderChamsWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("͸���������Ϣ���Ƿ��⣩")))
		{
			renderEspWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("�Ӿ�")))
		{
			renderVisualsWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("Ƥ���޸�")))
		{
			renderSkinChangerWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("����Ч��")))
		{
			renderSoundWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("UI���")))
		{
			renderStyleWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("����")))
		{
			renderMiscWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("�ٱ�������")))
		{
			renderReportbotWindow(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(U8("����")))
		{
			renderConfigWindow(true);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}
