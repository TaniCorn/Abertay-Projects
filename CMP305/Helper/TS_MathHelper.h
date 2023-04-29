#pragma once
#ifndef TS_MATHS_H
#define TS_MATHS_H

namespace TS_MATHS {


	static float clamp(float num, float low = 0, float hi = 1) {
		if (num < low)
			return low;
		else if (num > hi)
			return hi;
		else
			return num;
	}

	namespace TS_MATHS_DirectX {

		#include <DirectXMath.h>
		static float Magnitude(const DirectX::XMFLOAT3 v1)
		{
			return (v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z);
		}
		static float Magnitude(const DirectX::XMFLOAT2 v1)
		{
			return (v1.x * v1.x) + (v1.y * v1.y);
		}

		static float MagnitudeSqrRoot(const DirectX::XMFLOAT3 v1)
		{
			return sqrtf(Magnitude(v1));
		}
		static float MagnitudeSqrRoot(const DirectX::XMFLOAT2 v1)
		{
			return sqrtf(Magnitude(v1));
		}

		static DirectX::XMFLOAT3 Normalise(const DirectX::XMFLOAT3 v1) {
			float length = MagnitudeSqrRoot(v1);
			DirectX::XMFLOAT3 normalisedVector;
			normalisedVector.x = (v1.x / length);
			normalisedVector.y = (v1.y / length);
			normalisedVector.z = (v1.z / length);
			return normalisedVector;
		}
		static DirectX::XMFLOAT2 Normalise(const DirectX::XMFLOAT2 v1) {
			float length = MagnitudeSqrRoot(v1);
			DirectX::XMFLOAT2 normalisedVector;
			normalisedVector.x = (v1.x / length);
			normalisedVector.y = (v1.y / length);
			return normalisedVector;
		}

		static DirectX::XMFLOAT3 CrossProduct(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2) {
			DirectX::XMFLOAT3 result;
			result.x = (v1.y * v2.z - v1.z * v2.y);
			result.y = (v1.z * v2.x - v1.x * v2.z);
			result.z = (v1.x *v2.y - v1.y * v2.x);
			return result;
		}		


		static float DotProduct(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
		{
			return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
		}		
		static float DotProduct(const DirectX::XMFLOAT2 v1, const DirectX::XMFLOAT2 v2)
		{
			return (v1.x * v2.x) + (v1.y * v2.y);
		}
	}

}
#endif // !TS_MATHS_H
