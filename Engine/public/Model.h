#pragma once
#include "Component.h"

BEGIN(Engine)

class CModel :public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	const  aiScene*						m_pAIScene = nullptr;
	Assimp::Importer					m_Importer;
	TYPE								m_eType = TYPE_END;

	vector<class CMesh*>				m_MeshContainers;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END