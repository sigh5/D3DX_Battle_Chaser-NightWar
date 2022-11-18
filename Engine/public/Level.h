#pragma once

#include "Base.h"

/* Ŭ���̾�Ʈ�� ������ �������� �θ� �Ǵ� Ŭ�����̴�.  */
/* Ŭ���̾�Ʈ�� ������ �������� �� �����ȿ��� ���Ǵ� ��ü���� �����Ѵ�.etc */
/* ��ü���� �� �ַ� �ʿ��� ��ġ��ü���� �����Ѵ�. */


BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();


protected:
	void						ClearBroadCaster();
	virtual	 void				Add_BroadCaster(const wstring& pBroadCasterTag, class CGameObject* pBroadCaster);
	virtual	 class CGameObject*	Get_BroadCaster(const wstring& pBroadCasterTag);

private:
	class CGameObject*			Find_BroadCaster(const wstring& pBroadCasterTag);


protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	map<const wstring, class CGameObject*> m_mapBroadCasters;
public:
	virtual void Free();
};

END