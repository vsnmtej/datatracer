#include "saver.h"
#include <fstream>

#include <kll_sketch.hpp>
#include <frequent_items_sketch.hpp>

typedef datasketches::kll_sketch<float> distributionBox;
typedef datasketches::frequent_items_sketch<std::string> frequent_class_sketch;

Saver::Saver(int interval) {
    save_interval_minutes_ = interval;
}

void Saver::AddObjectToSave(void *object, int type, const std::string& filename) {
      std::cout << __func__ << ":" << __LINE__ << ":" << filename << std::endl;
  std::lock_guard<std::mutex> lock(queue_mutex_);
  data_object_t *tmp_obj = new data_object_t;
  std::cout << filename << std::endl;
  tmp_obj->obj = object;
  tmp_obj->type = type;
  tmp_obj->filename = filename;
  objects_to_save_.push(tmp_obj);
  cv_.notify_one(); // Notify the waiting thread about a new object
      std::cout << __func__ << ":" << __LINE__ << ":" << (long long int)object << std::endl;
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
    do {

    std::cout << __func__ << ":" << __LINE__ << std::endl;
    std::unique_lock<std::mutex> lock(queue_mutex_);
    cv_.wait(lock, [&] { return !objects_to_save_.empty() || !save_thread_.joinable(); }); // Wait for a new object or thread termination

    std::cout << __func__ << ":" << __LINE__ << std::endl;
    if (!save_thread_.joinable()) {
      break; // Thread termination condition
    }

    std::cout << __func__ << ":" << __LINE__ << std::endl;
    data_object_t *start_object = objects_to_save_.front();
    // Save only the first object in the queue and overwrite in the file
    do{
      data_object_t *object = objects_to_save_.front();
      std::cout << __func__ << ":" << __LINE__ << std::endl;
      SaveObjectToFile(object);
      std::cout << __func__ << ":" << __LINE__ << std::endl;

      // Rotate the queue by one element (circular approach)
      objects_to_save_.push(objects_to_save_.front());
      std::cout << __func__ << ":" << __LINE__ << std::endl;
      objects_to_save_.pop();
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    }while(start_object != objects_to_save_.front());
      std::cout << __func__ << ":" << __LINE__ << std::endl;

    }while(0); //scope of queue_mutex_
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    std::this_thread::sleep_for(std::chrono::minutes(save_interval_minutes_));
      std::cout << __func__ << ":" << __LINE__ << std::endl;
  }
}

void Saver::SaveObjectToFile(data_object_t *object) {
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    std::ofstream os(object->filename.c_str());
      std::cout << __func__ << ":" << __LINE__ << ":" << object->filename << std::endl;
    try {
    switch(object->type) {
        case KLL_TYPE:{
      std::cout << __func__ << ":" << __LINE__ << std::endl;
            distributionBox *obj = (distributionBox *)(object->obj);
      std::cout << __func__ << ":" << __LINE__ << ":" << (long long int)obj << std::endl;
            obj->serialize(os);
      std::cout << __func__ << ":" << __LINE__ << std::endl;
            break;
        }
        case FI_TYPE:{
            frequent_class_sketch *obj = (frequent_class_sketch *)(object->obj);
            obj->serialize(os);
            break;
        }
    }
    } catch (const std::exception& e) {
            std::cerr << "Error saving file: " << e.what() << std::endl;
    }
      std::cout << __func__ << ":" << __LINE__ << std::endl;
}

void Saver::StopSaving(void) {
    if (save_thread_.joinable()) {
        save_thread_.detach();
    }
}
