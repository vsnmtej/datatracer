/**
 * @file ImageProfile.cpp
 * @brief Implements ImageProfile class for image analysis
 */

#include <vector>
#include <cmath>
#include "helpers/inireader.h" // Assuming declarations for IniReader, Saver, distributionBox
#include "helpers/imagehelpers.h"

/**
 * @class ImageProfile
 * @brief Class for computing and managing various image statistics
 */
  /**
   * @brief Constructor to initialize ImageProfile object
   * @param conf_path Path to configuration file
   * @param saver Saver object for saving statistics
   */
ImageProfile::ImageProfile(std::string conf_path, Saver<distributionBox>& saver) {
    try {
      // Read configuration settings
      std::map<std::string, std::string> imagemetricsConfidence = parseIniFile(conf_path,
		      "imagemetrics.confidence", "");
      std::map<std::string, std::string> filesSavePath = parseIniFile(conf_path,
			  "files.savepath", "");
      // Register statistics for saving based on configuration
      for (const auto& stat_confidence : imagemetricsConfidence) {
        std::string name = stat_confidence.first;
        switch (name) {
          case "NOISE": saver.AddObjectToSave(noiseBox, filesSavePath["imgstats"]+"margin.bin"); break;
          case "BRIGHTNESS": saver.AddObjectToSave(brightnessBox, filesSavePath["imgstats"]+"brightness.bin"); break;
          case "SHARPNESS": saver.AddObjectToSave(sharpnessBox, filesSavePath["imgstats"]+"sharpness.bin"); break;
	  case "MEAN": saver.AddObjectToSave(meanBox, "filesSavePath["imgstats]+"mean.bin"); break;
	  case "HISTOGRAM":
	        if(channel ==1){
		        saver.AddObjectToSave(histogramBox, filesSavePath["imgstats"]+"histogram.bin");
	        }else if(channel ==3){
		        saver.AddObjectToSave(hisogramBox_b, filesSavePath["imgstats"]+"histogram_b.bin");
		        saver.AddObjectToSave(hisogramBox_g, filesSavePath["imgstats"]+"histogram_g.bin");
		        saver.AddObjectToSave(hisogramBox_r, filesSavePath["imgstats"]+"histogram_r.bin");
		}; break;
          }
      }
    } catch (const std::runtime_error& e) {
      std::cerr << e.what() << std::endl;
      return 1; // Indicate error
    }
  }

  /**
   * @brief Computes and logs selected image statistics
   * @param imgprofile Vector of statistics to compute
   * @param img OpenCV image matrix
   * @param save_sample Flag indicating whether to save samples exceeding thresholds
   * @return 1 on success, error code on failure
   */
  int ImageProfile::profile(std::vector<cv::Mat &img, bool save_sample = false) {
    for (const auto& imgstat : imagemetricsConfidence) {
      // Access name and threshold from the pair
      std::string name = imgstat.first;
      double threshold = std::stod(imgstat.second);
      switch (name) {
        case "noise": {
          // Compute noise statistic
          float stat_score = computeNoise(img);
          // Update corresponding distribution box and save image if threshold exceeded
          noiseBox.update(stat_score);
          if (stat_score >= threshold && save_sample == true) {
	      std::string imagePath = filesSavePath['noise'];
              std::string baseName = str(int(stat_score));
              std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
          }
          break;
        }
        case "brightness":{
	    stat_score = computeBrightness(&img);
	    brightnessBox.update(stat_score);
	    if (stat_score >= threshold && save_sample==true){
	        std::string imagePath = filesSavePath['brightness'];
                std::string baseName = str(int(stat_score));
                std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
          }
          break;
        }
	case "sharpness":{
	    stat_score = computeSharpness(&img);
	    sharpnessBox.update(stat_score);
	    if (stat_score >= threshold && save_sample==true){
                std::string imagePath = filesSavePath['sharpness'];
                std::string baseName = str(int(stat_score));
                std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
	    }
             break;
         }
	 case "mean":
	     stat_score = computeMean(&img);
	     entropyBox.update(stat_score);
	     if (stat_score >= threshold && save_sample==true){
		std::string imagePath = filesSavePath['mean'];
                std::string baseName = str(int(stat_score));
                std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
	      }
              break;
          }  
	  case "contrast":
	      stat_score = computeContrast(&img);
	      contrastBox.update(stat_score);
	      if (stat_score >= threshold && save_sample==true){
		std::string imagePath = filesSavePath['contrast'];
                std::string baseName = str(int(stat_score));
                std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
	      }
               break;
            }
	    case "histogram":{
               if (ch==3){
	        cv::MatIterator_<cv::Vec3b> it, end;
                 for (it = img.begin<cv::Vec3b>(); it != end; ++it) {
                    // Access pixel value through iterator
                        cv::Vec3b pixel_value = *it;
			pixelBox_b.update(pixel_value[0]);
			pixelBox_g.update(pixel_value[1]);
			pixelBox_r.update(pixel_value[2]);
                     }
	         }
	         else if (ch==1){
		    cv::MatIterator_<uchar> it, end;
                    for (it = grayscale_image.begin<uchar>(); it != end; ++it) {
                    // Access pixel value through iterator (assuming single-channel uchar data type)
                        uchar pixel_value = *it;
			pixelBox.update(pixel_value[0]);
                   }	 
	       }	  
      }
    }
    return 1; // Indicate success
  }


  /**
   * @brief Computes image noise using SNR
   * @param img OpenCV image matrix
   * @return Calculated noise value
   */
  float ImageProfile::computeNoise(cv::Mat &img) {
    return computeSNR(&img); // Assuming computeSNR implementation exists
  }

  /**
   * @brief Computes image brightness 
   * @param img OpenCV image matrix
   * @return Calculated brightness value
   */

  float ImageProfile::logBrightness(cv::Mat &img){
    return computeBrightness(&img);
  }
  /**
   * @brief Computes image contrast 
   * @param img OpenCV image matrix
   * @return Calculated contrast value
   */
   float ImageProfile::logContrast(cv::Mat &img){
    return computeContrast(&img);
   }
  /**
   * @brief Computes image sharpness using SNR
   * @param img OpenCV image matrix
   * @return Calculated sharpness value
   */
    float ImageProfile::logSharpness(cv::Mat &img){
     return computeSharpness(&img);    
   }
  /**
   * @brief Computes image mean 
   * @param img OpenCV image matrix
   * @return Calculated mean value
   */
    int ImageProfile::logMean(cv::Mat &img){
    return computeMean(&img);    
   }
