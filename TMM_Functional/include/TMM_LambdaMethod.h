#pragma once

#include <TMM_Setup.h>
#include "TMM_Callable.h"

namespace TMM
{
	template<typename Ret, typename... Args>
	class LambdaMethod : public Callable<Ret, Args...>
	{
		using CallFn = Ret(*)(void*, Args...);
		using DestroyFn = void(*)(void*);
		void* mpObj = nullptr;
		CallFn mCall = nullptr;
		DestroyFn mDestroy = nullptr;
	public:
		LambdaMethod() = default;

		template<typename F>
		LambdaMethod(F&& f);

		~LambdaMethod();

		Ret operator()(Args... args) const override;

		Ret Call(Args... args) const override;
	};

	template<typename T>
	struct LambdaTraits;

	template<typename C, typename Ret, typename... Args>
	struct LambdaTraits<Ret(C::*)(Args...) const>
	{
		using Return = Ret;
		using Method = LambdaMethod<Ret, Args...>;
	};

	template<typename F>
	auto MakeLambdaMethodPtr(F&& f);

	template<typename F>
	auto MakeLambdaMethod(F&& f);
}

#include "TMM_LambdaMethod.hpp"