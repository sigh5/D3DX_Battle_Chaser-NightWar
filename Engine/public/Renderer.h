#pragma once

#include "Component.h"

/* ȭ�鿡 �׷����� ��ü���� �׸��� ������� ���������Ѵ�. */
/* �����ϰ� �ִ� ������� ��ü���� ��ο���(�����Լ��� ȣ���Ѵ�)�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY,RENDER_SHADOWDEPTH, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI,RENDER_INVENTORY, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void	Set_DIstance(_float Temp){ fDistance = Temp; }

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_DebugRenderGroup(class CComponent* pComponent);
	HRESULT Draw_RenderGroup();
	
private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECTS;

private:
	list<class CComponent*>				m_DebugObject;
	typedef list<class CComponent*>		DEBUGOBJECTS;


private:
	class CTarget_Manager*				m_pTarget_Manager = nullptr;
	class CLight_Manager*				m_pLight_Manager = nullptr;
	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	class CShader*						m_pShader = nullptr;
	_float4x4							m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_uint								m_iShadowMapCX = 3000;
	_uint								m_iShadowMapCY = 2000 ;

	/*ForTool*/
	_float fDistance =					 1.5f;


	
private:
	HRESULT Render_Priority();
	HRESULT Render_ShadowDepth();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();
	HRESULT RENDER_Inventory();

private:
#ifdef _DEBUG
	HRESULT Render_DebugObject();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRenderer* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END