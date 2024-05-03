#include <gtest/gtest.h>
#include "ImageSampler.h"

// Compile the test with:
// g++ -std=c++17 -lgtest -lgtest_main -pthread -o test_ImageSampler test_ImageSampler.cpp

// Test the margin_confidence function
TEST(ImageSamplerTest, MarginConfidence) {
    ImageSampler sampler;

    std::vector<float> prob_dist = {0.1f, 0.2f, 0.4f, 0.3f};
    float result = sampler.margin_confidence(prob_dist);
    EXPECT_NEAR(result, 0.9f, 1e-5); // The margin confidence should be 1.0 - (0.4 - 0.3) = 0.9

    std::vector<float> sorted_prob_dist = {0.4f, 0.3f, 0.2f, 0.1f};
    result = sampler.margin_confidence(sorted_prob_dist, true);
    EXPECT_NEAR(result, 0.9f, 1e-5);
}

// Test the least_confidence function
TEST(ImageSamplerTest, LeastConfidence) {
    ImageSampler sampler;

    std::vector<float> prob_dist = {0.1f, 0.2f, 0.4f, 0.3f};
    float result = sampler.least_confidence(prob_dist);
    EXPECT_NEAR(result, 0.6f * (4.0f / 3.0f), 1e-5); // 1.0 - 0.4, then scaled with (4 / 3)

    std::vector<float> sorted_prob_dist = {0.4f, 0.3f, 0.2f, 0.1f};
    result = sampler.least_confidence(sorted_prob_dist, true);
    EXPECT_NEAR(result, 0.6f * (4.0f / 3.0f), 1e-5); // Same expected result
}

// Test the ratio_confidence function
TEST(ImageSamplerTest, RatioConfidence) {
    ImageSampler sampler;

    std::vector<float> prob_dist = {0.1f, 0.2f, 0.4f, 0.3f};
    float result = sampler.ratio_confidence(prob_dist);
    EXPECT_NEAR(result, 0.3f / 0.4f, 1e-5);

    std::vector<float> sorted_prob_dist = {0.4f, 0.3f, 0.2f, 0.1f};
    result = sampler.ratio_confidence(sorted_prob_dist, true);
    EXPECT_NEAR(result, 0.3f / 0.4f, 1e-5);
}

// Test the entropy_confidence function
TEST(ImageSamplerTest, EntropyConfidence) {
    ImageSampler sampler;

    std::vector<float> prob_dist = {0.1f, 0.2f, 0.4f, 0.3f};
    float result = sampler.entropy_confidence(prob_dist);
    float expected_entropy = -(0.4f * std::log2(0.4f) + 0.3f * std::log2(0.3f) + 0.2f * std::log2(0.2f) + 0.1f * std::log2(0.1f));
    expected_entropy /= std::log2(4.0f);
    EXPECT_NEAR(result, expected_entropy, 1e-5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
