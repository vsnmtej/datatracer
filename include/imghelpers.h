#ifndef IMAGE_ANALYSIS_H
#define IMAGE_ANALYSIS_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <cstring>

using namespace cv;

// Function to convert an image to grayscale
Mat convertGrayScale(Mat &img);

// Function to calculate the sharpness of an image using the Laplacian operator
double calcSharpness(Mat &img);

// Function to calculate the signal-to-noise ratio (SNR) of an image
double calcSNR(Mat &img);

// Function to calculate the mean RGB values of an image
int calcRGBMean(Mat &img, std::vector<double> &rgbMean);

// Function to calculate the contrast of an image
double calcContrast(Mat &img);

// Function to calculate the brightness of an image
double calcBrightness(Mat &img);

// saveImagewithIncremental
std::string saveImageWithIncrementalName(const cv::Mat& image, const std::string& path, const std::string& baseName);

#endif
