#pragma once

#include "Component.h"

/* ȭ�鿡 �׷����� ��ü���� �׸��� ������� ���������Ѵ�. */
/* �����ϰ� �ִ� ������� ��ü���� ��ο���(�����Լ��� ȣ���Ѵ�)�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual		void	Final_Update()override {}
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECTS;

private:
	class CTarget_Manager*				m_pTarget_Manager = nullptr;

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRenderer* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END