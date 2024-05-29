#include "saver.h"
#include <fstream>
#include "datatracer_log.h"

#include <kll_sketch.hpp>
#include <frequent_items_sketch.hpp>

typedef datasketches::kll_sketch<float> distributionBox;
typedef datasketches::frequent_items_sketch<std::string> frequent_class_sketch;
Saver::~Saver(){
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        while (!(objects_to_save_.empty())) {
          data_object_t *object = objects_to_save_.front();
          objects_to_save_.pop();
          delete object;
        }
    }
    StopSaving();
}
Saver::Saver(int interval, std::string class_name) {
    save_interval_ = interval;
    parent_name = class_name;
    exitSaveLoop.store(false);
}

void Saver::AddObjectToSave(void *object, int type, const std::string& filename) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  data_object_t *tmp_obj = new data_object_t;
  tmp_obj->obj = object;
  tmp_obj->type = type;
  tmp_obj->filename = filename;
  objects_to_save_.push(tmp_obj);
  cv_.notify_one(); // Notify the waiting thread about a new object
  log_info << parent_name << ": added " << filename << " into saver" << std::endl;
}

void Saver::StartSaving() {
  save_thread_ = std::thread(&Saver::SaveLoop, this);
  log_debug << parent_name << ": saver thread started" << std::endl;
}

// Trigger method is to asynchronously trigger the object save
void Saver::TriggerSave() {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  cv_.notify_one(); // Notify the waiting thread to process the queue manually
  log_debug << parent_name << ": save notification sent to saver thread" << std::endl;
}

void Saver::SaveLoop() {
  while (true) {
    do {

    if (exitSaveLoop.load()) {
      log_debug << parent_name << ": exited from saver thread" << std::endl;
      pthread_exit(nullptr);// Thread termination condition
    }

    std::unique_lock<std::mutex> lock(queue_mutex_);
    cv_.wait(lock, [&] { return !objects_to_save_.empty() || exitSaveLoop.load();}); // Wait for a new object or thread termination

    if (exitSaveLoop.load()) {
      log_debug << parent_name << ": exited from saver thread" << std::endl;
      pthread_exit(nullptr);// Thread termination condition
    }

    data_object_t *start_object = objects_to_save_.front();
    // Save only the first object in the queue and overwrite in the file
    do{
      data_object_t *object = objects_to_save_.front();
      SaveObjectToFile(object);

      // Rotate the queue by one element (circular approach)
      objects_to_save_.push(objects_to_save_.front());
      objects_to_save_.pop();
    }while(start_object != objects_to_save_.front());

    }while(0); //scope of queue_mutex_
    for (int i = 0; i < save_interval_; i++) {
        if (exitSaveLoop.load()) {
            log_debug << parent_name << ": exited from saver thread" << std::endl;
            pthread_exit(nullptr);// Thread termination condition
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

void Saver::SaveObjectToFile(data_object_t *object) {
    std::ofstream os(object->filename.c_str());
    try {
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
    } catch (const std::exception& e) {
            log_err << parent_name << " : Error saving file: " << e.what() << std::endl;
    }
}

void Saver::StopSaving(void) {
    if (save_thread_.joinable()) {
        exitSaveLoop.store(true);
        cv_.notify_one(); // Notify the waiting thread to process the queue manually
        save_thread_.join();
    }
}
