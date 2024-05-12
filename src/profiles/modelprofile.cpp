#include "modelprofile.h"
#include "IniParser.h"

/**
 * @class ModelProfile
 * @brief Class for computing and managing various model statistics
 */

/**
 * @brief ModelProfile class constructor
 * @param model_id Unique identifier for the model
 * @param no_of_classes Number of classes the model predicts
 * @param saver Reference to a Saver object used for saving model statistics
 */
ModelProfile::ModelProfile(std::string model_id, std::string conf_path,
	       	int save_interval, int top_classes) {
    Aws::Auth::AWSCredentials credentials;
    Aws::String region;
    std::string bucketName;
    std::string objectKey;
    std::chrono::milliseconds interval;
    int uploadtype=1;
    std::string endpointUrl="";
    std::string token="";

  // Set member variables
  saver = new Saver(save_interval);
  model_id_ = model_id;
  IniParser parser;
  modelConfig = parser.parseIniFile(conf_path,
                          "model", "");
  filesSavePath = modelConfig["files"];
  top_classes_ = top_classes;
  sketch1 = new frequent_class_sketch(64);
  //saver.AddObjectToSave(sketch1, 
  // Initialize model_classes_stat map with empty distributionBox objects for each class
  for (int cls = 0; cls < top_classes_; ++cls) {
    distributionBox dBox(200);		  
    boxes.push_back(dBox);
    saver->AddObjectToSave((void *)(&(boxes[cls])), KLL_TYPE,
		    filesSavePath+model_id+std::to_string(cls)+".bin");  // Register with Saver for saving
  }
    saver->StartSaving();
    uploader = new ImageUploader(uploadtype, endpointUrl, token, credentials, region, nullptr);
    uploader->startUploadThread(filesSavePath, bucketName, objectKey, interval);
}


// Public accessor to get the number of distribution boxes
int ModelProfile::getNumDistributionBoxes() const {
        return boxes.size();
    }

// Optional: Public accessor to return a reference to a specific distribution box
const distributionBox& ModelProfile::getDistributionBox(unsigned int index) const {
    if (index >= boxes.size()) {
       throw std::out_of_range("Invalid index for distribution box");
    }
    return boxes[index];
}

/**
 * @brief Logs classification model statistics
 * @param inference_latency Time taken for model inference
 * @param results Reference to the classification results
 * @return 0 on success, negative value on error
 *
 * This function iterates through the provided results and logs statistics for the most frequent classes.
 * It updates the `model_classes_stat` map with scores for each class.
 */
int ModelProfile::log_classification_model_stats(float inference_latency __attribute__((unused)),
	       	const ClassificationResults& results) {
  for (auto it = results.begin(); it != results.end(); ++it) {
	// Logic for identifying frequent classes goes here
    int cls = it->first;
    float score = it->second;
    sketch1->update(std::to_string(cls));  // Placeholder for storing frequent class IDs
    boxes[cls].update(score);
    model_classes_stat_.emplace(cls, boxes[cls]);  // Update score statistics for each class
    }
  return 0; // Assuming successful logging, replace with error handling if needed
}

/**
 * @brief Logs YOLOv5 model statistics
 * @param inference_latency Time taken for model inference
 * @param results Reference to the detection results from YOLOv5 model
 * @return 0 on success, negative value on error
 *
 * This function logs inference latency, number of detections, objectness scores, and class frequencies for YOLOv5 model.
 * It updates the `model_classes_stat` map with scores for each detected class.
int ModelProfile::log_yolov5_model_stats(float inference_latency, YoloDetections& results) {
  inference_latency_.push_back(inference_latency);
  no_detections_per_image_.push_back(results.size());
  for (const auto& result : results) {
    objectnessbox_.update(objectness_score);  // Log objectness score (implementation assumed)
    frequency_class_.update(cls);        // Placeholder for storing frequent class IDs

    for (const auto& detection : detections) {  // Loop through detected classes (implementation assumed)
      model_classes_stat_[id].push_back(score); // Update score statistics for each detected class
    }
  }
  return 0; // Assuming successful logging, replace with error handling if needed
}
*/
