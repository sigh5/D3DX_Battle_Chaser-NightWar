#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)


END


BEGIN(Client)

class CHpBar final : public CUI
{
protected:
	CHpBar(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHpBar(const CHpBar& rhs);
	virtual ~CHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public: /* imgui */
	virtual void Imgui_RenderProperty()override {}

	void	UI_Event(int a, int b);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;


};

END