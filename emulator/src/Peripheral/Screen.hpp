#pragma once
#include "../Config.hpp"
#include "Peripheral.hpp"
#include "../Chipset/MMURegion.hpp"
#include "../Data/HardwareId.hpp"
#include "../Data/SpriteInfo.hpp"
#include "../Data/ColourInfo.hpp"
#include "../Logger.hpp"
#include "../Chipset/MMU.hpp"
#include "../Emulator.hpp"
#include "../Chipset/Chipset.hpp"
#include <vector>
#include <SDL.h> // Assuming SDL_Renderer and SDL_Texture are from SDL

namespace casioemu
{
	struct SpriteBitmap
	{
		const char *name;
		uint8_t mask, offset;
	};

    // Forward declaration for Emulator
    class Emulator;

    // Define SpriteEnums struct for each HardwareId
    template <HardwareId hardware_id>
    struct SpriteEnums {};

    template <>
    struct SpriteEnums<HW_CLASSWIZ_II> {
        enum Sprite : unsigned {
            SPR_PIXEL, SPR_S, SPR_MATH, SPR_D, SPR_R, SPR_G, SPR_FIX, SPR_SCI, SPR_FX, SPR_E,
            SPR_CMPLX, SPR_ANGLE, SPR_WDOWN, SPR_VERIFY, SPR_GX, SPR_LEFT, SPR_DOWN, SPR_UP,
            SPR_RIGHT, SPR_PAUSE, SPR_SUN, SPR_MAX
        };
    };

    template <>
    struct SpriteEnums<HW_CLASSWIZ> {
        enum Sprite : unsigned {
            SPR_PIXEL, SPR_S, SPR_A, SPR_M, SPR_STO, SPR_MATH, SPR_D, SPR_R, SPR_G, SPR_FIX,
            SPR_SCI, SPR_E, SPR_CMPLX, SPR_ANGLE, SPR_WDOWN, SPR_LEFT, SPR_DOWN, SPR_UP,
            SPR_RIGHT, SPR_PAUSE, SPR_SUN, SPR_MAX
        };
    };

    template <>
    struct SpriteEnums<HW_ES_PLUS> {
        enum Sprite : unsigned {
            SPR_PIXEL, SPR_S, SPR_A, SPR_M, SPR_STO, SPR_RCL, SPR_STAT, SPR_CMPLX, SPR_MAT,
            SPR_VCT, SPR_D, SPR_R, SPR_G, SPR_FIX, SPR_SCI, SPR_MATH, SPR_DOWN, SPR_UP,
            SPR_DISP, SPR_MAX
        };
    };

	template <HardwareId hardware_id>
	class Screen : public Peripheral
	{
	public:
		using Peripheral::Peripheral;
        using Sprite = typename SpriteEnums<hardware_id>::Sprite; // Use the specialized enum

		void Initialise();
		void Uninitialise();
		void Frame();

		static int const N_ROW, // excluding the 1 row used for status line
			ROW_SIZE, // bytes
			OFFSET, // bytes
			ROW_SIZE_DISP; // bytes used to display

		static const SpriteBitmap sprite_bitmap[];

	private:
		MMURegion region_buffer, region_buffer1, region_contrast, region_mode, region_range, region_select;
		uint8_t *screen_buffer, *screen_buffer1, screen_contrast, screen_mode, screen_range, screen_select;

	    SDL_Renderer *renderer;
	    SDL_Texture *interface_texture;

		std::vector<SpriteInfo> sprite_info;
		ColourInfo ink_colour;

		/**
		 * Similar to MMURegion::DefaultRead, but takes the pointer to the Screen
		 * object as the userdata instead of the uint8_t member.
		 */
		template<typename value_type, value_type mask = (value_type)-1,
			value_type Screen:: *member_ptr>
		static uint8_t DefaultRead(MMURegion *region, size_t offset)
		{
			auto this_obj = (Screen *)(region->userdata);
			value_type value = this_obj->*member_ptr;
			return (value & mask) >> ((offset - region->base) * 8);
		}

		/**
		 * Similar to MMURegion::DefaultWrite, except this also set the
		 * (require_frame) flag of (Peripheral) class.
		 * If (only_on_change) is true, (require_frame) is not set if the new value
		 * is the same as the old value.
		 * (region->userdata) should be a pointer to a (Screen) instance.
		 *
		 * TODO: Probably this should be a member of Peripheral class instead.
		 * (in that case (Screen) class needs to be parameterized)
		 */
		template<typename value_type, value_type mask = (value_type)-1,
			value_type Screen:: *member_ptr, bool only_on_change = true>
		static void SetRequireFrameWrite(MMURegion *region, size_t offset, uint8_t data)
		{
			auto this_obj = (Screen *)(region->userdata);
			value_type &value = this_obj->*member_ptr;

			value_type old_value;
			if (only_on_change)
				old_value = value;

			// This part is identical to MMURegion::DefaultWrite.
			// * TODO Try to avoid duplication?
			value &= ~(((value_type)0xFF) << ((offset - region->base) * 8));
			value |= ((value_type)data) << ((offset - region->base) * 8);
			value &= mask;

			if (only_on_change && old_value == value)
				return;
			this_obj->require_frame = true;
		}
	};

	Peripheral *CreateScreen(Emulator& emulator);
}
