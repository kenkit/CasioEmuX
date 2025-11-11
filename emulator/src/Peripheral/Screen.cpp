#include "Screen.hpp"

#include "../Chipset/MMURegion.hpp"
#include "../Data/HardwareId.hpp"
#include "../Data/SpriteInfo.hpp"
#include "../Data/ColourInfo.hpp"
#include "../Logger.hpp"
#include "../Chipset/MMU.hpp"
#include "../Emulator.hpp"
#include "../Chipset/Chipset.hpp"

#include <vector>

namespace casioemu
{
	template <> const int Screen<HW_CLASSWIZ_II>::N_ROW = 63;
	template <> const int Screen<HW_CLASSWIZ_II>::ROW_SIZE = 32;
	template <> const int Screen<HW_CLASSWIZ_II>::OFFSET = 32;
	template <> const int Screen<HW_CLASSWIZ_II>::ROW_SIZE_DISP = 24;

	template <> const int Screen<HW_CLASSWIZ>::N_ROW = 63;
	template <> const int Screen<HW_CLASSWIZ>::ROW_SIZE = 32;
	template <> const int Screen<HW_CLASSWIZ>::OFFSET = 32;
	template <> const int Screen<HW_CLASSWIZ>::ROW_SIZE_DISP = 24;

	template <> const int Screen<HW_ES_PLUS>::N_ROW = 31;
	template <> const int Screen<HW_ES_PLUS>::ROW_SIZE = 16;
	template <> const int Screen<HW_ES_PLUS>::OFFSET = 16;
	template <> const int Screen<HW_ES_PLUS>::ROW_SIZE_DISP = 12;

	template<> const SpriteBitmap Screen<HW_CLASSWIZ_II>::sprite_bitmap[SpriteEnums<HW_CLASSWIZ_II>::SPR_MAX] = {
		{"rsd_pixel",    0,    0},
		{"rsd_s",     0x01, 0x01},
		{"rsd_math",  0x01, 0x03},
		{"rsd_d",     0x01, 0x04},
		{"rsd_r",     0x01, 0x05},
		{"rsd_g",     0x01, 0x06},
		{"rsd_fix",   0x01, 0x07},
		{"rsd_sci",   0x01, 0x08},
		{"rsd_fx",    0x01, 0x09},
		{"rsd_e",     0x01, 0x0A},
		{"rsd_cmplx", 0x01, 0x0B},
		{"rsd_angle", 0x01, 0x0C},
		{"rsd_wdown", 0x01, 0x0D},
		{"rsd_verify",0x01, 0x0E},
		{"rsd_gx",    0x01, 0x0F},
		{"rsd_left",  0x01, 0x10},
		{"rsd_down",  0x01, 0x11},
		{"rsd_up",    0x01, 0x12},
		{"rsd_right", 0x01, 0x13},
		{"rsd_pause", 0x01, 0x15},
		{"rsd_sun",   0x01, 0x16}
	};

	template<> const SpriteBitmap Screen<HW_CLASSWIZ>::sprite_bitmap[SpriteEnums<HW_CLASSWIZ>::SPR_MAX] = {
		{"rsd_pixel",    0,    0},
		{"rsd_s",     0x01, 0x00},
		{"rsd_a",     0x01, 0x01},
		{"rsd_m",     0x01, 0x02},
		{"rsd_sto",   0x01, 0x03},
		{"rsd_math",  0x01, 0x05},
		{"rsd_d",     0x01, 0x06},
		{"rsd_r",     0x01, 0x07},
		{"rsd_g",     0x01, 0x08},
		{"rsd_fix",   0x01, 0x09},
		{"rsd_sci",   0x01, 0x0A},
		{"rsd_e",     0x01, 0x0B},
		{"rsd_cmplx", 0x01, 0x0C},
		{"rsd_angle", 0x01, 0x0D},
		{"rsd_wdown", 0x01, 0x0F},
		{"rsd_left",  0x01, 0x10},
		{"rsd_down",  0x01, 0x11},
		{"rsd_up",    0x01, 0x12},
		{"rsd_right", 0x01, 0x13},
		{"rsd_pause", 0x01, 0x15},
		{"rsd_sun",   0x01, 0x16}
	};

