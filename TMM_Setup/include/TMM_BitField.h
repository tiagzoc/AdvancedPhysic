#pragma once

// REQUIRED internal include
#include <TMM_Setup.h>

namespace TMM
{
	template<unsigned char SIZE_8 = 1>
	class BitField
	{
		unsigned char mBytes[SIZE_8] = {};

		// for i e [0, 7] ex. i = 3 => 0b00001000
		constexpr unsigned char Mask(unsigned short i_bit) {
			return 1 << i_bit;
		}

		// for i e [0, 7] ex. i = 3 => 0b11110111
		constexpr unsigned char Mask_reversed(unsigned short i_bit) {
			return ~(1 << i_bit);
		}

	public:
		BitField() = default;

		// Use : TMM_BITFIELD_MASK
		constexpr void ApplyOR(unsigned char mask, unsigned char i_byte = 0)
		{
			mBytes[i_byte] |= mask;
		}

		// Use : TMM_BITFIELD_MASK
		constexpr void ApplyAND(unsigned char mask, unsigned char i_byte = 0)
		{
			mBytes[i_byte] &= mask;
		}

		// Use : TMM_BITFIELD_MASK
		constexpr void ApplyXOR(unsigned char mask, unsigned char i_byte = 0)
		{
			mBytes[i_byte] ^= mask;
		}

		// Use : TMM_BITFIELD_MASK
		constexpr void ApplyNOR(unsigned char mask, unsigned char i_byte = 0)
		{
			mBytes[i_byte] |= mask;
			mBytes[i_byte] = ~mBytes[i_byte];
		}

		// Use : TMM_BITFIELD_MASK
		constexpr void ApplyNAND(unsigned char mask, unsigned char i_byte = 0)
		{
			mBytes[i_byte] &= mask;
			mBytes[i_byte] = ~mBytes[i_byte];
		}

		// Use : TMM_BITFIELD_MASK
		constexpr void ApplyNXOR(unsigned char mask, unsigned char i_byte = 0)
		{
			mBytes[i_byte] ^= mask;
			mBytes[i_byte] = ~mBytes[i_byte];
		}

		constexpr bool Get(unsigned char i_bit, unsigned char i_byte = 0)
		{
			return mBytes[i_byte] & Mask(i_bit);
		}

		constexpr void Toggle(unsigned char i_bit, unsigned char i_byte = 0)
		{
			mBytes[i_byte] ^= Mask(i_bit);
		}

		constexpr void SetTrue(unsigned char i_bit, unsigned char i_byte = 0) 
		{
			mBytes[i_byte] |= Mask(i_bit);
		}

		constexpr void SetFalse(unsigned char i_bit, unsigned char i_byte = 0)
		{
			mBytes[i_byte] &= Mask_reversed(i_bit);
		}

		constexpr void operator |= (unsigned char mask)
		{
			static_assert(SIZE_8 == 1, "Cant use operator |= on BitField with more than 8bits");
			mBytes[0] |= mask;
		}

		constexpr void operator &= (unsigned char mask)
		{
			static_assert(SIZE_8 == 1, "Cant use operator &= BitField with more than 8bits");
			mBytes[0] &= mask;
		}

		constexpr void operator ^= (unsigned char mask)
		{
			static_assert(SIZE_8 == 1, "Cant use operator ^= BitField with more than 8bits");
			mBytes[0] ^= mask;
		}

		constexpr void operator ~()
		{
			static_assert(SIZE_8 == 1, "Cant use operator ~ on BitField with more than 8bits");
			mBytes[0] = ~mBytes[0];
		}
	};

}