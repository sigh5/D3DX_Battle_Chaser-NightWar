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
	LIGHTDESC*	Get_LightDesc(wstring NameTag);

	void				Set_LightView_Matrirx(wstring nameTag,_float4x4 fMatrix);
	void				Set_LightProj_Matrirx(wstring nameTag, _float4x4 fMatrix);

	_float4x4			Get_Light_Matrix( wstring nameTag);
	_float4x4			Get_Light_ProjMatrix(wstring nameTag);

	_float4x4			Test1();
	_float4x4			Test2();
public:
	//HRESULT		Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext *pContext, const LIGHTDESC& LightDesc);
	void		Render_Light(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	void		Clear();
	HRESULT		Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext *pContext, wstring NameTag, const LIGHTDESC& LightDesc);
	HRESULT		Delete_Light(wstring NameTag);

private:
	class	CLight*		Find_Light(wstring nameTag);

	_float4x4  m_LightViewMatrix;
	_float4x4  m_LightProjMatrix;

private:
	/*vector<class CLight*>	m_Lights;
	using LIGHTS = vector<class CLight*>;*/

	map<const wstring, class CLight*> m_Lights;



public:
	virtual		void		Free()override;

};

END