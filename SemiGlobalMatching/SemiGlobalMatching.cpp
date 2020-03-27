/* -*-c++-*- SemiGlobalMatching - Copyright (C) 2020.
* Author	: Ethan Li <ethan.li.whu@gmail.com>
* Describe	: implement of semi-global matching class
*/

#include "stdafx.h"
#include "SemiGlobalMatching.h"
#include "sgm_util.h"
#include <algorithm>

SemiGlobalMatching::SemiGlobalMatching(): width_(0), height_(0), img_left_(nullptr), img_right_(nullptr),
                                          census_left_(nullptr),
                                          census_right_(nullptr),
                                          cost_init_(nullptr),
                                          cost_aggr_(nullptr),
                                          disp_left_(nullptr), is_initialized_(false)
{
}


SemiGlobalMatching::~SemiGlobalMatching()
{
    Release();
    is_initialized_ = false;
}

bool SemiGlobalMatching::Initialize(const uint32& width, const uint32& height, const SGMOption& option)
{
    // ������ ��ֵ
    
	// Ӱ��ߴ�
    width_ = width;
    height_ = height;
    // SGM����
    option_ = option;

    if(width == 0 || height == 0) {
        return false;
    }

    //������ �����ڴ�ռ�

    // censusֵ������Ӱ��
    census_left_ = new uint32[width * height]();
    census_right_ = new uint32[width * height]();

    // ƥ����ۣ���ʼ/�ۺϣ�
    const sint32 disp_range = option.max_disparity - option.min_disparity;
    if (disp_range <= 0) {
        return false;
    }
    cost_init_ = new uint16[width * height * disp_range]();
    cost_aggr_ = new uint16[width * height * disp_range]();

    // �Ӳ�ͼ
    disp_left_ = new float32[width * height]();

    is_initialized_ = census_left_ && census_right_ && cost_init_ && cost_aggr_ && disp_left_;

    return is_initialized_;
}


void SemiGlobalMatching::Release()
{
    // �ͷ��ڴ�
    if (census_left_ != nullptr) {
        delete[] census_left_;
        census_left_ = nullptr;
    }
    if (census_right_ != nullptr) {
        delete[] census_right_;
        census_right_ = nullptr;
    }
    if (cost_init_ != nullptr) {
        delete[] cost_init_;
        cost_init_ = nullptr;
    }
    if (cost_aggr_ != nullptr) {
        delete[] cost_aggr_;
        cost_aggr_ = nullptr;
    }
    if (disp_left_ != nullptr) {
        delete[] disp_left_;
        disp_left_ = nullptr;
    }
}


bool SemiGlobalMatching::Match(const uint8* img_left, const uint8* img_right, float32* disp_left)
{
    if(!is_initialized_) {
        return false;
    }
    if (img_left == nullptr || img_right == nullptr) {
        return false;
    }

    img_left_ = img_left;
    img_right_ = img_right;

    // census�任
    CensusTransform();

    // ���ۼ���
    ComputeCost();

    // ���۾ۺ�
    CostAggregation();

    // �Ӳ����
    ComputeDisparity();

    // ����Ӳ�ͼ
    memcpy(disp_left, disp_left_, width_ * height_ * sizeof(float32));

	return true;
}

bool SemiGlobalMatching::Reset(const uint32& width, const uint32& height, const SGMOption& option)
{
    // �ͷ��ڴ�
    Release();

    // ���ó�ʼ�����
    is_initialized_ = false;

    // ��ʼ��
    return Initialize(width, height, option);
}

void SemiGlobalMatching::CensusTransform() const
{
	// ����Ӱ��census�任
    sgm_util::census_transform_5x5(img_left_, census_left_, width_, height_);
    sgm_util::census_transform_5x5(img_right_, census_right_, width_, height_);
}

void SemiGlobalMatching::ComputeCost() const
{
    const sint32& min_disparity = option_.min_disparity;
    const sint32& max_disparity = option_.max_disparity;
    
	// ������ۣ�����Hamming���룩
    for (uint32 i = 0u; i < height_; i++) {
        for (uint32 j = 0u; j < width_; j++) {

            // ��Ӱ��censusֵ
            const uint32 census_val_l = census_left_[i * width_ + j];

            // ���Ӳ�������ֵ
        	for (sint32 d = min_disparity; d < max_disparity; d++) {
                if (j - d < 0 || j - d >= width_) {
                    continue;
                }
                // ��Ӱ���Ӧ����censusֵ
                const uint32 census_val_r = census_right_[i * width_ + j - d];
                
        		// ����ƥ�����
        		cost_init_[(d - min_disparity) * width_ * height_ + i * width_ + j] = sgm_util::Hamming32(census_val_l, census_val_r);
            }
        }
    }
}

void SemiGlobalMatching::CostAggregation() const
{
}

void SemiGlobalMatching::ComputeDisparity() const
{
    const sint32& min_disparity = option_.min_disparity;
    const sint32& max_disparity = option_.max_disparity;

    // δʵ�־ۺϲ��裬���ó�ʼ����ֵ������
    uint16* cost_ptr = cost_init_;

    // �����ؼ��������Ӳ�
    for (uint32 i = 0u; i < height_; i++) {
        for (uint32 j = 0u; j < width_; j++) {
            
            uint16 min_cost = UINT16_MAX;
            uint16 max_cost = 0;
            sint32 best_disparity = 0;

            // �����ӲΧ�ڵ����д���ֵ�������С����ֵ����Ӧ���Ӳ�ֵ
            for (sint32 d = min_disparity; d < max_disparity; d++) {
                const uint16& cost = cost_ptr[(d - min_disparity) * width_ * height_ + i * width_ + j];
                if(min_cost > cost) {
                    min_cost = cost;
                    best_disparity = d;
                }
                max_cost = std::max(max_cost, cost);
            }

            // ��С����ֵ��Ӧ���Ӳ�ֵ��Ϊ���ص������Ӳ�
            if (max_cost != min_cost) {
                disp_left_[i * width_ + j] = static_cast<float>(best_disparity);
            }
            else {
                disp_left_[i * width_ + j] = Invalid_Float;
            }
        }
    }
}
