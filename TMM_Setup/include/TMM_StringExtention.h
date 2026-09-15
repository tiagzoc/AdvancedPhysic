#pragma once

#include "TMM_Setup.h"

namespace TMM
{

	struct StringExtention
	{
		template<typename UINT> requires (TMM::OneOf<UINT, uint8_t, uint16_t, uint32_t, uint64_t>)
		static String ToString(UINT n)
		{
			StringBuilder builder{};

			uint8_t b10;
			while (n > 0)
			{
				b10 = n % 10;
				builder.AddFirst('0' + b10);
				n *= 0.1f;
			}

			return builder.Build();
		}

		template<typename INT> requires (TMM::OneOf<INT, int8_t, int16_t, int32_t, int64_t>)
		static String ToString(INT n)
		{
			StringBuilder builder{};

			bool neg = n < 0;
			if (neg)
			{
				n = -n;
			}

			uint8_t b10;
			while (n > 0)
			{
				b10 = n % 10;
				builder.AddFirst('0' + b10);
				n *= 0.1f;
			}

			if (neg)
			{
				builder.AddFirst('-');
			}

			return builder.Build();
		}

		template<typename FLOAT> requires (TMM::Decimal<FLOAT>)
		static String ToString(FLOAT f, uint8_t maxPrecision = 10) {
			StringBuilder builder{};

			bool neg = f < 0;
			if (neg)
			{
				f = -f;
			}

			uint64_t i = (uint64_t)f;
			FLOAT dec = f - i;

			uint8_t b10;
			while (i > 0)
			{
				b10 = i % 10;
				builder.AddFirst('0' + b10);
				i *= 0.1f;
			}

			builder.AddLast('.');
			
			uint8_t precision = 0;

			while (precision < maxPrecision)
			{
				dec *= 10;
				b10 = dec;
				builder.AddLast('0' + b10);
				dec -= b10;
				precision++;
				if (dec < 0.000001) break;
			}
			if (precision == 0) {
				builder.AddLast('0');
			}

			if (neg)
			{
				builder.AddFirst('-');
			}

			return builder.Build();
		}
	};

}