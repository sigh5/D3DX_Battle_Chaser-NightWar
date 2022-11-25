#pragma once
#include "VIBuffer.h"

// �޽��� �ﰢ������ ����
// ���� ��ü������ �������̴�.
// �̸޽ø� �׷��������� ����, �ε��� ���۸� �����Ͽ����Ѵ�.

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
	// �� �޽��� m_iMaterialIndex��° ���׸����� ����Ѵ�.
	_uint			m_iMaterialIndex = 0;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMesh* pAIMesh);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();


};

END
