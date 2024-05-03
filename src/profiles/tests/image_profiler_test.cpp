#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "helpers/imageprocessing.h" // Replace with your class header file

class ImageProcessingTest : public ::testing::Test {
protected:
    cv::Mat colorImage;
    cv::Mat grayscaleImage;
    std::string testImagePath = "test_images";
    std::string testImageBaseName = "test_image";

    void SetUp() override {
        colorImage = cv::Mat(100, 100, CV_8UC3, cv::Scalar(100, 150, 200)); // A simple 100x100 image
        grayscaleImage = cv::Mat(100, 100, CV_8UC1, cv::Scalar(127)); // A grayscale image
    }
};

// Test convertGrayScale function
TEST_F(ImageProcessingTest, ConvertGrayScale) {
    cv::Mat grayscaleResult;
    int result = convertGrayScale(colorImage, grayscaleResult);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(grayscaleResult.channels(), 1); // Grayscale should have one channel
}

// Test calcSharpness function
TEST_F(ImageProcessingTest, CalcSharpness) {
    double sharpness = calcSharpness(grayscaleImage);
    EXPECT_GT(sharpness, 0); // Sharpness should be greater than zero
}

// Test calcSNR function
TEST_F(ImageProcessingTest, CalcSNR) {
    double snr = calcSNR(grayscaleImage);
    EXPECT_GT(snr, 0); // SNR should be greater than zero
}

// Test calcRGBMean function
TEST_F(ImageProcessingTest, CalcRGBMean) {
    std::vector<double> rgbMean(3);
    int result = calcRGBMean(colorImage, rgbMean);
    ASSERT_EQ(result, 0);
    EXPECT_NEAR(rgbMean[0], 100, 1.0); // Expected blue component
    EXPECT_NEAR(rgbMean[1], 150, 1.0); // Expected green component
    EXPECT_NEAR(rgbMean[2], 200, 1.0); // Expected red component
}

// Test calcContrast function
TEST_F(ImageProcessingTest, CalcContrast) {
    double contrast = calcContrast(grayscaleImage);
    EXPECT_GE(contrast, 0); // Contrast should not be negative
}

// Test calcBrightness function
TEST_F(ImageProcessingTest, CalcBrightness) {
    double brightness = caclBrightness(colorImage);
    EXPECT_GT(brightness, 0); // Brightness should be positive
}

// Test saveImageWithIncrementalName function
TEST_F(ImageProcessingTest, SaveImageWithIncrementalName) {
    std::string savedImagePath = saveImageWithIncrementalName(colorImage, testImagePath, testImageBaseName);
    ASSERT_FALSE(savedImagePath.empty()); // Path should not be empty
    EXPECT_TRUE(cv::imread(savedImagePath).data != nullptr); // The image should be readable
}

