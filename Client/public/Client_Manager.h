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


enum OBJ_TYPE { OBJ_UI, OBJ_GAMEOBJCET, OBJ_END };

class CClient_Manager  
{


private:
	CClient_Manager() = default;


public:
	static void	Client_Manager_Update();	// ��ü���� �������

public: /* For_UI */
	static void distance_Limit_UP(CTransform* pTransform, _float4& vTransformFunction, OBJ_TYPE eType);


public:
	static _double	 TimeDelta;

	// ����Ʈ������, ��ǥ����


};

