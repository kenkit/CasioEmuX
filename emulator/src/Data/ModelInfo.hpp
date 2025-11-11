#pragma once
#include "../Config.hpp"

#include <string>

namespace casioemu
{
	class Emulator;
	class SpriteInfo;
	class ColourInfo;

	struct ModelInfo
	{
		ModelInfo(Emulator *emulator, std::string key);
		Emulator *emulator;
		std::string key;

		std::string asString();
		int asInt();
		SpriteInfo asSpriteInfo();
		ColourInfo asColourInfo();
	};
}