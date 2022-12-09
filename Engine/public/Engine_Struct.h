#pragma once
namespace Engine
{
	enum aiTextureType
	{
		aiTextureType_NONE = 0,
		aiTextureType_DIFFUSE = 1,
		aiTextureType_SPECULAR = 2,
		aiTextureType_AMBIENT = 3,
		aiTextureType_EMISSIVE = 4,
		aiTextureType_HEIGHT = 5,
		aiTextureType_NORMALS = 6,
		aiTextureType_SHININESS = 7,
		aiTextureType_OPACITY = 8,
		aiTextureType_DISPLACEMENT = 9,
		aiTextureType_LIGHTMAP = 10,
		aiTextureType_REFLECTION = 11,
		aiTextureType_BASE_COLOR = 12,
		aiTextureType_NORMAL_CAMERA = 13,
		aiTextureType_EMISSION_COLOR = 14,
		aiTextureType_METALNESS = 15,
		aiTextureType_DIFFUSE_ROUGHNESS = 16,
		aiTextureType_AMBIENT_OCCLUSION = 17,
		aiTextureType_UNKNOWN = 18,
	};

#define AI_TEXTURE_TYPE_MAX  aiTextureType_UNKNOWN

	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWindowMode;
		unsigned int	iViewportSizeX;
		unsigned int	iViewportSizeY;
		HWND			hWnd;		
	}GRAPHIC_DESC;


	typedef struct tagModelMaterial
	{
		class CTexture*		pTexture[AI_TEXTURE_TYPE_MAX]; // texture 도 diffuse , normal .. 등등등  어싱크가 필요한 타입이 많다.
	}MODELMATERIAL;

	typedef struct tagLightDesc
	{
		enum TYPE {TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END};
		TYPE		eType;

		bool		isEnable;

		XMFLOAT4	vDirection;

		// 모든 정점이 받는 빛의 방향벡터가 다 다르게 표현되야하기때문이다.
		// 셰이더내에서 빛의 방향벡터를 구해서 연산을 해야한다.
		XMFLOAT4	vPosition;
		float		fRange;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHTDESC;


	typedef struct tagKeyframe
	{
		double		Time;
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;
	}KEYFRAME;



	typedef struct tagFaceIndices16
	{
		unsigned short			_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long			_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertexPositionTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexUV;
	}VTXTEX;


	typedef struct tagVertexCubePositionTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexUV;
	}VTXCUBETEX;


	typedef struct ENGINE_DLL tagVertexPositionTexture_Declaration
	{
		static const unsigned int		iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];			
	}VTXTEX_DECLARATION;

	typedef struct tagVertexPositionNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexPositionNormalTexture_Declaration
	{
		static const unsigned int		iNumElements = 3;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX_DECLARATION;


	typedef struct tagVertexModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int		iNumElements = 4;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex; /* 이 정점이 사용해야하는 뼈의 인덱스(전체뼈 중의 순서(x), 이 정점이 소속된 메시에 영향을 주는 뼈중의 순서(o))  */
		XMFLOAT4		vBlendWeight;
	}VTXANIMMODEL;


	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int		iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;


	
	typedef struct Tag_HighLightUIDesc
	{
		float		 iNumSizeX;
		float		 iNumSizeY;
		float		 yPos;
		unsigned int iTextureIndex;
		wchar_t		 szObjectName[MAX_PATH];

	}HIGHLIGHT_UIDESC;




}