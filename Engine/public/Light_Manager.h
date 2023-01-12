#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final :public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC*	Get_LightDesc(_uint iIndex) const;

public:
	HRESULT		Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext *pContext, const LIGHTDESC& LightDesc);
	void		Render_Light(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	void		Clear();


private:
	vector<class CLight*>	m_Lights;
	using LIGHTS = vector<class CLight*>;

public:
	virtual		void		Free()override;

};

END