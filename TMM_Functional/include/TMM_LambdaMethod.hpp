#include "TMM_LambdaMethod.h"

namespace TMM
{
	template<typename Ret, typename ...Args>
	template<typename F>
	inline LambdaMethod<Ret, Args...>::LambdaMethod(F&& f)
	{
		using Fun = TMM::Decay_t<F>;
		mpObj = new Fun(TMM::forward<F>(f));
		mCall = [](void* obj, Args... args) -> Ret {
			return (*static_cast<Fun*>(obj))(args...);
			};
		mDestroy = [](void* obj) -> void {
			delete static_cast<Fun*>(obj);
			};
	}

	template<typename Ret, typename ...Args>
	inline LambdaMethod<Ret, Args...>::~LambdaMethod()
	{
		if (mDestroy != nullptr)
		{
			mDestroy(mpObj);
		}
	}

	template<typename Ret, typename ...Args>
	inline Ret LambdaMethod<Ret, Args...>::Call(Args ...args) const
	{
		return mCall(mpObj, args...);
	}

	template<typename Ret, typename ...Args>
	inline Ret LambdaMethod<Ret, Args...>::operator()(Args ...args) const
	{
		return mCall(mpObj, args...);
	}


	template<typename F>
	auto MakeLambdaMethodPtr(F&& f)
	{
		using Decayed = TMM::Decay_t<F>;
		using Traits = LambdaTraits<decltype(&Decayed::operator())>;
		return new typename Traits::Method(TMM::forward<F>(f));
	}

	template<typename F>
	auto MakeLambdaMethod(F&& f)
	{
		using Decayed = TMM::Decay_t<F>;
		using Traits = LambdaTraits<decltype(&Decayed::operator())>;
		return typename Traits::Method(TMM::forward<F>(f));
	}

}