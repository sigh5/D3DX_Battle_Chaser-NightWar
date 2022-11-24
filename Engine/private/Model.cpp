#include "..\public\Model.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
{
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath)
{
	_uint			iFlag = 0;

	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
		//		미리 떨어져있는 뼈들을 붙이는 플래그	// 왼손좌표계로변환				// 조금의 최적화
	else 
		iFlag  = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
		// aiProcess_PreTransformVertices 안쓰는 이유 == 내가 직접 조정할려고

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	m_pAIScene->HasAnimations(); // 애니메이션 있는지 확인하는 함수


	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	m_Importer.FreeScene(); // 해제 코드
}