	template<> const SpriteBitmap Screen<HW_ES_PLUS>::sprite_bitmap[SpriteEnums<HW_ES_PLUS>::SPR_MAX] = {
		{"rsd_pixel",    0,    0},
		{"rsd_s",     0x10, 0x00},
		{"rsd_a",     0x04, 0x00},
		{"rsd_m",     0x10, 0x01},
		{"rsd_sto",   0x02, 0x01},
		{"rsd_rcl",   0x40, 0x02},
		{"rsd_stat",  0x40, 0x03},
		{"rsd_cmplx", 0x80, 0x04},
		{"rsd_mat",   0x40, 0x05},
		{"rsd_vct",   0x01, 0x05},
		{"rsd_d",     0x20, 0x07},
		{"rsd_r",     0x02, 0x07},
		{"rsd_g",     0x10, 0x08},
		{"rsd_fix",   0x01, 0x08},
		{"rsd_sci",   0x20, 0x09},
		{"rsd_math",  0x40, 0x0A},
		{"rsd_down",  0x08, 0x0A},
		{"rsd_up",    0x80, 0x0B},
		{"rsd_disp",  0x10, 0x0B}
	};

	template <HardwareId hardware_id> void Screen<hardware_id>::Initialise()
	{
		auto constexpr SPR_MAX = SpriteEnums<hardware_id>::SPR_MAX;

		static_assert(SPR_MAX == (sizeof(sprite_bitmap) / sizeof(sprite_bitmap[0])), "SPR_MAX and sizeof(sprite_bitmap) don't match");

	    renderer = emulator.GetRenderer();
	    interface_texture = emulator.GetInterfaceTexture();
		sprite_info.resize(SPR_MAX);
		for (int ix = 0; ix != SPR_MAX; ++ix)
			sprite_info[ix] = emulator.GetModelInfo(sprite_bitmap[ix].name).asSpriteInfo();
		
		ink_colour = emulator.GetModelInfo("ink_colour").asColourInfo();
		require_frame = true;

		screen_buffer = new uint8_t[(N_ROW + 1) * ROW_SIZE];

		if (emulator.hardware_id != HW_CLASSWIZ_II) {
			region_buffer.Setup(0xF800, (N_ROW + 1) * ROW_SIZE, "Screen/Buffer", this, [](MMURegion *region, size_t offset) {
				offset -= region->base;
				if (offset % ROW_SIZE >= ROW_SIZE_DISP)
					return (uint8_t)0;
				return ((Screen *)region->userdata)->screen_buffer[offset];
			}, [](MMURegion *region, size_t offset, uint8_t data) {
				offset -= region->base;
				if (offset % ROW_SIZE >= ROW_SIZE_DISP)
					return;

				auto this_obj = (Screen *)region->userdata;
				// * Set require_frame to true only if the value changed.
				this_obj->require_frame |= this_obj->screen_buffer[offset] != data;
				this_obj->screen_buffer[offset] = data;
			}, emulator);
		} else {
			screen_buffer1 = new uint8_t[(N_ROW + 1) * ROW_SIZE];
			region_select.Setup(0xF037, 1, "Screen/Select", this, DefaultRead<uint8_t, 0x04, &Screen::screen_select>,
				SetRequireFrameWrite<uint8_t, 0x04, &Screen::screen_select>, emulator);
			if(!static_cast<bool>(emulator.GetModelInfo("real_hardware").asInt())) {
				region_buffer.Setup(0xF800, (N_ROW + 1) * ROW_SIZE, "Screen/Buffer", this, [](MMURegion *region, size_t offset) {
					offset -= region->base;
					if (offset % ROW_SIZE >= ROW_SIZE_DISP)
						return (uint8_t)0;
					return ((Screen *)region->userdata)->screen_buffer[offset];
				}, [](MMURegion *region, size_t offset, uint8_t data) {
					offset -= region->base;
					if (offset % ROW_SIZE >= ROW_SIZE_DISP)
						return;

					auto this_obj = (Screen *)region->userdata;
					// * Set require_frame to true only if the value changed.
					this_obj->require_frame |= this_obj->screen_buffer[offset] != data;
					this_obj->screen_buffer[offset] = data;
				}, emulator);
				region_buffer1.Setup(0x89000, (N_ROW + 1) * ROW_SIZE, "Screen/Buffer1", this, [](MMURegion* region, size_t offset) {
					offset -= region->base;
					if (offset % ROW_SIZE >= ROW_SIZE_DISP)
						return (uint8_t)0;
					return ((Screen*)region->userdata)->screen_buffer1[offset];
				}, [](MMURegion* region, size_t offset, uint8_t data) {
					offset -= region->base;
					if (offset % ROW_SIZE >= ROW_SIZE_DISP)
						return;

					auto this_obj = (Screen*)region->userdata;
					// * Set require_frame to true only if the value changed.
					this_obj->require_frame |= this_obj->screen_buffer1[offset] != data;
					this_obj->screen_buffer1[offset] = data;
				}, emulator);
			} else {
				region_buffer.Setup(0xF800, (N_ROW + 1) * ROW_SIZE, "Screen/Buffer", this, [](MMURegion *region, size_t offset) {
					offset -= region->base;
					if (offset % ROW_SIZE >= ROW_SIZE_DISP)
						return (uint8_t)0;
					if(((Screen *)region->userdata)->screen_select & 0x04) {
						return ((Screen *)region->userdata)->screen_buffer1[offset];
					} else {
						return ((Screen *)region->userdata)->screen_buffer[offset];
					}
				}, [](MMURegion *region, size_t offset, uint8_t data) {
					offset -= region->base;
					if (offset % ROW_SIZE >= ROW_SIZE_DISP)
						return;

					auto this_obj = (Screen *)region->userdata;
					// * Set require_frame to true only if the value changed.
					if(((Screen *)region->userdata)->screen_select & 0x04) {
						this_obj->require_frame |= this_obj->screen_buffer1[offset] != data;
						this_obj->screen_buffer1[offset] = data;
					} else {
						this_obj->require_frame |= this_obj->screen_buffer[offset] != data;
						this_obj->screen_buffer[offset] = data;
					}
				}, emulator);
			}
		}

		region_range.Setup(0xF030, 1, "Screen/Range", this, DefaultRead<uint8_t, 0x07, &Screen::screen_range>,
				SetRequireFrameWrite<uint8_t, 0x07, &Screen::screen_range>, emulator);

		region_mode.Setup(0xF031, 1, "Screen/Mode", this, DefaultRead<uint8_t, 0x07, &Screen::screen_mode>,
				SetRequireFrameWrite<uint8_t, 0x07, &Screen::screen_mode>, emulator);

		region_contrast.Setup(0xF032, 1, "Screen/Contrast", this, DefaultRead<uint8_t, 0x3F, &Screen::screen_contrast>,
				SetRequireFrameWrite<uint8_t, 0x3F, &Screen::screen_contrast>, emulator);
	}

