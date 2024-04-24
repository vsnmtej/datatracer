#include "saver.h"
#include <fstream>


template <typename T>
void Saver<T>::AddObjectToSave(const T& object, const std::string& filename) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  objects_to_save_.emplace_back(object, filename);  
  cv_.notify_one(); // Notify the waiting thread about a new object
}

template <typename T>
void Saver<T>::StartSaving() {
  save_thread_ = std::thread(&Saver<T>::SaveLoop, this);
}

// Trigger method is to asynchronously trigger the object save
template <typename T>
void Saver<T>::TriggerSave() {
  std::lock_guard<std::mutex> lock(queue_mutex_);

  cv_.notify_one(); // Notify the waiting thread to process the queue manually
}

template <typename T>
void Saver<T>::SaveLoop() {
  while (true) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    cv_.wait(lock, [&] { return !objects_to_save_.empty() || !save_thread_.joinable(); }); // Wait for a new object or thread termination

    if (!save_thread_.joinable()) {
      break; // Thread termination condition
    }

    // Save only the first object in the queue and overwrite in the file
    if (!objects_to_save_.empty()) {
      T object = objects_to_save_.front().first;
      std::string filename = objects_to_save_.front().second;  // Access filename
      SaveObjectToFile(object, filename);

      // Rotate the queue by one element (circular approach)
      objects_to_save_.push(objects_to_save_.front());
      objects_to_save_.pop();
    }

    std::this_thread::sleep_for(std::chrono::minutes(save_interval_minutes_));
  }
}


template <typename T>
void Saver<T>::SaveObjectToFile(const T& object, const std::string& filename_) {
  std::ofstream os(filename_);
  object.serialize(os);
}

