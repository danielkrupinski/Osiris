#include <fstream>
#include <ShlObj.h>

#include "cereal/archives/binary.hpp"

#include "Config.h"

Config::Config(const char* name) noexcept
{
    PWSTR pathToDocuments;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
        path = pathToDocuments;
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }

	std::ifstream file(path.string().c_str(), std::ifstream::binary);
	if (file.good()) {
		file.seekg(0, file.end);
		long size = file.tellg();
		file.seekg(0);
		char* buffer = new char[size];

		file.read(buffer, size);
		file.close();
		DeleteFile(path.string().c_str());

		CreateDirectory(path.string().c_str(), NULL);

		std::ofstream output(std::string(path.string() + "/" + "My Config").c_str(), std::ofstream::binary);
		output.write(buffer, 4);

		delete[] buffer;
		output.close();
	}
	else {
		DWORD dwAttr = GetFileAttributes(path.string().c_str());
		if (dwAttr == INVALID_FILE_ATTRIBUTES)
			CreateDirectory(path.string().c_str(), NULL);
	}

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(std::string(path.string() + "/*").c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0)
				configs.push_back(std::string(data.cFileName));
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	load("Default");
}

void Config::load(std::string item) noexcept
{
	if (std::strcmp(item.c_str(), "Default") == 0) {
		currentConfig = "Default";
		reset();
		return;
	}

	std::filesystem::path configPath = path;
	const char* fileName = item.c_str();
	configPath /= fileName;

	std::ifstream in(configPath);

    if (!in.good())
        return;

    cereal::BinaryInputArchive inputArchive{ in };
    try {
        inputArchive(aimbot, triggerbot, glow, chams, visuals, knifeChanger, misc);
    }
    catch (cereal::Exception&) { }
    in.close();
	currentConfig = item;
}

void Config::save(std::string item) const noexcept
{
	std::filesystem::path configPath = path;
	const char* fileName = item.c_str();
	configPath /= fileName;

	std::ofstream out(configPath);

    if (!out.good())
        return;

    cereal::BinaryOutputArchive outputArchive{ out };
    try {
        outputArchive(aimbot, triggerbot, glow, chams, visuals, knifeChanger, misc);
    }
    catch (cereal::Exception&) { }
    out.close();
}

void Config::add(std::string item) noexcept
{
	configs.push_back(item);
	save(item);
}

void Config::remove(std::string item) noexcept
{
	auto to_remove = std::find(configs.begin(), configs.end(), item);
	if (to_remove != configs.end())
		configs.erase(to_remove);

	DeleteFile(std::string(path.string() + "/" + item).c_str());
}

void Config::rename(std::string item, std::string newName) noexcept
{
	auto to_rename = std::find(configs.begin(), configs.end(), item);
	if (to_rename != configs.end())
	{
		auto index = std::distance(configs.begin(), to_rename);
		configs[index] = newName;
	}

	std::rename(std::string(path.string() + "/" + item).c_str(), std::string(path.string() + "/" + newName).c_str());
}

void Config::reset() noexcept
{
    aimbot = { };
    triggerbot = { };
    glow = { };
    chams = { };
    visuals = { };
    knifeChanger = { };
    misc = { };
}

std::vector<std::string> Config::getConfigs() noexcept
{
	return configs;
}
