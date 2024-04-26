/**
 * @file ImageProfile.h (assuming this is a header file)
 * @brief Header file for the ImageProfile class.
 *
 * This header file defines the ImageProfile class, which is used to analyze and store image statistics.
 */

#ifndef IMAGE_PROFILE_H
#define IMAGE_PROFILE_H

#include "IniParser.h" // Assuming declarations for IniReader, Saver, distributionBox
#include "imghelpers.h"
#include "saver.h"
#include <kll_sketch.hpp>


// Typedef for distribution box data structure (assuming datasketches::kll_sketch<float>)
typedef datasketches::kll_sketch<float> distributionBox;

/**
 * @class ImageProfile
 * @brief A class for analyzing and storing image statistics.
 *
 * This class provides functionalities for analyzing image properties like distribution of pixel values, contrast, brightness, etc. It utilizes KLL sketches for memory-efficient storage of these statistics.
 */
class ImageProfile {
public:

	int channel;

  /**
   * @brief Constructs an ImageProfile object with the specified image characteristics.
   *
   * @param channels The number of channels in the image (e.g., grayscale: 1, RGB: 3).
   * @param img_type The image type (implementation specific).
   * @param metrics The set of image metrics to be tracked (e.g., "contrast", "brightness").
   */
  ImageProfile(std::string conf_path, Saver<distributionBox>& saver);

  /**
   * @brief Logs image statistics for the provided image data.
   *
   * This function analyzes the provided image data and updates the internal KLL sketches with relevant statistics.
   *
   * @param img The image data as a vector of bytes.
   * @param image_width The width of the image in pixels.
   * @param image_height The height of the image in pixels.
   * @param imgpixels_stat A map to store per-channel statistics (implementation specific).
   * @param imgprofile_map A map to store various image profile metrics (e.g., "contrast").
   */

  int profile(std::vector<cv::Mat> &img, bool save_sample);

  std::vector<std::pair<std::string, double>> samplingConfidences;

private:

  float computeNoise(cv::Mat &img);

  /**
   * @brief KLL sketch for storing contrast distribution.
   */
  distributionBox contrastBox;

  /**
   * @brief KLL sketch for storing brightness distribution.
   */
  distributionBox brightnessBox;
  distributionBox sharpnessBox;

  distributionBox histogramBox;
  distributionBox histogramBox_r;
  distributionBox histogramBox_g;
  distributionBox histogramBox_b;

  /**
   * @brief KLL sketch for storing mean pixel value distribution.
   */
  distributionBox meanBox;
  distributionBox entropyBox;

  /**
   * @brief KLL sketch for storing noise distribution.
   */
  distributionBox noiseBox;

  // Per-channel KLL sketches (assuming pixelBox_r, etc. are for individual channels)
  distributionBox pixelBox_r;
  distributionBox pixelBox_g;
  distributionBox pixelBox_b;

  /**
   * @brief KLL sketch for storing overall pixel value distribution.
   */
  distributionBox pixelBox;

	std::map<std::string, std::string> filesSavePath;
	std::map<std::string, std::string> imagemetricsConfidence;
};

#endif
