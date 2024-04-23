/**
 * @file ModelProfile.h
 * @brief Header file for ModelProfile class for logging model statistics
 */

#ifndef MODEL_STATS_H
#define MODEL_STATS_H

#include <string>
#include <vector>
#include <map>

// Assuming declarations for Saver, distributionBox, ClassificationResult, and YoloDetection

/**
 * @brief Class for managing and logging model statistics
 */
typedef datasketches::kll_sketch<unit> distributionBox ;
class ModelProfile {
public:
  /**
   * @brief Constructor to initialize ModelProfile object
   * @param model_id Unique identifier for the model
   * @param no_of_classes Number of classes the model predicts
   * @param saver Reference to a Saver object used for saving model statistics
   */
  ModelProfile(std::string model_id, unit8 no_of_classes, Saver<distributionBox>& saver);

  /**
   * @brief Logs statistics for a classification model
   * @param inference_latency Time taken for model inference
   * @param results Reference to the classification results
   * @return 0 on success, negative value on error
   */
  int log_classification_model_stats(float inference_latency, const ClassificationResult& results);

  /**
   * @brief Logs statistics for a YOLOv5 model
   * @param inference_latency Time taken for model inference
   * @param results Reference to the detection results from YOLOv5 model
   * @return 0 on success, negative value on error
   */
  int log_yolov5_model_stats(float inference_latency, const YoloDetections& results);

private:
  // Member variables (declarations only, definitions in .cpp file)
  std::string model_id_;
  unit8 no_of_classes_;
  std::vector<float> inference_latency_;
  std::vector<int> no_detections_per_image_;
  std::vector<unit8> frequent_class_;
  std::vector<double> objectnessbox_;
  std::map<unit8, distributionBox> model_classes_stat_;
};

#endif // MODEL_STATS_H
