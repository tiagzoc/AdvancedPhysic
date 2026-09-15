#include "TMM_Method.h"


namespace TMM {

	template<typename Obj, typename Ret, typename ...Args>
	auto MakeMethod(Obj* obj, Ret(Obj::* method)(Args...))
	{
		return Method<Obj, Ret, Args...>(obj, method);
	}

	template<typename Obj, typename Ret>
	auto MakeMethod(Obj* obj, Ret(Obj::* method)())
	{
		return Method<Obj, Ret>(obj, method);
	}

	template<typename Obj, typename Ret, typename ...Args>
	auto MakeMethodPtr(Obj* obj, Ret(Obj::* method)(Args...))
	{
		return new Method<Obj, Ret, Args...>(obj, method);
	}

	template<typename Obj, typename Ret>
	auto MakeMethodPtr(Obj* obj, Ret(Obj::* method)())
	{
		return new Method<Obj, Ret>(obj, method);
	}

	template<typename Obj, typename Ret, typename ...Args>
	inline Method<Obj, Ret, Args...>::Method(Obj* pObj, Ret(Obj::* pMethod)(Args...))
		: mpObj(pObj), mpMethod(pMethod)
	{

	}

	template<typename Obj, typename Ret, typename ...Args>
	inline Ret Method<Obj, Ret, Args...>::Call(Args ...args) const
	{
		return (mpObj->*mpMethod)(args...);
	}

	template<typename Obj, typename Ret, typename ...Args>
	inline Ret Method<Obj, Ret, Args...>::operator()(Args ...args) const
	{
		return Call(args...);
	}

	template<typename Obj, typename Ret, typename ...Args>
	inline void Method<Obj, Ret, Args...>::SwapObj(Obj* pObj)
	{
		mpObj = pObj;
	}

	template<typename Obj, typename Ret, typename ...Args>
	inline void Method<Obj, Ret, Args...>::operator = (Obj* pObj)
	{
		SwapObj(pObj);
	}
}