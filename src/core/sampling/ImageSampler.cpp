/**
 * @file ImageSampler.cpp
 * @brief Implements ImageSampler class for image sampling based on uncertainty
 */

#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "IniReader.h"

/**
 * @class ImageSampler
 * @brief Class for selecting uncertain image samples for further analysis
 */
  /**
   * @brief Constructor to initialize ImageSampler object
   * @param conf_path Path to configuration file
   * @param saver Saver object for saving sampling statistics
   */
ImageSampler::ImageSampler(std::string conf_path, Saver<distributionBox>& saver) {
    try {
      // Read configuration settings
      std::map<std::string, std::string> samplingConfidences = parseIniFile(conf_path,
		      "sampling.confidence", "");
      std::map<std::string, std::string> filesSavePath = parseIniFile(conf_path,
			  "files.savepath", "");

      // Register sampling statistics for saving based on configuration
      for (const auto& sampling_confidence : samplingConfidences) {
        std::string name = sampling_confidence.first;
        switch (name) {
          case "MARGINCONFIDENCE":
            saver.AddObjectToSave(marginConfidenceBox, filesSavePath["samplestats"]+"marginconfidence.bin");
            break;
          case "LEASTCONFIDENCE":
            saver.AddObjectToSave(leastConfidenceBox, filesSavePath["samplestats"]+"leastconfidence.bin");
            break;
          // ... Register other sampling statistics similarly
	  case "RATIOCONFIDENCE":
            saver.AddObjectToSave(ratioConfidenceBox, filesSavePath["samplestats"]+"ratioconfidence.bin");
	    break;
	  case "ENTROPYCONFIDENCE":
            saver.AddObjectToSave(entropyConfidenceBox, filesSavePath["samplestats"]+"entropyconfidence.bin");
	    break;
        }
      }
    } catch (const std::runtime_error& e) {
      std::cerr << e.what() << std::endl;
      return 1; // Indicate error
    }
  }

  /**
   * @brief Selects uncertain image samples based on configured criteria
   * @param results Vector of confidence scores for each image prediction
   * @param uncertainty_sampling Vector indicating uncertainty criteria for each sample
   * @param img OpenCV image matrix
   * @param save_sample Flag indicating whether to save sampled images
   * @return 1 on success, error code on failure
   */

  int ImageSampler::sampling(std::vector<std::pair<float, int>> &results, std::vector<std::pair<int, bool>> &uncertainty_sampling, cv::Mat &img, bool save_sample = true) {
    std::vector<float> confidence; // Extract confidence scores

    // Extract confidence scores from results
    for (const auto& pair : results) {
      confidence.push_back(pair.first);
    }

    // Apply configured sampling criteria to identify uncertain samples
    for (const auto& sampling_confidence : samplingConfidences) {
      std::string name = sampling_confidence.first;
      double threshold = sampling_confidence.second;
      float confidence_score = -1.0f;

      switch (name) {
        case "MARGINCONFIDENCE":
          // Compute margin confidence and update statistics
          confidence_score = margin_confidence(confidence);
          marginConfidencebox.update(confidence_score);
          if (confidence_score >= threshold) {
	      std::string imagePath = filesSavePath['marginconfidence'];
              std::string baseName = "marginconfidence";
              std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
          }

          break;
	case "LEASTCONFIDENCE":
	    confidence_score = least_confidence(&confidence);
	    leastConfidencebox.update(confidence_score);
	    if (confidence_score >= threshold){
	      std::string imagePath = filesSavePath['leastconfidence'];
              std::string baseName = "leastconfidence";
              std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
	    }
	  break; 
	case "RATIOCONFIDENCE":
	    confidence_score = ratio_confidence(&confidence);
	    ratioConfidencebox.update(confidence_score);
	    if (confidence_score >= threshold){
	      std::string imagePath = filesSavePath['ratioconfidence'];
              std::string baseName = "ratioconfidence";
              std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
	    }
	  break; 
	case "ENTROPYCONFIDENCE":
	    confidence_score = entropy_confidence(&confidence);
	    entropyConfidencebox.update(confidence_score);
	    if (confidence_score >= threshold){
	      std::string imagePath = filesSavePath['entropyconfidence'];
              std::string baseName = "entropyconfidence";
              std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
	    }
	 break; 
       }
      }
    }
    return 1; // Indicate success
  }


  /**
   * @brief Calculates margin confidence (difference between top two probabilities)
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Margin confidence score
   */

   float ImageSampler::margin_confidence(std::vector<float>& prob_dist, bool sorted = false) {
    if (!sorted) {
        std::sort(prob_dist.begin(), prob_dist.end(), std::greater<float>()); // Sort probabilities from largest to smallest
    }
    float difference = prob_dist[0] - prob_dist[1]; // Difference between the top two probabilities
    float margin_conf = 1.0f - difference;
    return margin_conf;
   }

   
  /**
   * @brief Calculates least confidence 
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return least confidence score
   */

    float ImageSampler::least_confidence(std::vector<float>& prob_dist, bool sorted = false) {
    float simple_least_conf;
    if (sorted) {
        simple_least_conf = prob_dist[0]; // Most confident prediction
    } else {
        simple_least_conf = *std::max_element(prob_dist.begin(), prob_dist.end()); // Most confident prediction
    }
    size_t num_labels = prob_dist.size(); // Number of labels
    float normalized_least_conf = (1.0f - simple_least_conf) * (static_cast<float>(num_labels) / (num_labels - 1));
    return normalized_least_conf;
    }


  /**
   * @brief Calculates ratio confidence (difference between top two probabilities)
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Margin confidence score
   */

    float ImageSampler::ratio_confidence(std::vector<float>& prob_dist, bool sorted = false) {
    if (!sorted) {
        std::sort(prob_dist.begin(), prob_dist.end(), std::greater<float>()); // Sort probabilities from largest to smallest
    }
    float ratio_conf = prob_dist[1] / prob_dist[0]; // Ratio between the top two probabilities
    return ratio_conf;
    }


  /**
   * @brief Calculates entropy based confidence
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Entropy confidence score
   */

   float ImageSampler::entropy_based(std::vector<float>& prob_dist) {
    float raw_entropy = 0.0;
    size_t num_labels = prob_dist.size();

    for (size_t i = 0; i < num_labels; i++) {
        if (prob_dist[i] > 0.0) {
            raw_entropy -= prob_dist[i] * std::log2(prob_dist[i]); // Multiply each probability by its base 2 log and sum
        }
    }
    float normalized_entropy = raw_entropy / std::log2(static_cast<float>(num_labels));
    return normalized_entropy;
    }
