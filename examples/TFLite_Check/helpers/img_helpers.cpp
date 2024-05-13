#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

int convertGrayScale(Mat img, Mat grayscale){

	Mat grayscale;
	cvtColor(image, &grayscale, COLOR_BGR2GRAY);
	return 0;
}

double calcSharpness(Mat &image){
   
    // convert the image in to grayscale
    Mat grayscale;
    cvtColor(image, grayscale, COLOR_BGR2GRAY);

    // Apply the Laplacian operator to the grayscale image
    Mat laplacian;
    Laplacian(grayscale, &laplacian, CV_64F);

    // Compute the mean and std of laplacian image
    Scalar mean, sigma;
    meanStdDev(laplacian, &mean, &sigma);

    // Calculate the sharpness of the image using the Laplacian STD
    double sharpness = sigma.val[0] * sigma.val[0];

    return sharpness;	    
}

double calcSNR(Mat image){

    // Convert the image to grayscale
    Mat grayscale;
    cvtColor(image, grayscale, COLOR_BGR2GRAY);

    Scalar mean, sigma;
    meanStdDev(grayscale, mean, sigma);
    
    // Calculate the signal to noise ration using the following formula
    double snr = 10 * log10(mean.val[0] * mean.val[0] / sigma.val[0] * sigma.val[0]);
    return snr;    
}

int calcRGBMean(Mat image, std::vector<double> &rgbMean){

    int height = image.rows;
    int width = image.cols;
    int channels = image.channels();
    
    rgbMean[0] = 0;
    rgbMean[1] = 0;
    rgbMean[2] = 0;

    for (int i = 0; i < height ; i++){
        for (int j = 0; j < width; j++){
	    rgbMean[0] += image.at<Vec3b>(i, j)[0];
	    rgbMean[1] += image.at<Vec3b>(i, j)[1];
	    rgbMean[2] += image.at<Vec3b>(i, j)[2];
	}
    }
    int hw = height*width;
    rgbMean[0] /= hw;
    rgbMean[1] /= hw;
    rgbMean[2]/ = hw;
    return 0;
}

double calcContrast(Mat image){
    Mat grayscale;
    cvtColor(image, grayscale, COLOR_BGR2GRAY);
    double min_pixel_value, max_pixel_value;
    minMaxLoc(grayscale, &min_pixel_value, &max_pixel_value);
    return (max_pixel_value - min_pixel_value)/ max_pixel_value;  
}

double caclBrightness(Mat image){
	std::vector<double> mean;
	calcRGBMean(image, &mean);
	return mean[0] * 0.299 + mean[1] * 0.587 + mean[2] * 0.114;
}

/*
int main() {
  // Read the input image
  Mat image = imread("image.jpg", IMREAD_COLOR);

  // Convert the image to grayscale
  Mat grayscale;
  cvtColor(image, grayscale, COLOR_BGR2GRAY);

  // Apply the Laplacian operator to the grayscale image
  Mat laplacian;
  Laplacian(grayscale, laplacian, CV_64F);

  // Compute the mean and standard deviation of the Laplacian image
  Scalar mean, sigma;
  meanStdDev(laplacian, mean, sigma);

  // Calculate the sharpness of the image using the Laplacian standard deviation
  double sharpness = sigma.val[0] * sigma.val[0];

  // Print the sharpness of the image
  std::cout << "Sharpness of the image: " << sharpness << std::endl;

  return 0;
}
*/
