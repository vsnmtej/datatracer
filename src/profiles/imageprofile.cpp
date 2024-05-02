/**
 * @file ImageProfile.cpp
 * @brief Implements ImageProfile class for image analysis
 */

#include <vector>
#include <cmath>
#include "imageprofile.h"
#include "IniParser.h"

/**
 * @class ImageProfile
 * @brief Class for computing and managing various image statistics
 */
  /**
   * @brief Constructor to initialize ImageProfile object
   * @param conf_path Path to configuration file
   * @param saver Saver object for saving statistics
   */


ImageProfile::ImageProfile(std::string conf_path, Saver<distributionBox>& saver, int channels=1) {
    try {
      // Read configuration settings
		IniParser parser; // Assuming filename is correct
      imagemetricsConfidence = parser.parseIniFile(conf_path,
		      "imagemetrics.confidence", "");
      filesSavePath = parser.parseIniFile(conf_path,
			  "files.savepath", "");

      // Register statistics for saving based on configuration
      for (const auto& stat_confidence : imagemetricsConfidence) {
        std::string name = stat_confidence.first;
          if (strcmp(name.c_str(), "NOISE") == 0){
            saver.AddObjectToSave(noiseBox, filesSavePath["imgstats"]+"margin.bin");
	  }  
          else if (strcmp(name.c_str(), "BRIGHTNESS") == 0){
            saver.AddObjectToSave(brightnessBox, filesSavePath["imgstats"]+"brightness.bin");
	  }  
          else if (strcmp(name.c_str(), "SHARPNESS") == 0){
            saver.AddObjectToSave(sharpnessBox, filesSavePath["imgstats"]+"sharpness.bin");
	  }
          else if (strcmp(name.c_str(), "MEAN") == 0){
		  for (int i = 0; i < channels; ++i) {
                       saver.AddObjectToSave(meanBox[i],
				       filesSavePath["imgstats"]+"mean_"+std::to_str(i)+".bin"); 
                   }
	  } 
          else if (strcmp(name.c_str(), "HISTOGRAM") == 0) {
             for (int i = 0; i < channels; ++i) {
                       saver.AddObjectToSave(pixelBox[i],
				       filesSavePath["imgstats"]+"pixel_"+std::to_str(i)+".bin"); 
             } 
          }	     
       }
    } catch (const std::runtime_error& e) {
      std::cerr << e.what() << std::endl;
    }
  }


  /**
   * @brief Computes and logs selected image statistics
   * @param imgprofile Vector of statistics to compute
   * @param img OpenCV image matrix
   * @param save_sample Flag indicating whether to save samples exceeding thresholds
   * @return 1 on success, error code on failure
   */
  int ImageProfile::profile(std::vector<cv::Mat> &imgv, bool save_sample = false) {
    float stat_score;
	for (cv::Mat img : imgv) {
    for (const auto& imgstat : samplingConfidences) {
		// Access name and threshold from the pair
		std::string name = imgstat.first;
		double threshold = imgstat.second;
		std::string baseName = name;
		if (strcmp(name.c_str(), "NOISE") == 0) {
          // Compute noise statistic
          stat_score = computeNoise(&img);
          // Update corresponding distribution box and save image if threshold exceeded
          noiseBox.update(stat_score);
          if (stat_score >= threshold && save_sample == true) {
			  std::string imagePath = filesSavePath["noise"];
              std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
          }
        } else if (strcmp(name.c_str(), "BRIGHTNESS") == 0) {
			stat_score = calcBrightness(&img);
			brightnessBox.update(stat_score);
			if (stat_score >= threshold && save_sample==true){
				std::string imagePath = filesSavePath["brightness"];
                std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
          }
        } else if (strcmp(name.c_str(), "SHARPNESS") == 0) {
			stat_score = calcSharpness(&img);
			sharpnessBox.update(stat_score);
			if (stat_score >= threshold && save_sample==true){
					std::string imagePath = filesSavePath["sharpness"];
					std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
			}
        } else if (strcmp(name.c_str(), "MEAN") == 0) {
		         cv::scalar mean_values = cv::mean(img);
			 for (int i = 0; i < mean_value.rows; ++i) {
			      meanBox[i].update(mean_values[i]);	 
                         }    
        } else if (strcmp(name.c_str(), "CONTRAST") == 0) {
			stat_score = calcContrast(&img);
			contrastBox.update(stat_score);
			if (stat_score >= threshold && save_sample==true){
				std::string imagePath = filesSavePath["contrast"];
				std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
			}
        } else if (strcmp(name.c_str(), "HISTOGRAM") == 0) {
		        try {
        iterateImage(colorImage, printPixelValues);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
			if (channel==3){
				cv::MatIterator_<cv::Vec3b> it, end;
				for (it = img.begin<cv::Vec3b>(); it != end; ++it) {
					// Access pixel value through iterator
					cv::Vec3b pixel_value = *it;
					pixelBox[i].update(pixel_value[0]);
					pixelBox[i].update(pixel_value[1]);
					pixelBox[i].update(pixel_value[2]);
				}
			}
			else if (channel==1){
				cv::MatIterator_<uchar> it, end;
				for (it = img.begin<uchar>(); it != end; ++it) {
					// Access pixel value through iterator (assuming single-channel uchar data type)
					uchar pixel_value = *it;
					pixelBox.update(pixel_value);
				}
			}
		}
        }
    }
    return 1; // Indicate success
  }

    auto printPixelValues = [](const std::vector<int>& pixelValues) {
        std::cout << "Pixel values: ";
        for (int value : pixelValues) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    };
