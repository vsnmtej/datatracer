#include <gtest/gtest.h>
#include "Saver.h" // Include your Saver header
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

// A simple struct representing a KLL Data Sketch with a serialize function
struct KLLSketch {
    int data;
    
    void serialize(std::ostream& os) const {
        os << "KLLSketch with data: " << data << "\n";
    }
};

// Test class with common test utilities
class SaverTest : public ::testing::Test {
protected:
    std::string testFilename = "test_save.dat"; // Name of the file to save

    void SetUp() override {
        // Clear the file before each test
        std::ofstream ofs(testFilename, std::ios::trunc);
        ofs.close();
    }

    void TearDown() override {
        // Remove the test file after each test
        std::remove(testFilename.c_str());
    }
};

TEST_F(SaverTest, AddObjectToSave) {
    Saver<KLLSketch>& saver = Saver<KLLSketch>::GetInstance(5); // Save interval of 5 minutes
    KLLSketch sketch{42};
    
    saver.AddObjectToSave(sketch, testFilename); // Add a KLLSketch object to save
    
    // Check if the object was added to the queue
    EXPECT_TRUE(!saver.objects_to_save_.empty());
}

TEST_F(SaverTest, SaveObjectToFile) {
    Saver<KLLSketch>& saver = Saver<KLLSketch>::GetInstance(5); // Save interval of 5 minutes
    KLLSketch sketch{42};
    
    saver.SaveObjectToFile(sketch, testFilename); // Save manually
    
    // Check if the file has the correct content
    std::ifstream is1(testFilename);
    auto sketch1 = datasketches::kll_sketch<float>::deserialize(is1);
    
    EXPECT_EQ(u.get_min_item(), "42");
}

TEST_F(SaverTest, StartSavingAndTriggerSave) {
    Saver<KLLSketch>& saver = Saver<KLLSketch>::GetInstance(5);
    saver.StartSaving(); // Start the save loop
    
    KLLSketch sketch{42};
    saver.AddObjectToSave(sketch, testFilename); // Add object to save
    
    // Trigger save manually
    saver.TriggerSave();
    
    // Wait for some time to let the background thread work
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    / Check if the file has the correct content
    std::ifstream is1(testFilename);
    auto sketch1 = datasketches::kll_sketch<float>::deserialize(is1);
  

    EXPECT_EQ(u.get_min_item(), "42");
}

