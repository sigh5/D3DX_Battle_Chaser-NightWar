#pragma once
#include "Client_Defines.h"
#include "Base.h"
/* 
	����Ƽ(����� ������Ʈ��� �����ϸ��)
	�Լ��� ���� �ָ��� �͵� ��ü���� ���� ����
*/
BEGIN(Engine)
class CTransform;

END


enum DIR { DIR_RIGHT, DIR_UP , DIR_LOOK, DIR_END};

class CClient_Manager  
{


public:
	CClient_Manager() = default;


public:
	static void	Client_Manager_Update();	// ��ü���� �������

public: /* For_UI */
	void distance_Limit_UP(CTransform* pTransform, _float4& vTransformFunction, DIR eType);


public:
	static _double	 TimeDelta;

	// ����Ʈ������, ��ǥ����


};

