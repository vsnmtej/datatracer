#ifndef SAVER_H
#define SAVER_H

#include <mutex>
#include <thread>
#include <condition_variable>
//#include "MyObject.h" // Include your object header

template <typename T>
class Saver {
public:
  // Constructor to specify filename and save interval
  Saver(const std::string& filename, int save_interval_minutes);

  // Add an object to the queue for saving
  void AddObjectToSave(const T& object);

  // Start the background thread to save objects from the queue periodically
  void StartSaving();

  // Manual trigger to save all objects in the queue immediately
  void TriggerSave();

private:
  // Function to be executed in the background thread
  void SaveLoop();

  std::string filename_;          // Filename for saving
  int save_interval_minutes_;     // Interval between saves in minutes
  std::thread save_thread_;        // Thread object for saving
  std::mutex queue_mutex_;         // Mutex for queue access
  std::condition_variable cv_;     // Condition variable for thread synchronization
  std::queue<T> objects_to_save_;  // Queue of objects to be saved

  // Replace this function with your actual logic to save the object to a file
  void SaveObjectToFile(const T& object);
};

#endif // SAVER_H

