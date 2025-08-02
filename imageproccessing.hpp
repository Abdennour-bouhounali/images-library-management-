#ifndef IMAGEPROCCESSING_HPP
#define IMAGEPROCCESSING_HPP
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std; 

class ImageProccessing
{
public:
    ImageProccessing();

    Mat calculateHistogram(const Mat& inputImage);
    Mat applyGaussianFilter(const Mat& inputImage);
    Mat toGrayScale(const cv::Mat& inputImage) ;
    Mat applyCustomMedianFilter(const cv::Mat& inputImage, int kernelSize);
    Mat applyEdgeDetection(const Mat& inputImage);
    Mat applyThreshold(const Mat& inputImage, int thresholdValue);
    Mat rotateImage(const Mat& inputImage, int angle);
    Mat applySIFT(const Mat& inputImage);
    Mat applyErosion(const Mat& inputImage, int kernelSize) ;
};

#endif // IMAGEPROCCESSING_HPP
