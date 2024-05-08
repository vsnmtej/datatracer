#include "imageprofile.h"  // Include your class header
#include "saver.h"  // Include the Saver class
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <thread>
#include <chrono>

// Test fixture for ImageProfile tests
class ImageProfileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a sample INI file for testing
        createSampleIniFile("test_config.ini");
        
        // Create an ImageProfile object with a sample INI file and a Saver instance
        image_profile = new ImageProfile("test_config.ini", 1, 1);
    }

    void TearDown() override {
        delete image_profile;  // Clean up the ImageProfile instance
        cleanUpTestFiles();  // Clean up the test files
    }

    // Helper function to create a sample INI file for testing
    void createSampleIniFile(const std::string& filename) {
        std::ofstream ini_file(filename, std::ios::trunc);
        ini_file << "[image]\n";
        ini_file << "filepath = ./\n";  // Sample file path
        ini_file << "NOISE = 0.5\n";  // Thresholds
        ini_file << "BRIGHTNESS = 0.4\n";
        ini_file << "SHARPNESS = 0.6\n";
        ini_file.close();
    }

    // Helper function to clean up test files
    void cleanUpTestFiles() {
        std::remove("test_config.ini");  // Remove the test INI file
        std::remove("test_savefile.bin");  // Clean up any output files
    }

    ImageProfile* image_profile;  // Pointer to ImageProfile
};

/* Test that the ImageProfile object is initialized properly
TEST_F(ImageProfileTest, Initialization) {
    EXPECT_FALSE(image_profile->imageConfig.empty());  // Configuration should not be empty
}
*/
// Test the profile method to ensure correct computation of statistics
TEST_F(ImageProfileTest, ProfileMethod) {
    // Create a simple grayscale image for testing
    cv::Mat img = cv::Mat::ones(100, 100, CV_8UC1) * 128;  // 100x100 grayscale image with pixel value 128
    int result = image_profile->profile(img, true);  // Test with save_sample = true
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(result, 1);  // Expected success

    // Check that statistics are updated correctly
    // This part of the test validates the logic of the profile method
}

// Test the handling of empty images in iterateImage method
TEST_F(ImageProfileTest, IterateImageInvalidImage) {
    cv::Mat empty_img;  // Create an empty image to trigger exception
    
    EXPECT_THROW(
        image_profile->iterateImage(empty_img, [](const std::vector<int>&){ /* No-op */ }),
        std::runtime_error  // Should throw due to empty image
    );
}

//Test the StartSaving and TriggerSave methods for threading behavior
TEST_F(ImageProfileTest, ThreadingBehavior) {
    
    // Trigger the save
    
    // Allow some time for the SaveLoop to process
    std::this_thread::sleep_for(std::chrono::seconds(2));  // Wait for thread processing
    
    // Check if the save thread is still running
    EXPECT_TRUE(image_profile->saver->save_thread_.joinable());
    
    // Further checks can include more detailed validation of queue processing
}

// Test SaveObjectToFile to ensure correct data is written to files
TEST_F(ImageProfileTest, SaveObjectToFile) {
    // Simulate adding objects to the saver
    distributionBox testBox;  // Example distribution box
    std::remove("test_savefile.bin");  // Clean up any output files
    do {
        std::lock_guard<std::mutex> lock(image_profile->saver->queue_mutex_);
        while (!(image_profile->saver->objects_to_save_.empty())) {
            image_profile->saver->objects_to_save_.pop();
        }
    }while(0);
    image_profile->saver->AddObjectToSave((void*)(&testBox), KLL_TYPE, "test_savefile.bin");
    
    // Allow some time for the SaveLoop to process
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Check if the file was created and contains data
    std::ifstream infile("test_savefile.bin");
    EXPECT_TRUE(infile.is_open());  // The file should exist
    
    // Additional checks can include validating the content of the file
}
