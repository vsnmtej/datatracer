#include "modelprofile.h"
#include "saver.h"
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

// Test fixture for ModelProfile tests
class ModelProfileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a sample INI file for testing
        createSampleIniFile("test_config.ini");

        // Initialize a Saver object for testing
        saver = new Saver(1);

        // Create a ModelProfile object with sample configuration and Saver
        model_profile = new ModelProfile("test_model", "test_config.ini", *saver, 3); // 3 top classes
    }

    void TearDown() override {
        delete model_profile; // Clean up ModelProfile instance
        delete saver; // Clean up Saver instance
        cleanUpTestFiles(); // Remove test files created during testing
    }

    // Helper function to create a sample INI file for testing
    void createSampleIniFile(const std::string& filename) {
        std::ofstream ini_file(filename, std::ios::trunc);
        ini_file << "[model]\n";
        ini_file << "files = ./\n"; // Sample file path
        ini_file.close();
    }

    // Helper function to clean up test files
    void cleanUpTestFiles() {
        std::remove("test_config.ini"); // Remove the test INI file
    }

    ModelProfile* model_profile; // Pointer to ModelProfile instance
    Saver* saver; // Pointer to Saver instance
};

// Test that the ModelProfile object is initialized correctly
TEST_F(ModelProfileTest, Initialization) {
    EXPECT_FALSE(model_profile->modelConfig.empty()); // Configuration should not be empty
    EXPECT_EQ(model_profile->filesSavePath, "./"); // Expected file path
    EXPECT_EQ(model_profile->boxes.size(), 3); // Should create 3 distribution boxes
}

// Test the log_classification_model_stats method
TEST_F(ModelProfileTest, LogClassificationModelStats) {
    // Create a sample ClassificationResults for testing
    ClassificationResults results = {
        {0.9, 1}, // Score and class ID
        {0.8, 2},
        {0.7, 3},
    };

    float latency = 1.5f; // Example inference latency
    int result = model_profile->log_classification_model_stats(latency, results);

    EXPECT_EQ(result, 0); // Expected successful logging

    // Validate that the distribution boxes have been updated
    EXPECT_GT(model_profile->boxes[0].get_n(), 0); // Check that box 1 was updated
    EXPECT_GT(model_profile->boxes[1].get_n(), 0); // Check that box 2 was updated
    EXPECT_GT(model_profile->boxes[2].get_n(), 0); // Check that box 3 was updated
}

// Test handling of invalid configurations in ModelProfile
TEST_F(ModelProfileTest, InvalidConfiguration) {
    // Create an invalid INI file
    std::ofstream ini_file("invalid_config.ini", std::ios::trunc);
    ini_file << "[invalid]\n"; // Invalid section
    ini_file.close();

    // Attempt to create ModelProfile with invalid configuration
    EXPECT_THROW(
        ModelProfile("invalid_model", "invalid_config.ini", *saver, 3),
        std::runtime_error // Should throw due to invalid configuration
    );
}

// Test handling of empty classification results
TEST_F(ModelProfileTest, EmptyClassificationResults) {
    ClassificationResults empty_results; // Empty classification results
    float latency = 1.0f;

    int result = model_profile->log_classification_model_stats(latency, empty_results);

    EXPECT_EQ(result, 0); // Should still return success
}

// Test if objects are correctly registered with Saver
TEST_F(ModelProfileTest, ObjectsRegisteredWithSaver) {
    EXPECT_EQ(saver->objects_to_save_.size(), 3); // Should have 3 objects to save (3 distribution boxes)
}
