#pragma once
#include "sgm_types.h"
namespace sgm_util
{
	//������������ census���߼�
	// census�任

	/**
	 * \brief census�任
	 * \param source	���룬Ӱ������
	 * \param census	�����censusֵ����
	 * \param width		���룬Ӱ���
	 * \param height	���룬Ӱ���
	 */
	void census_transform_5x5(const uint8* source, uint32* census, const uint32& width, const uint32& height);
	void census_transform_9x7(const uint8* source, uint64* census, const uint32& width, const uint32& height);
	// Hamming����
	uint16 Hamming32(const uint32& x, const uint32& y);

}