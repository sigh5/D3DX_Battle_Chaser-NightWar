#pragma once

namespace Engine
{
	typedef		signed char				_byte;
	typedef		unsigned char			_ubyte;

	typedef		wchar_t					_tchar;

	typedef		signed short			_short;
	typedef		unsigned short			_ushort;

	typedef		signed int				_int;
	typedef		unsigned int			_uint;

	typedef		signed long				_long;
	typedef		unsigned long			_ulong;

	typedef		float					_float;
	typedef		double					_double;

	typedef		bool					_bool;

	typedef		XMFLOAT2				_float2;
	typedef		XMFLOAT3				_float3;
	typedef		XMFLOAT4				_float4;	// 데이터 전체를 묶음
	typedef		XMVECTOR				_vector;	// 계산 용 지역변수로 사용권장
	typedef		FXMVECTOR				_fvector;
	typedef		CXMVECTOR				_cvector;
	typedef		HXMVECTOR				_hvector;
	typedef		GXMVECTOR				_gvector;

	typedef		XMFLOAT4X4				_float4x4;
	typedef		XMMATRIX				_matrix;
}