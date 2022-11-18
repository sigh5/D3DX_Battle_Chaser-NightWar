#pragma once

#include "Base.h"

BEGIN(Engine)

template <typename... Args>
class BaseDelegater
{
	using CallBack = std::function<void(Args ...)>;

public:
	~BaseDelegater()
	{
		for (auto &Event : m_vecBinds)
			Safe_Release(Event.first);

		m_vecBinds.clear();
		m_vecBinds.shrink_to_fit();
	}

	void broadcast(Args& ...args)	// 관찰 대상이 정보 전달하면 등록된 함수 실행
	{
		for (auto iter = m_vecBinds.begin(); iter != m_vecBinds.end();)
		{
			if (iter->first->AddRef() == 0)
			{
				Safe_Release(iter->first);
				iter = m_vecBinds.erase(iter);
			}
			else
			{
				iter->second(args...);
				++iter;
			}
		}
	}

	template <typename T>		// 옵저버들의 실행 함수 등록
	void bind(T* obj, void (T::*memFunc)(Args ...))
	{
		if (nullptr == dynamic_cast<CBase*>(obj))
			return;

		CallBack callback = [obj, memFunc](Args ... args)
		{
			(obj->*memFunc)(args...);
		};

		Safe_AddRef(obj);
		m_vecBinds.emplace_back(std::make_pair(obj, callback));
	}

	template <typename T>
	void unbind(T* obj)
	{
		auto removedIter = remove_if(m_vecBinds.begin(), m_vecBinds.end(), [&obj](std::pair<void*, CallBack> e)
		{
			return e.first == obj;
		});
		m_vecBinds.erase(removedIter, m_vecBinds.end());
	}


private:
	std::vector<std::pair<CBase*, CallBack>> m_vecBinds;

};


END