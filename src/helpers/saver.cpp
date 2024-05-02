#include "saver.h"
#include <fstream>

#include <kll_sketch.hpp>
#include <frequent_items_sketch.hpp>

typedef datasketches::kll_sketch<float> distributionBox;
typedef datasketches::frequent_items_sketch<std::string> frequent_class_sketch;

void Saver::AddObjectToSave(void *object, int type, const std::string& filename) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  data_object_t *tmp_obj = new data_object_t;
  tmp_obj->obj = object;
  tmp_obj->type = type;
  tmp_obj->filename = filename;
  objects_to_save_.push(tmp_obj);
  cv_.notify_one(); // Notify the waiting thread about a new object
}

void Saver::StartSaving() {
  save_thread_ = std::thread(&Saver::SaveLoop, this);
}

// Trigger method is to asynchronously trigger the object save
void Saver::TriggerSave() {
  std::lock_guard<std::mutex> lock(queue_mutex_);

  cv_.notify_one(); // Notify the waiting thread to process the queue manually
}

void Saver::SaveLoop() {
  while (true) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    cv_.wait(lock, [&] { return !objects_to_save_.empty() || !save_thread_.joinable(); }); // Wait for a new object or thread termination

    if (!save_thread_.joinable()) {
      break; // Thread termination condition
    }

    // Save only the first object in the queue and overwrite in the file
    if (!objects_to_save_.empty()) {
      data_object_t *object = objects_to_save_.front();
      SaveObjectToFile(object);

      // Rotate the queue by one element (circular approach)
      objects_to_save_.push(objects_to_save_.front());
      objects_to_save_.pop();
    }

    std::this_thread::sleep_for(std::chrono::minutes(save_interval_minutes_));
  }
}

void Saver::SaveObjectToFile(data_object_t *object) {
    std::ofstream os(object->filename.c_str());
    switch(object->type) {
        case KLL_TYPE:{
            distributionBox *obj = (distributionBox *)(object->obj);
            obj->serialize(os);
            break;
        }
        case FI_TYPE:{
            frequent_class_sketch *obj = (frequent_class_sketch *)(object->obj);
            obj->serialize(os);
            break;
        }
    }
}

