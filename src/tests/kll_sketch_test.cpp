#include <gtest/gtest.h>
#include "KLLSketch.h"

TEST(KLLSketchTest, UpdateAndQuantile) {
    KLLSketch sketch(128);

    for (int i = 0; i < 1000; i++) {
        double data_point = static_cast<double>(i);
        sketch.update(data_point);
    }

    // Test quantile estimation for various quantiles
    double quantiles[] = {0.25, 0.5, 0.75, 0.9};
    for (double quantile : quantiles) {
        double estimate = sketch.getQuantile(quantile);
        ASSERT_GT(estimate, 0.0); // Ensure estimate is non-negative

        // Check if the estimate is within a reasonable range
        double expected_value = quantile * 1000;
        ASSERT_LE(estimate, expected_value * 1.1);
        ASSERT_GE(estimate, expected_value * 0.9);
    }
}
