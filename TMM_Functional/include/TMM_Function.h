#pragma once

// REQUIRED internal include
#include <TMM_Setup.h>
#include "TMM_Callable.h"

#define TMM_LAMBDA +[]

namespace TMM {
	template<typename Ret, typename... Args>
	class Function : public Callable<Ret, Args...> {
		Ret(*mpMethod)(Args...);
	public:
		Function(Ret(* pMethod)(Args...));

		void* Ptr();
		Ret Call(Args... args) const override;
		Ret operator()(Args... args) const override;
	};

	template <typename Ret, typename... Args>
	auto MakeFunction(Ret(*method)(Args...));

	template <typename Ret>
	auto MakeFunction(Ret(*method)());

	template <typename Ret, typename... Args>
	auto MakeFunctionPtr(Ret(*method)(Args...));

	template <typename Ret>
	auto MakeFunctionPtr(Ret(*method)());
}

#include <TMM_Function.hpp>
