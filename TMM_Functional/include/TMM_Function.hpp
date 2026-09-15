#include "TMM_Function.h"

namespace TMM {

	template<typename Ret, typename ...Args>
	auto MakeFunction(Ret(*method)(Args...))
	{
		return Function<Ret, Args...>(method);
	}

	template<typename Ret>
	auto MakeFunction(Ret(*method)())
	{
		return Function<Ret>(method);
	}

	template<typename Ret, typename ...Args>
	auto MakeFunctionPtr(Ret(*method)(Args...))
	{
		return new Function<Ret, Args...>(method);
	}

	template<typename Ret>
	auto MakeFunctionPtr(Ret(*method)())
	{
		return new Function<Ret>(method);
	}

	template<typename Ret, typename ...Args>
	inline Function<Ret, Args...>::Function(Ret(*pMethod)(Args...))
		: mpMethod(pMethod) { }

	template<typename Ret, typename ...Args>
	inline void* Function<Ret, Args...>::Ptr()
	{
		return mpMethod;
	}

	template<typename Ret, typename ...Args>
	inline Ret Function<Ret, Args...>::Call(Args ...args) const
	{
		return (*mpMethod)(args...);
	}

	template<typename Ret, typename ...Args>
	inline Ret Function<Ret, Args...>::operator()(Args ...args) const
	{
		return Call(args...);
	}
}