#include "imghelpers.h"
#include <sys/types.h> // For types like DIR and struct stat
#include <sys/stat.h> // For mkdir and file permissions
#include <dirent.h> // For directory operations
#include <cstring> // For strcmp
#include <string> // For std::string
#include <sstream> // For std::stringstream
#include <algorithm> // For std::max
#include <iostream> // For std::cerr and std::endl
#include <cstdio> // For perror
#include "datatracer_log.h"

int convertGrayScale(Mat &img, Mat &grayscale){
	cvtColor(img, grayscale, COLOR_BGR2GRAY);
	return 0;
}

double calcSharpness(Mat &img){
    // convert the image in to grayscale
    Mat grayscale;
    if (img.channels() == 3) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGRA2GRAY);
    } else {
        // If the image is already grayscale
        grayscale = img;
    }

    // Apply the Laplacian operator to the grayscale image
    Mat laplacian;
    Laplacian(grayscale, laplacian, CV_64F);

    // Compute the mean and std of laplacian image
    Scalar mean, sigma;
    meanStdDev(laplacian, mean, sigma);

    // Calculate the sharpness of the image using the Laplacian STD
    double sharpness = sigma.val[0] * sigma.val[0];

    return sharpness;	    
}

double calcSNR(Mat &img){
    // Convert the image to grayscale
    Mat grayscale;
    if (img.channels() == 3) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGRA2GRAY);
    } else {
        // If the image is already grayscale
        grayscale = img;
    }

    Scalar mean, sigma;
    meanStdDev(grayscale, mean, sigma);
   
    // Calculate the Signal-to-Noise Ratio (SNR)
    double signal = mean[0];
    double noise = sigma[0];

    if (noise == 0) {
        // If noise is zero, return a high SNR (almost infinite)
        return std::numeric_limits<double>::infinity();
    }

    // SNR in decibels (dB)
    double snr = 20 * std::log10(signal / noise);
 
    return snr;    
}

/*
int calcMean(Mat &img, std::vector<double> &rgbMean){
    int height = img.rows;
    int width = img.cols;
    int channels = img.channels();

    rgbMean[0] = 0;
    rgbMean[1] = 0;
    rgbMean[2] = 0;

    for (int i = 0; i < height ; i++){
        for (int j = 0; j < width; j++){
	    rgbMean[0] += img.at<Vec3b>(i, j)[0];
	    rgbMean[1] += img.at<Vec3b>(i, j)[1];
	    rgbMean[2] += img.at<Vec3b>(i, j)[2];
	}
    }
    int hw = height*width;
    rgbMean[0] /= hw;
    rgbMean[1] /= hw;
    rgbMean[2] /= hw;
    return 0;
}

double calcMean(const cv::Mat& img, int channelNumber) {
    if (channelNumber < 0 || channelNumber >= img.channels()) {
        throw std::out_of_range("Channel number out of range");
    }
    // Split the image into separate channels
    std::vector<cv::Mat> channels;
    cv::split(img, channels);
    // Ensure channel number is valid
    if (channelNumber < 0 || channelNumber >= channels.size()) {
        throw std::out_of_range("Invalid channel number");
    }
    // Calculate the mean of the specified channel
    cv::Scalar mean = cv::mean(channels[channelNumber]);
    // Return the mean value for the specified channel
    return mean[0]; // For single-channel images, the mean is in the first element of the scalar
}
*/
double calcContrast(Mat &img){
    Mat grayscale;
    if (img.channels() == 3) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGRA2GRAY);
    } else {
        // If the image is already grayscale
        grayscale = img;
    }
   
    double min_pixel_value, max_pixel_value;
    minMaxLoc(grayscale, &min_pixel_value, &max_pixel_value);
    return (max_pixel_value - min_pixel_value)/ max_pixel_value;  
}

double calcBrightness(Mat &img){
	std::vector<double> mean;
        Scalar mean_values = cv::mean(img);
	if (mean_values.rows > 0){
	    return mean_values[0] * 0.299 + mean_values[1] * 0.587 + mean_values[2] * 0.114;
	}else{
	    return mean_values[0];
        }
}

std::string saveImageWithIncrementalName(const cv::Mat& img, const std::string& path, const std::string& baseName) {
    // Ensure the directory exists
    // Check if the directory already exists
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
		// Attempt to create the directory
		if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
			// Error occurred
			perror("Error creating directory");
			return "";
		}
	}

    // Find the highest numbered image in the directory
    int highestIndex = 0;
	DIR* dir = opendir(path.c_str());
    if (dir) {
        dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				std::string filename(entry->d_name);
				if (filename.find(baseName) == 0) {
					int index = std::stoi(filename.substr(baseName.length()));
					highestIndex = std::max(highestIndex, index);
				}
			}
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        return "";
    }

    // Increment the index for the new image
    int newIndex = highestIndex + 1;

    // Generate the new filename
    std::stringstream ss;
    ss << path << "/" << baseName << std::setfill('0') << std::setw(4) << newIndex << ".png"; // Using four digits
    std::string newFilename = ss.str();

    // Save the image
    cv::imwrite(newFilename, img);

    return newFilename;
}

