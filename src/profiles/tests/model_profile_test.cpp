#include "modelprofile.h"
#include "saver.h"
#include <gtest/gtest.h>
#include <fstream>

// Test Fixture for ModelProfile
class ModelProfileTest : public ::testing::Test {
protected:
    void SetUp() override {
        createSampleIniFile("test_config.ini");

        saver = new Saver(1);
        model_profile = new ModelProfile("test_model", "test_config.ini", *saver, 3);
    }

    void TearDown() override {
        delete model_profile;
        delete saver;
        cleanUpTestFiles();
    }

    // Helper functions to create test data
    void createSampleIniFile(const std::string& filename) {
        std::ofstream ini_file(filename, std::ios::trunc);
        ini_file << "[model]\n";
        ini_file << "files = ./\n";
        ini_file.close();
    }

    void cleanUpTestFiles() {
        std::remove("test_config.ini");
    }

    ModelProfile* model_profile;
    Saver* saver;
};

// Test Initialization
TEST_F(ModelProfileTest, Initialization) {
    EXPECT_EQ(model_profile->getNumDistributionBoxes(), 3); // Should create 3 distribution boxes
}

// Test log_classification_model_stats with proper data
TEST_F(ModelProfileTest, LogClassificationModelStats) {
    ClassificationResults results = {
        {0.9f, 1}, // Score and class ID
        {0.8f, 2},
        {0.7f, 3},
    };

    float latency = 1.5f;
    int result = model_profile->log_classification_model_stats(latency, results);

    EXPECT_EQ(result, 0); // Successful logging

    // Validate that the boxes have been updated
    EXPECT_GT(model_profile->getDistributionBox(0).get_n(), 0); // Check if the first box was updated
    EXPECT_GT(model_profile->getDistributionBox(1).get_n(), 0); // Check the second box
    EXPECT_GT(model_profile->getDistributionBox(2).get_n(), 0); // Check the third box
}

// Test Invalid Configuration
TEST_F(ModelProfileTest, InvalidConfiguration) {
    createSampleIniFile("invalid_config.ini");

    EXPECT_THROW(
        ModelProfile("invalid_model", "invalid_config.ini", *saver, 3),
        std::runtime_error  // Should throw due to invalid configuration
    );
}

// Test handling of empty classification results
TEST_F(ModelProfileTest, EmptyClassificationResults) {
    ClassificationResults empty_results;
    float latency = 1.0f;

    int result = model_profile->log_classification_model_stats(latency, empty_results);

    EXPECT_EQ(result, 0); // Should return success even with empty results
}

//Test if objects are registered with Saver
TEST_F(ModelProfileTest, ObjectsRegisteredWithSaver) {
    EXPECT_EQ(saver->objects_to_save_.size(), 3); // Should have 3 objects to save
}

