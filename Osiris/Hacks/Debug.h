#pragma once
#include <vector>
#include "../Interfaces.h"
#include "../Hacks/Backtrack.h"

enum class FrameStage;
class GameEvent;
struct UserCmd;


namespace Debug{

	struct screen {
		int Width = 0;
		int Height = 0;
		int CurrPosW = 0;
		int CurrPosH = 5;
		int CurrColumnMax = 0;

	};

	extern screen Screen;
	
	struct coords {
		int x;
		int y;
	};
	
	struct ColorInfo {
		bool enabled;
		float r;
		float g;
		float b;
		float a;
	};
	
	// Output
	void DrawBox(coords start, coords end);
	void DrawGraphBox(coords start, coords end, float min_val, float max_val, float val, float ratio, std::wstring name);
	bool SetupTextPos(std::vector <std::wstring>& Text);
	void Draw_Text(std::vector <std::wstring> &Text);
	void AnimStateMonitor() noexcept;
	std::vector<std::wstring> formatRecord(Backtrack::Record record, Entity* entity, int index);

	void BacktrackMonitor() noexcept;
	void DrawDesyncInfo();
	void run();

	//input 

	void AnimStateModifier() noexcept;

}
