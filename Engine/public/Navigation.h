#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int		iCurrentIndex = -1;		// 객체마다 다 하나씩 있거든 
								
	}NAVIDESC;

public:
	void	Set_SaveSort_NavigatorVector(vector<_float3>* SortVec);

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_bool isMove_OnNavigation(_fvector TargetPos);

public:	/*For.Tool*/
	void	AddCell(_float3* vPoints);
	void	Delete_Navi();


#ifdef _DEBUG
public:
	HRESULT Render(_uint iShaderPass=0);
#endif // 

private:
	vector<class CCell*>	m_Cells;
	NAVIDESC				m_NaviDesc;

private:
	HRESULT	Ready_Neighbor();


#ifdef _DEBUG
private:
	class CShader*			m_pShader = nullptr;
#endif // _DEBUG



public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END