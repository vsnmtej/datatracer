#include <gtest/gtest.h>
#include <fstream>

#include "http_uploader.h"

// Basic test for successful POST request
TEST(HttpUploaderTest, PostFile) {
    HttpUploader uploader("https://sensorupload.datatracer.com", "dummy_token");

    // You can use a small dummy file for the test
    std::string testFilePath = "test_dummy_file.txt";

    // Create a dummy file for testing
    std::ofstream testFile(testFilePath);
    testFile << "Test data";
    testFile.close();

    // Test the POST request
    ASSERT_TRUE(uploader.postFile(testFilePath, "00.00.00", std::time(nullptr)));

    // Cleanup the dummy file after test
    std::remove(testFilePath.c_str());
}
