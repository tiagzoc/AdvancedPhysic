#pragma once

#ifndef NDEBUG

#define _CRTDBG_MAP_ALLOC
#define D3D_DEBUG_INFO
#include <crtdbg.h>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__ , __LINE__)
#define new DEBUG_NEW

#endif // !NDEBUG

#ifdef _MSC_VER
#define ALIGN16 __declspec(align(16))
#define ALIGN(size) __declspec(align(size))
#else
#define ALIGN16 __attribute__((aligned(16)))
#define ALIGN(size) __attribute__((aligned(size)))
#endif

// Notation Defines

#define PURE = 0

// Compute Manip

#define TMM_MAX(a, b) ((a) < (b) ? (b) : (a))
#define TMM_MIN(a, b) ((a) < (b) ? (a) : (b))
#define TMM_CLAMP(v, v_min, v_max) ((v) < (v_min) ? (v_min) : ((v) > (v_max) ? (v_max) : (v)))
#define TMM_CLAMP01(v) TMM_CLAMP(v, 0, 1)
#define TMM_SWAP(a, b) auto temp = (a); (a) = (b); (b) = temp;
#define TMM_SWAP_XOR(a, b) (a) = (a) ^ (b); (b) = (a) ^ (b); (a) = (a) ^ (b);
#define TMM_LERP(a, b, t) (a) + (t) * ((b) - (a))

// SHORTCUT

#define TMM_ITER_t(type, var, max) for(type (var) = 0; (var) < (max); ++(var))
#define TMM_ITER(var, max) TMM_ITER_t(uint64_t, var, max)

// Bit Manip

#define TMM_ON  0b1
#define TMM_OFF 0b0

#define TMM_BIT(n) (1 >> n)
#define TMM_BIT0 0b00000001
#define TMM_BIT1 0b00000010
#define TMM_BIT2 0b00000100
#define TMM_BIT3 0b00001000
#define TMM_BIT4 0b00010000
#define TMM_BIT5 0b00100000
#define TMM_BIT6 0b01000000
#define TMM_BIT7 0b10000000

#define TMM_BITFIELD_MASK_ALL  0b11111111
#define TMM_BITFIELD_MASK_NONE 0b00000000
#define TMM_BITFIELD_MASK(bit0, bit1, bit2, bit3, bit4, bit5, bit6, bit7) ( \
				   (bit0) | ((bit1) << 1) | ((bit2) << 2 ) | ((bit3) << 3 ) | \
			((bit4) << 4) | ((bit5) << 5) | ((bit6) << 6 ) | ((bit7) << 7 ) \
	)

// REQUIRED external include
#include <stdint.h>

namespace TMM {

	namespace CONSTANT
	{
		static constexpr double E					= 2.71828182845904523536;
		static constexpr double GOLDEN_RATION		= 1.61803398874989484820;
		static constexpr double SQ_ROOT2			= 1.41421356237309504880;
		static constexpr double SQ_ROOT3			= 1.73205080756887729352;
		static constexpr double GRAVITY_CONTANT		= 9.80665;
		static constexpr double PI					= 3.14159265358979323846;
		static constexpr double PI2_MUL				= PI * 2.0;
		static constexpr double PI2_DIV				= PI * 0.5;
		static constexpr double PI4_DIV				= PI * 0.25;
		static constexpr double PI6_DIV				= PI * 0.16666666666666666666;
	}

	// === USING ===

	// Units 

	// date
	using Year		= unsigned int;
	using Month		= unsigned int;
	using Day		= unsigned int;

	// time
	using Hour_f			= float;
	using Hour_u			= unsigned int;
	using Minute_f			= float;
	using Minute_u			= unsigned int;
	using Second_f			= float;
	using Second_u			= unsigned int;
	using Millisecond_f		= float;
	using Millisecond_u		= unsigned int;
	using Microsecond_u		= unsigned long long;
	using Nanosecond_u		= unsigned long long;

	// angle
	using Degree_f = float;
	using Degree_d = double;
	using Radian_f = float;
	using Radian_d = double;


	// === ENUMS ===

	enum class BASE_TYPE : uint32_t
	{
		UNKNOWN,

		BOOL,

		FLOAT,
		DOUBLE,

		UINT8,
		UINT16,
		UINT32,
		UINT64,

		INT8,
		INT16,
		INT32,
		INT64
	};

	// === ERRORS ===

	using ERROR_CODE = int;
#define TMM_FAILED(code) (code) != 0
#define TMM_FAILED_WITH(code, err) (code) & (err) != 0

	// === COMPILE-TIME MANIPULATION ===

	// ==  BOOLEAN  ==

    template<typename A, typename B>
    struct is_same {
        static constexpr bool value = false;
    };

    template<typename A>
    struct is_same<A, A> {
        static constexpr bool value = true;
    };

	template<typename T, typename... TEST>
	struct is_one_of {
		static constexpr bool value = (false || ... || TMM::is_same<T, TEST>::value);
	};

	template<typename T>
	struct is_array {
		static constexpr bool value = false;
	};

	template<typename T, uint64_t SIZE>
	struct is_array<T[SIZE]> {
		static constexpr bool value = true;
	};

	template<typename T>
	struct is_function {
		static constexpr bool value = false;
	};

	template<typename Ret, typename... Args>
	struct is_function<Ret(Args...)> {
		static constexpr bool value = true;
	};

	template<typename Ret, typename... Args>
	struct is_function<Ret(Args...) noexcept> {
		static constexpr bool value = true;
	};

