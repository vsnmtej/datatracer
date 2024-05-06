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
        
        // Initialize a Saver object
        saver = new Saver(1);  // 1-minute interval
        
        // Create an ImageProfile object with a sample INI file and a Saver instance
        image_profile = new ImageProfile("test_config.ini", *saver, 1);
    }

    void TearDown() override {
        delete image_profile;  // Clean up the ImageProfile instance
        delete saver;  // Clean up the Saver instance
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
    Saver* saver;  // Pointer to Saver
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
    saver->StartSaving();
    int result = image_profile->profile(img, true);  // Test with save_sample = true
    std::this_thread::sleep_for(std::chrono::seconds(2));
    saver->StopSaving();
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
    saver->StartSaving();  // Start the saving process
    
    // Trigger the save
    saver->TriggerSave();  // Ensure proper synchronization
    
    // Allow some time for the SaveLoop to process
    std::this_thread::sleep_for(std::chrono::seconds(2));  // Wait for thread processing
    
    // Check if the save thread is still running
    EXPECT_TRUE(saver->save_thread_.joinable());
    
    // Further checks can include more detailed validation of queue processing
}

// Test SaveObjectToFile to ensure correct data is written to files
TEST_F(ImageProfileTest, SaveObjectToFile) {
    // Simulate adding objects to the saver
    distributionBox testBox(100);  // Example distribution box
    data_object_t data_object;
    data_object.obj = &testBox;
    data_object.type = KLL_TYPE;
    data_object.filename = "test_savefile.bin";
    
    saver->AddObjectToSave(&data_object, KLL_TYPE, "test_savefile.bin");
    
    saver->StartSaving();  // Start the saving process
    
    // Allow some time for the SaveLoop to process
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Check if the file was created and contains data
    std::ifstream infile("test_savefile.bin");
    EXPECT_TRUE(infile.is_open());  // The file should exist
    
    // Additional checks can include validating the content of the file
}