	template<HardwareId hardware_id> void Screen<hardware_id>::Uninitialise()
	{
		delete[] screen_buffer;
		if(emulator.hardware_id == HW_CLASSWIZ_II)
			delete[] screen_buffer1;
	}

	template<HardwareId hardware_id> void Screen<hardware_id>::Frame()
	{
		typename Screen<hardware_id>::Sprite SPR_PIXEL_ENUM = Screen<hardware_id>::Sprite::SPR_PIXEL;
		typename Screen<hardware_id>::Sprite SPR_MAX_ENUM = Screen<hardware_id>::Sprite::SPR_MAX;

		require_frame = false;

		int ink_alpha_on = 20 + screen_contrast * 16;
		if (ink_alpha_on > 255)
			ink_alpha_on = 255;
		int ink_alpha_off = (screen_contrast - 8) * 2;
		if (ink_alpha_off < 0)
			ink_alpha_off = 0;

		bool enable_status, enable_dotmatrix, clear_dots;

		switch (screen_mode)
		{
		case 4:
			enable_dotmatrix = true;
			clear_dots = true;
			enable_status = false;
			break;

		case 5:
			enable_dotmatrix = true;
			clear_dots = false;
			enable_status = true;
			break;

		case 6:
			enable_dotmatrix = true;
			clear_dots = true;
			enable_status = true;
			ink_alpha_on = 80;
			ink_alpha_off = 20;
			break;

		default:
			return;
		}

		SDL_SetTextureColorMod(interface_texture, ink_colour.r, ink_colour.g, ink_colour.b);

		if (enable_status)
		{
			int ink_alpha = ink_alpha_off;
			if(emulator.hardware_id == HW_CLASSWIZ_II && static_cast<bool>(emulator.GetModelInfo("real_hardware").asInt())) {
				for (int ix = SPR_PIXEL_ENUM + 1; ix != SPR_MAX_ENUM; ++ix)
				{
					ink_alpha = ink_alpha_off;
					if (screen_buffer[sprite_bitmap[ix].offset] & sprite_bitmap[ix].mask)
						ink_alpha += (ink_alpha_on - ink_alpha_off) * 0.333;
					if (screen_buffer1[sprite_bitmap[ix].offset] & sprite_bitmap[ix].mask)
						ink_alpha += (ink_alpha_on - ink_alpha_off) * 0.667;
					SDL_SetTextureAlphaMod(interface_texture, ink_alpha);
					SDL_RenderCopy(renderer, interface_texture, &sprite_info[ix].src, &sprite_info[ix].dest);
				}
			} else {
				for (int ix = SPR_PIXEL_ENUM + 1; ix != SPR_MAX_ENUM; ++ix)
				{
					if (screen_buffer[sprite_bitmap[ix].offset] & sprite_bitmap[ix].mask)
						SDL_SetTextureAlphaMod(interface_texture, ink_alpha_on);
					else
						SDL_SetTextureAlphaMod(interface_texture, ink_alpha_off);
					SDL_RenderCopy(renderer, interface_texture, &sprite_info[ix].src, &sprite_info[ix].dest);
				}
			}
		}

		if (enable_dotmatrix)
		{
			SDL_Rect dest = Screen<hardware_id>::sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].dest;
			int ink_alpha = ink_alpha_off;
			if (emulator.hardware_id == HW_CLASSWIZ_II) {
				for (int iy = 0; iy != N_ROW; ++iy)
				{
					dest.x = sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].dest.x;
					dest.y = sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].dest.y + iy * sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].src.h;
					for (int ix = 0; ix != ROW_SIZE_DISP; ++ix)
					{
						for (uint8_t mask = 0x80; mask; mask >>= 1, dest.x += sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].src.w)
						{
							ink_alpha = ink_alpha_off;
							if (!clear_dots && screen_buffer[iy * ROW_SIZE + OFFSET + ix] & mask)
								ink_alpha += static_cast<int>((ink_alpha_on - ink_alpha_off) * 0.333);
							if (!clear_dots && screen_buffer1[iy * ROW_SIZE + OFFSET + ix] & mask)
								ink_alpha += static_cast<int>((ink_alpha_on - ink_alpha_off) * 0.667);
							SDL_SetTextureAlphaMod(interface_texture, ink_alpha);
							SDL_RenderCopy(renderer, interface_texture, &sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].src, &dest);
						}
					}
				}
			}
			else {
				for (int iy = 0; iy != N_ROW; ++iy)
				{
					dest.x = sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].dest.x;
					dest.y = sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].dest.y + iy * sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].src.h;
					for (int ix = 0; ix != ROW_SIZE_DISP; ++ix)
					{
						for (uint8_t mask = 0x80; mask; mask >>= 1, dest.x += sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].src.w)
						{
							if (!clear_dots && screen_buffer[iy * ROW_SIZE + OFFSET + ix] & mask)
								SDL_SetTextureAlphaMod(interface_texture, ink_alpha_on);
							else
								SDL_SetTextureAlphaMod(interface_texture, ink_alpha_off);
							SDL_RenderCopy(renderer, interface_texture, &sprite_info[static_cast<int>(SpriteEnums<hardware_id>::Sprite::SPR_PIXEL)].src, &dest);
						}
					}
				}
			}
		}
	}

	Peripheral *CreateScreen(Emulator& emulator)
	{
		switch (emulator.hardware_id)
		{
		case HW_ES_PLUS:
		case HW_FX_5800P:
			return new Screen<HW_ES_PLUS>(emulator);

		case HW_CLASSWIZ:
			return new Screen<HW_CLASSWIZ>(emulator);

		case HW_CLASSWIZ_II:
			return new Screen<HW_CLASSWIZ_II>(emulator);
		default:
			PANIC("Unknown hardware id\n");
		}
	}
}