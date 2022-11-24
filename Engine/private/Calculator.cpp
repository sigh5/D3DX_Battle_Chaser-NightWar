#include "..\public\Calculator.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{

}

HRESULT CCalculator::Ready_Calculator()
{
	return S_OK;
}

_vector CCalculator::MovePicking_Point(CGameObject * pMoveObject, CGameObject * pTerrainObject)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	//if (PickingCube(pGameInstance->GetHWND(), ))



	RELEASE_INSTANCE(CGameInstance);

	return _vector();
}



void CCalculator::Free()
{
}
