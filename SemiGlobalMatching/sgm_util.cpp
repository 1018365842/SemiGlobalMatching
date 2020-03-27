#include "stdafx.h"
#include "sgm_util.h"
#include <algorithm>

void sgm_util::census_transform_5x5(const uint8* source, uint32* census, const uint32& width,
	const uint32& height)
{
	if (source == nullptr || census == nullptr || width <= 5u || height <= 5u) {
		return;
	}

	// �����ؼ���censusֵ
	for (uint32 i = 2u; i < height - 2; i++) {
		for (uint32 j = 2u; j < width - 2; j++) {
			
			// ��������ֵ
			const uint8 gray_center = source[i * width + j];
			
			// ������СΪ5x5�Ĵ������������أ���һ�Ƚ�����ֵ����������ֵ�ĵĴ�С������censusֵ
			uint32 census_val = 0u;
			for (sint32 r = -2; r <= 2; r++) {
				for (sint32 c = -2; c <= 2; c++) {
					census_val <<= 1;
					const uint8 gray = source[(i + r) * width + j + c];
					if (gray < gray_center) {
						census_val += 1;
					}
				}
			}

			// �������ص�censusֵ
			census[i * width + j] = census_val;		
		}
	}
}

void sgm_util::census_transform_9x7(const uint8* source, uint64* census, const uint32& width, const uint32& height)
{
}

uint16 sgm_util::Hamming32(const uint32& x, const uint32& y)
{
	uint32 dist = 0, val = x ^ y;

	// Count the number of set bits
	while (val) {
		++dist;
		val &= val - 1;
	}

	return dist;
}
