#include <gtest/gtest.h>
#include "IniParser.h" // Assuming this is the header where the parseIniFile method is defined
#include <map>
#include <fstream>

// A utility function to create a temporary INI file for testing
void createTestIniFile(const std::string& filename, const std::string& content) {
    std::ofstream outFile(filename);
    outFile << content;
    outFile.close();
}

class IniParserTest : public ::testing::Test {
protected:
    std::string testIniFilename = "test.ini";

    virtual void SetUp() override {
        // Creating a basic test INI file with sections and subsections
        std::string iniContent = R"(
[sampling.confidence]
  margincofidence.value = 0.5
  least.value = 0.4

[imagemetrics.confidence]
  noise.value = 0.5
  brightness.value = 0.4
)";

        createTestIniFile(testIniFilename, iniContent);
    }

    virtual void TearDown() override {
        // Clean up the test INI file after tests
        std::remove(testIniFilename.c_str());
    }
};

TEST_F(IniParserTest, ParseValidSectionAndSubsection) {
    std::map<std::string, std::string> result = IniParser::parseIniFile(testIniFilename, "sampling.confidence", "");
    EXPECT_EQ(result["margincofidence"], "0.5");
    EXPECT_EQ(result["least"], "0.4");
}

TEST_F(IniParserTest, ParseSectionAndSpecificSubsection) {
    std::map<std::string, std::string> result = IniParser::parseIniFile(testIniFilename, "imagemetrics.confidence", "noise");
    EXPECT_EQ(result["value"], "0.5");
}

TEST_F(IniParserTest, ParseInvalidSection) {
    std::map<std::string, std::string> result = IniParser::parseIniFile(testIniFilename, "invalid.section", "");
    EXPECT_TRUE(result.empty()); // The result should be empty
}

TEST_F(IniParserTest, ParseValidSectionButInvalidSubsection) {
    std::map<std::string, std::string> result = IniParser::parseIniFile(testIniFilename, "sampling.confidence", "nonexistent");
    EXPECT_TRUE(result.empty()); // The result should be empty
}

TEST_F(IniParserTest, ParseEmptyIniFile) {
    createTestIniFile(testIniFilename, ""); // Overwriting with an empty content
    std::map<std::string, std::string> result = IniParser::parseIniFile(testIniFilename, "any.section", "");
    EXPECT_TRUE(result.empty()); // The result should be empty
}


