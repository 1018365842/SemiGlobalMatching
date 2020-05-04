/* -*-c++-*- SemiGlobalMatching - Copyright (C) 2020.
* Author	: Ethan Li <ethan.li.whu@gmail.com>
* Describe	: main
*/

#include "stdafx.h"
#include "SemiGlobalMatching.h"

// opencv library
#include <opencv2/opencv.hpp>
#ifdef _DEBUG
#pragma comment(lib,"opencv_world310d.lib")
#else
#pragma comment(lib,"opencv_world310.lib")
#endif


/**
 * \brief 
 * \param argv 3
 * \param argc argc[1]:��Ӱ��·�� argc[2]: ��Ӱ��·�� argc[3]: �Ӳ�ͼ·��[optional]
 * \param eg. $(SolutionDir)Data\cone\im2.png $(SolutionDir)Data\cone\im6.png $(SolutionDir)Data\cone\disp.bmp
 * \return 
 */
int main(int argv,char** argc)
{
    if(argv < 3) {
        return 0;
    }

    //��������������������������������������������������������������������������������������������������������������������������������������������������������������//
    // ��ȡӰ��
    std::string path_left = argc[1];
    std::string path_right = argc[2];
   
    cv::Mat img_left = cv::imread(path_left, cv::IMREAD_GRAYSCALE);
    cv::Mat img_right = cv::imread(path_right, cv::IMREAD_GRAYSCALE);

    if (img_left.data == nullptr || img_right.data == nullptr) {
        std::cout << "��ȡӰ��ʧ�ܣ�" << std::endl;
        return -1;
    }
    if (img_left.rows != img_right.rows || img_left.cols != img_right.cols) {
        std::cout << "����Ӱ��ߴ粻һ�£�" << std::endl;
        return -1;
    }


    //��������������������������������������������������������������������������������������������������������������������������������������������������������������//
    const sint32 width = static_cast<uint32>(img_left.cols);
    const sint32 height = static_cast<uint32>(img_right.rows);

	// ����Ӱ��ĻҶ�����
	unsigned char* bytes_left = new unsigned char[width*height];
	unsigned char* bytes_right = new unsigned char[width*height];
	for (int i = 0; i < height;i++) {
		for (int j = 0; j < width;j++) {
			bytes_left[i*width + j] = img_left.at<unsigned char>(i, j);
			bytes_right[i*width + j] = img_right.at<unsigned char>(i, j);
		}
	}


    // SGMƥ��������
    SemiGlobalMatching::SGMOption sgm_option;
    // �ۺ�·����
	sgm_option.num_paths = 8;
	// ��ѡ�ӲΧ
    sgm_option.min_disparity = 0;
    sgm_option.max_disparity = 64;
    // һ���Լ��
    sgm_option.is_check_lr = true;
    sgm_option.lrcheck_thres = 1.0f;
    // Ψһ��Լ��
	sgm_option.is_check_unique = true;
    sgm_option.uniqueness_ratio = 0.99;
    // �޳�С��ͨ��
	sgm_option.is_remove_speckles = true;
    sgm_option.min_speckle_aera = 30;
    // �ͷ���P1��P2
	sgm_option.p1 = 10;
    sgm_option.p2_init = 150;
	// �Ӳ�ͼ���
	sgm_option.is_fill_holes = true;
    
	// ����SGMƥ����ʵ��
    SemiGlobalMatching sgm;

    //��������������������������������������������������������������������������������������������������������������������������������������������������������������//
    // ��ʼ��
    if(!sgm.Initialize(width, height, sgm_option)) {
        std::cout << "SGM��ʼ��ʧ�ܣ�" << std::endl;
        return -2;
    }


    //��������������������������������������������������������������������������������������������������������������������������������������������������������������//
    // ƥ��
    auto disparity = new float32[uint32(width * height)]();
    if(!sgm.Match(bytes_left,bytes_right,disparity)) {
        std::cout << "SGMƥ��ʧ�ܣ�" << std::endl;
        return -2;
    }

	//��������������������������������������������������������������������������������������������������������������������������������������������������������������//
    // ��ʾ�Ӳ�ͼ
    cv::Mat disp_mat = cv::Mat(height, width, CV_8UC1);
    for (sint32 i = 0; i < height; i++) {
        for (sint32 j = 0; j < width; j++) {
            const float32 disp = disparity[i * width + j];
            if (disp == Invalid_Float) {
                disp_mat.data[i * width + j] = 0;
            }
            else {
                disp_mat.data[i * width + j] = static_cast<uchar>(2.5*static_cast<uchar>(disp));
            }
        }
    }

	if (argv >= 4) {
		std::string disp_map_path = argc[3];
		cv::imwrite(disp_map_path, disp_mat);
	}

    cv::imshow("�Ӳ�ͼ", disp_mat);
	cv::Mat disp_color;
	applyColorMap(disp_mat, disp_color, cv::COLORMAP_JET);
	cv::imshow("�Ӳ�ͼ-α��", disp_color);

	cv::waitKey(0);

    //��������������������������������������������������������������������������������������������������������������������������������������������������������������//
    // �ͷ��ڴ�
    delete[] disparity; 
	disparity = nullptr;
	delete[] bytes_left;
	bytes_left = nullptr;
	delete[] bytes_right;
	bytes_right = nullptr;

    system("pause");
	return 0;
}

