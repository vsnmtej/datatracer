/**
 * @file ConfidenceMetrics.h
 * @brief Header file for ImageSampler class and confidence metric calculations
 */

#ifndef CONFIDENCE_METRICS_H
#define CONFIDENCE_METRICS_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

/**
 * @class ImageSampler
 * @brief Class for selecting uncertain image samples for further analysis based on various confidence metrics
 */
class ImageSampler {
public:
  /**
   * @brief Constructor to initialize ImageSampler object with configuration file path
   * @param configFilePath Path to the configuration file
   */
  explicit ImageSampler(const std::string& configFilePath);

  // Typedef for distribution box data structure (assuming datasketches::kll_sketch<unit>)
  typedef datasketches::kll_sketch<unit> distributionBox;

  /**
   * @brief Selects uncertain image samples based on configured criteria
   * @param classificationResults Vector of confidence scores for each image prediction
   * @param image OpenCV image matrix
   * @param saveSample Flag indicating whether to save sampled images
   */
  void sample(const std::vector<std::pair<float, int>>& classificationResults, const cv::Mat& image, bool saveSample = true);

private:
  /**
   * @brief Calculates margin confidence (difference between top two probabilities)
   * @param probabilityDistribution Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Margin confidence score
   */
  float calculateMarginConfidence(const std::vector<float>& probabilityDistribution, bool sorted = false);

  /**
   * @brief Calculates least confidence (normalized maximum probability)
   * @param probabilityDistribution Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Least confidence score
   */
  float calculateLeastConfidence(const std::vector<float>& probabilityDistribution, bool sorted = false);

  /**
   * @brief Calculates ratio confidence (ratio of top two probabilities)
   * @param probabilityDistribution Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Ratio confidence score
   */
  float calculateRatioConfidence(const std::vector<float>& probabilityDistribution, bool sorted = false);

  /**
   * @brief Calculates entropy-based confidence
   * @param probabilityDistribution Vector of class probabilities
   * @return Entropy-based confidence score
   */
  float calculateEntropyBasedConfidence(const std::vector<float>& probabilityDistribution);

  // Functions for determining sampling methods (implementations assumed elsewhere)
  // /**
  //  * @brief Determines active uncertainty sampling methods based on configuration
  //  * @return Vector of active uncertainty sampling method IDs
  //  */
  // std::vector<int> determineActiveUncertaintySamplingMethods() const;

  // /**
  //  * @brief Determines active filter flags based on active sampling methods
  //  * @param samplingMethods Vector of active uncertainty sampling method IDs
  //  * @return Combined filter flag value
  //  */
  // int determineActiveFilterFlags(const std::vector<int>& samplingMethods) const;

  /**
   * @brief Saves the image with a timestamped filename (implementation assumed elsewhere)
   * @param image OpenCV image matrix
   */
  void checkAndSave(const cv::Mat& image);

  // Member variables for storing confidence metric statistics
  distributionBox marginConfidenceBox;
  distributionBox leastConfidenceBox;
  distributionBox ratioConfidenceBox;
  distributionBox entropyConfidenceBox;
  std::string imageSamplePath_; // Path for saving sampled images (assuming set in constructor)
};

#endif // CONFIDENCE_METRICS_H

