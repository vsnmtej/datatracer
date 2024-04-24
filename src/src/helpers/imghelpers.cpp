#include "imghelpers.h"

int convertGrayScale(Mat &img, Mat &grayscale){
	cvtColor(img, grayscale, COLOR_BGR2GRAY);
	return 0;
}

double calcSharpness(Mat &img){
    // convert the image in to grayscale
    Mat grayscale;
    cvtColor(img, grayscale, COLOR_BGR2GRAY);

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
    cvtColor(img, grayscale, COLOR_BGR2GRAY);

    Scalar mean, sigma;
    meanStdDev(grayscale, mean, sigma);
    
    // Calculate the signal to noise ration using the following formula
    double snr = 10 * log10(mean.val[0] * mean.val[0] / sigma.val[0] * sigma.val[0]);
    return snr;    
}

int calcRGBMean(Mat &img, std::vector<double> &rgbMean){
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

double calcContrast(Mat &img){
    Mat grayscale;
    cvtColor(img, grayscale, COLOR_BGR2GRAY);
    double min_pixel_value, max_pixel_value;
    minMaxLoc(grayscale, &min_pixel_value, &max_pixel_value);
    return (max_pixel_value - min_pixel_value)/ max_pixel_value;  
}

double caclBrightness(Mat &img){
	std::vector<double> mean;
	calcRGBMean(img, mean);
	return mean[0] * 0.299 + mean[1] * 0.587 + mean[2] * 0.114;
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