	// check if two <typename> are the same
	template<typename A, typename B>
	concept Same = is_same<A, B>::value;

	// check if <typename T> is one of the differents <typename TEST>
	template<typename T, typename... TEST>
	concept OneOf = is_one_of<T, TEST...>::value;

	// extention of OneOf, check if <typename> T is a native type reprensenting a number
	template<typename T>
	concept Countable = OneOf <T,
		unsigned char, unsigned short, unsigned int, unsigned long long,
		char, short, int, long long,
		float, double
	>;

	// extention of OneOf, check if <typename> T is a native type reprensenting a decimal number
	template<typename T>
	concept Decimal = OneOf <T,
		float, double
	>;

	template<typename T>
	concept HasOperatorEq = requires(const T & a, const T & b) { a == b; };

	template<typename T>
	concept HasOperatorSup = requires(const T & a, const T & b) { a > b; };

	template<typename T>
	concept HasOperatorInf = requires(const T & a, const T & b) { a < b; };

	template<typename T>
	concept HasOperatorSupEq = requires(const T & a, const T & b) { a >= b; };

	template<typename T>
	concept HasOperatorInfEq = requires(const T & a, const T & b) { a <= b; };

	template<typename T>
	concept HasOperatorDif = requires(const T & a, const T & b) { a != b; };

	template<typename T>
	concept HasAllOperatorLogic = HasOperatorEq<T> && HasOperatorInf<T> && HasOperatorSup<T> && HasOperatorSupEq<T> && HasOperatorInfEq<T> && HasOperatorDif<T>;

	template<typename T>
	concept HasOperatorPlus = requires(const T & a, const T & b) { a + b; };

	template<typename T>
	concept HasOperatorMinus = requires(const T & a, const T & b) { a - b; };

	template<typename T>
	concept HasOperatorMult = requires(const T & a, const T & b) { a * b; };

	template<typename T>
	concept HasOperatorDiv = requires(const T & a, const T & b) { a / b; };

	// check if <typename T> is an array (T myArr[xxx])
	template<typename T>
	constexpr bool IsArray = is_array<T>::value;

	// check if <typename T> is a function (ex. int(void*, bool))
	template<typename T>
	constexpr bool IsFunction = is_function<T>::value;

	// check if <typename T> is an enum
	template<typename T>
	constexpr bool IsEnum = __is_enum(T);

	// check if <typename Base> is a parent of <typename Derived>
	template<typename Base, typename Derived>
	constexpr bool IsBaseOf = __is_base_of(Base, Derived);

	// ==  TYPE  ==

	// Used with following Add_pointer to create a void* usable in any situation for compile time checks
	template <class... _Types>
	using void_t = void;

	template<typename T>
	struct remove_ref { using type = T; };

	template<typename T>
	struct remove_ref<T&> { using type = T; };

	template<typename T>
	struct remove_ref<T&&> { using type = T; };

	// Utilitary to remove reference in <typename> (ex. int => int // int& => int // int&& => int)
	template<typename T>
	using Remove_ref_t = typename remove_ref<T>::type;

	template <bool>
	struct select { 
		template <class _Ty1, class>
		using Apply = _Ty1;
	};

	template <>
	struct select<false> {
		template <class, class _Ty2>
		using Apply = _Ty2;
	};

	// A compile-time check to select between two output (ex. Select_t<sizeof(T) > 1, int, char> = (int) if (sizeof(T) > 1) else (char))
	template <bool TEST, class A, class B>
	using Select_t = typename select<TEST>::template Apply<A, B>;

	template <class T, class = void>
	struct Add_pointer {
		using type = T;
	};

	template<class T>
	struct Add_pointer<T, void_t<Remove_ref_t<T>>*> {
		using type = Remove_ref_t<T>*;
	};

	// Transform the <class> T to be a T* (remove any reference) (ex. int => int* // int& => int*)
	template <class T>
	using Add_pointer_t = typename Add_pointer<T>::type;

	template <class T>
	struct decay {
		using _T1 = Remove_ref_t<T>;
		using _T2 = typename select<IsFunction<_T1>>::template Apply<Add_pointer_t<_T1>, _T1>;
		using type = typename select<IsArray<_T1>>::template Apply<Add_pointer_t<_T1>, _T2>;
	};

	// return if (T is a function) => a ptr to this function else if (T is an array) => a ptr to this array
	template <class T>
	using Decay_t = decay<T>::type;

	 
	template<typename T>
	struct MAX_INTEGER {
		const static long long VALUE = (0x1ull << (sizeof(T) * 8 - 1)) - 1;
	};

	template<typename T>
	struct MIN_INTEGER {
		const static long long VALUE = -TMM::MAX_INTEGER<T>::VALUE - 1;
	};

	//template<>
	//struct MAX_INTEGER<long long>
	//{
	//	const static long long SIZE = 9223372036854775807;
	//};

	//template<>
	//struct MIN_INTEGER<long long>
	//{
	//	const static long long SIZE = -9223372036854775808;
	//};

	// === RUNNTIME MANIPULATION

	template <class T>
	constexpr T&& forward(Remove_ref_t<T>& _Arg) noexcept {
		return static_cast<T&&>(_Arg);
	}

	template <class T>
	constexpr T&& forward(Remove_ref_t<T>&& _Arg) noexcept {
		return static_cast<T&&>(_Arg);
	}
}

#include "TMM_BitField.h"
#include "TMM_BitMask.h"
#include "TMM_String.h"
#include "TMM_StringBuilder.h"
