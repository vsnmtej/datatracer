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


ImageProfile::ImageProfile(std::string conf_path, int save_interval, int channels=1) {
    try {
        Aws::Auth::AWSCredentials credentials;
        Aws::String region;
        std::string bucketName;
        std::string objectKey;
        std::chrono::milliseconds interval;

        uploader = new ImageUploader(credentials, region);
        saver = new Saver(save_interval);

      // Read configuration settings
		IniParser parser; // Assuming filename is correct
      imageConfig = parser.parseIniFile(conf_path,
		      "image", "");
      filesSavePath = imageConfig["filepath"];

      // Register statistics for saving based on configuration
      for (const auto& stat_confidence : imageConfig) {
        std::string name = stat_confidence.first;
          if (strcmp(name.c_str(), "NOISE") == 0){
            saver->AddObjectToSave((void*)(&noiseBox), KLL_TYPE, filesSavePath+"margin.bin");
	  }  
          else if (strcmp(name.c_str(), "BRIGHTNESS") == 0){
            saver->AddObjectToSave((void*)(&brightnessBox), KLL_TYPE, filesSavePath+"brightness.bin");
	  }  
          else if (strcmp(name.c_str(), "SHARPNESS") == 0){
            saver->AddObjectToSave((void*)(&sharpnessBox), KLL_TYPE, filesSavePath+"sharpness.bin");
	  }
          else if (strcmp(name.c_str(), "MEAN") == 0){
		  for (int i = 0; i < channels; ++i) {
		       distributionBox dbox(200);	  
		       meanBox.push_back(dbox); 	  
                       saver->AddObjectToSave((void*)(&meanBox[i]),
				       KLL_TYPE, filesSavePath+"mean_"+std::to_string(i)+".bin"); 
                   }
	  } 
          else if (strcmp(name.c_str(), "HISTOGRAM") == 0) {
             for (int i = 0; i < channels; ++i) {
		       distributionBox dbox(200);	
                       pixelBox.push_back(dbox);
		       saver->AddObjectToSave((void*)(&pixelBox[i]),
				       KLL_TYPE, filesSavePath+"pixel_"+std::to_string(i)+".bin"); 
             }
          }	     
       }
    saver->StartSaving();
    uploader->startUploadThread(filesSavePath, bucketName, objectKey, interval);
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
  int ImageProfile::profile(cv::Mat &img, bool save_sample = false) {
    float stat_score;
    for (const auto& imgstat : imageConfig) {
		// Access name and threshold from the pair
		std::string name = imgstat.first;
		std::string baseName = name;
	if (strcmp(name.c_str(), "NOISE") == 0) {
          // Compute noise statistic
          stat_score = calcSNR(img);
	  float threshold = std::stof(imgstat.second);
          // Update corresponding distribution box and save image if threshold exceeded
          noiseBox.update(stat_score);
          if (stat_score >= threshold && save_sample == true) {
			  std::string imagePath = filesSavePath;
              std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
          }
        } else if (strcmp(name.c_str(), "BRIGHTNESS") == 0) {
			stat_score = calcBrightness(img);
		        float threshold = std::stof(imgstat.second);
			brightnessBox.update(stat_score);
			if (stat_score >= threshold && save_sample==true){
				std::string imagePath = filesSavePath;
                std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
          }
        } else if (strcmp(name.c_str(), "SHARPNESS") == 0) {
			stat_score = calcSharpness(img);
		        float threshold = std::stof(imgstat.second);
			sharpnessBox.update(stat_score);
			if (stat_score >= threshold && save_sample==true){
			    std::string imagePath = filesSavePath;
			    std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
			}
        } else if (strcmp(name.c_str(), "MEAN") == 0) {
		         cv::Scalar mean_values = cv::mean(img);
			 for (int i = 0; i < mean_values.rows; ++i) {
			      meanBox[i].update(mean_values[i]);	 
                         }    
        } else if (strcmp(name.c_str(), "CONTRAST") == 0) {
			stat_score = calcContrast(img);
		        float threshold = std::stof(imgstat.second);
			contrastBox.update(stat_score);
			if (stat_score >= threshold && save_sample==true){
			    std::string imagePath = filesSavePath;
			    std::string savedImagePath = saveImageWithIncrementalName(img, imagePath, baseName);
			}
        } else if (strcmp(name.c_str(), "HISTOGRAM") == 0) {
		std::function<void(const std::vector<int>&)> callback = [this](const std::vector<int>& pixelValues) {
            this->updatePixelValues(pixelValues); // Use the class's method as the callback
	    };
	    iterateImage(img, callback);
        }
    }
    return 1; // Indicate success
  }


// Function to iterate over an image and apply a callback for each pixel's values
void ImageProfile::iterateImage(const cv::Mat& img, const std::function<void(const std::vector<int>&)>& callback) {
    if (img.empty()) {
        throw std::runtime_error("Image is empty.");
    }

    int channels = img.channels();

    if (channels >= 1 && channels <= 4) {
        // Use cv::Vec with dynamic size based on the number of channels
        using VecType = cv::Vec<uchar, 4>;

        // Iterate over the image and extract values for each pixel
        for (auto it = img.begin<VecType>(); it != img.end<VecType>(); ++it) {
            const VecType& pixel = *it;

            // Collect only the relevant channel values based on the channel count
            std::vector<int> pixelValues(pixel.val, pixel.val + channels);

            // Apply the callback with the pixel values
            callback(pixelValues);
        }
    } else {
        throw std::runtime_error("Unsupported number of channels.");
    }
}

void ImageProfile::updatePixelValues(const std::vector<int>& pixelValues) {
     for (size_t i = 0; i < pixelValues.size(); ++i){
            pixelBox[i].update(pixelValues[i]);
    }
}
