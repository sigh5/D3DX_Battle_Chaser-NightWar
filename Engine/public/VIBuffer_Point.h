#pragma once
#include "VIBuffer.h"


BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Point : public CVIBuffer
{
private:
	CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point(const CVIBuffer_Point& rhs);
	virtual ~CVIBuffer_Point() = default;

public:
	void	Set_ShaderPass(_uint iShaderPass) { m_iShaderPass = iShaderPass; }
	void	Set_FrameCnt(_uint iFrameCnt) { m_iFrameCnt = iFrameCnt; }
	void	Set_TextureMax_Width_Cnt(_uint iMaxWidth_Cnt) { m_iTextureMax_Width_Cnt = iMaxWidth_Cnt; }
	void	Set_TextureMax_Height_Cnt(_uint iMaxHeight_Cnt) { m_iTextureMax_Height_Cnt = iMaxHeight_Cnt; }
	void	Set_Width_Ratio(_float fWidthRatio) { m_fWidthRatio = fWidthRatio; }
	void	Set_Height_Ratio(_float fHeightRatio) { m_fHeightRatio = fHeightRatio; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:/*For.Imgui*/
	virtual		void	Imgui_RenderProperty()override;

public:
	void				UV_Move_Tick(_double TimeDelta);
	HRESULT				Set_UV_RawValue(class CShader* pShader);

private:
	_uint					m_iPlayOnFrameCnt = 0;		//속도 조절 m_iPlayOnFrameCnt ==5 이면  5/60 틱에 한번 도는거임 

	_uint					m_iWidthTextureCnt = 0;		// 계속 증가해야됌
	_uint					m_iHeightTextureCnt = 0;	// 계속 증가해야됌
	_uint					m_iFrameCnt = 0;
	_uint					m_iTextureMax_Width_Cnt = 0;
	_uint					m_iTextureMax_Height_Cnt = 0;

	_float					m_fWidthRatio = 0.f;
	_float					m_fHeightRatio = 0.f;

	_float					m_fCurX = 0.25f;
	_float					m_fCurY = 0.25f;
	_float					m_fOldX = 0.75f;
	_float					m_fOldY = 0.5f;

	_bool					m_bEdit = false;

	_uint					m_iShaderPass = 0;		// 0 :가로로 그린다 ,1: 세로로 그린다 ,2: 가로가 끝나면 세로로 그린다. 3 :

													/*For.Imgui*/
	int					iPlayOnFrameCnt = 0;
	int					iTextureMax_Width_Cnt = 0;
	int					 iTextureMax_Height_Cnt = 0;
	float fWidthRatio = 0.f;
	float fHeightRatio = 0.f;

public:
	static CVIBuffer_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END