/**
 * @file ImageSampler.cpp
 * @brief Implements ImageSampler class for image sampling based on uncertainty
 */
#include "ImageSampler.h"
#include "imghelpers.h"

/**
 * @class ImageSampler
 * @brief Class for selecting uncertain image samples for further analysis
 */
  /**
   * @brief Constructor to initialize ImageSampler object
   * @param conf_path Path to configuration file
   * @param saver Saver object for saving sampling statistics
   */
ImageSampler::ImageSampler(std::string conf_path, Saver<distributionBox> &saver) {
  try {
    // Read configuration settings
    IniParser parser; // Assuming filename is correct
    samplingConfidences = parser.parseIniFile(conf_path, "sampling.confidence", "");
    filesSavePath = parser.parseIniFile(conf_path, "files.savepath", "");

    // Register sampling statistics for saving based on configuration
    for (const auto& sampling_confidence : samplingConfidences) {
      std::string name = sampling_confidence.first;
      if (strcmp(name.c_str(), "MARGINCONFIDENCE")) {
      saver.AddObjectToSave(marginConfidenceBox, filesSavePath["samplestats"]+"marginconfidence.bin");
      } else if(strcmp(name.c_str(), "LEASTCONFIDENCE") == 0) {
      saver.AddObjectToSave(leastConfidenceBox, filesSavePath["samplestats"]+"leastconfidence.bin");
      } else if(strcmp(name.c_str(), "RATIOCONFIDENCE") == 0) {
      // ... Register other sampling statistics similarly
      saver.AddObjectToSave(ratioConfidenceBox, filesSavePath["samplestats"]+"ratioconfidence.bin");
      } else if(strcmp(name.c_str(), "ENTROPYCONFIDENCE") == 0) {
      saver.AddObjectToSave(entropyConfidenceBox, filesSavePath["samplestats"]+"entropyconfidence.bin");
      }
    }
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
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

int ImageSampler::sample(std::vector<std::pair<float, int>> &results, cv::Mat &img, bool save_sample = true) {
	std::vector<float> confidence; // Extract confidence scores

	// Extract confidence scores from results
	for (const auto& pair : results) {
		confidence.push_back(pair.first);
	}

	// Apply configured sampling criteria to identify uncertain samples
	for (const auto& sampling_confidence : samplingConfidences) {
		std::string name = sampling_confidence.first;
		double threshold = std::stod(sampling_confidence.second);
		float confidence_score = -1.0f;

		if (strcmp(name.c_str(), "marginconfidence") == 0) {
			// Compute margin confidence and update statistics
			confidence_score = margin_confidence(confidence, false);
			marginConfidenceBox.update(confidence_score);
			if (confidence_score >= threshold) {
				std::string imagePath = filesSavePath["marginconfidence"];
				std::string baseName = "marginconfidence";
				std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
			}
		} else if (strcmp(name.c_str(), "leastconfidence") == 0) {
			confidence_score = least_confidence(confidence, false);
			leastConfidenceBox.update(confidence_score);
			if (confidence_score >= threshold){
				std::string imagePath = filesSavePath["leastconfidence"];
				std::string baseName = "leastconfidence";
				std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
			}
		} else if (strcmp(name.c_str(), "ratioconfidence") == 0) {
			confidence_score = ratio_confidence(confidence, false);
			ratioConfidenceBox.update(confidence_score);
			if (confidence_score >= threshold){
				std::string imagePath = filesSavePath["ratioconfidence"];
				std::string baseName = "ratioconfidence";
				std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
			}
		} else if (strcmp(name.c_str(), "entropyconfidence") == 0) {
			confidence_score = entropy_confidence(confidence);
			entropyConfidenceBox.update(confidence_score);
			if (confidence_score >= threshold){
				std::string imagePath = filesSavePath["entropyconfidence"];
				std::string baseName = "entropyconfidence";
				std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
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

   float ImageSampler::entropy_confidence(std::vector<float>& prob_dist) {
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
