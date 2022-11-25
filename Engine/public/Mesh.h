#pragma once
#include "VIBuffer.h"

// 메쉬는 삼각형들의 집합
// 모델의 교체가능한 파츠들이다.
// 이메시를 그려내기위한 정점, 인덱스 버퍼를 보유하여야한다.

BEGIN(Engine)


class CMesh final: public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint	Get_MaterialIndex()const { return m_iMaterialIndex; }

public:
	virtual HRESULT Initialize_Prototype(aiMesh * pAIMesh);
	virtual HRESULT Initialize(void* pArg);
	virtual	void	Final_Update()override;




private:
	// 이 메쉬는 m_iMaterialIndex번째 머테리얼을 사용한다.
	_uint			m_iMaterialIndex = 0;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMesh* pAIMesh);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();


};

END
