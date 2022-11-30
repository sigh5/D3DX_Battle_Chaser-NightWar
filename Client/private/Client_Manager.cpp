#include "stdafx.h"
#include "..\public\Client_Manager.h"
#include "GameInstance.h"


_double CClient_Manager::TimeDelta = 0;

void CClient_Manager::Client_Manager_Update()
{
	

}

void CClient_Manager::distance_Limit_UP(CTransform * pTransform, _float4& vLimitFunction, OBJ_TYPE eType)
{
	if (eType == OBJ_UI)
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, pTransform->Get_State(CTransform::STATE_TRANSLATION));

		if (vPosition.y >= vLimitFunction.y)
			return;
		
		vPosition.y +=  TimeDelta* 50.0;

		if (vPosition.y >= 350.f)
		{
			vPosition.y = 50.f;
			vLimitFunction = vPosition;
		}
		
		pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPosition));

	}
	
	else
	{

	}
	


	
}





