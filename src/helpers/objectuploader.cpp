/**
 * @file ImageUploader.cpp
 * @brief Implementation file for the ImageUploader class
 */

#include "objectuploader.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/threading/PooledThreadExecutor.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <fstream>
#include <iostream>
#include <thread>

ImageUploader::ImageUploader(int uploadtype, const std::string& endpointUrl, const std::string& token,
                            const Aws::Auth::AWSCredentials& credentials, const Aws::String& region,
                            std::shared_ptr<Aws::Utils::Threading::Executor> executor) :
  credentials_(credentials), region_(region),
  stopFlag_(false){
    type = uploadtype;
    if (uploadtype == 0)
        _HttpUploader = new HttpUploader(endpointUrl, token);
    
  // Initialize AWS SDK (assuming already done elsewhere)
  // Aws::InitAPI(options);
}

/**
 * @brief Starts a thread that uploads the image to S3 in a loop at a specified interval
 * @param imagePath Path to the image file on local storage
 * @param bucketName Name of the S3 bucket where the image will be uploaded
 * @param objectKey Name of the object (filename) in the S3 bucket
 * @param interval Upload interval in milliseconds
 * @return true on success, false on error
 */
bool ImageUploader::startUploadThread(const std::string& imagePath, const std::string& bucketName,
                                       const std::string& objectKey, const std::chrono::milliseconds& interval) { 
        // Start the upload thread
        std::thread uploadThreadObj(&ImageUploader::uploadThread, this, imagePath, bucketName, objectKey, interval);
        uploadThreadObj.detach(); // Detach the thread to avoid resource leaks
    return true;
}

/**
 * @brief Stops the running upload thread
 */
void ImageUploader::stopUploadThread() {
  stopFlag_.store(true);
}

/**
 * @brief Function to run the upload thread in a separate thread
 * @param imagePath Path to the image file on local storage
 * @param bucketName Name of the S3 bucket where the image will be uploaded
 * @param objectKey Name of the object (filename) in the S3 bucket
 * @param interval Upload interval in milliseconds
 */
void ImageUploader::uploadThread(const std::string& imagePath, const std::string& bucketName,
                                  const std::string& objectKey, const std::chrono::milliseconds& interval) {
// Create S3 client
    Aws::S3::S3Client *s3Client;
//TODO:resolve error
//    if(type == 1) {
//        s3Client = new Aws::S3::S3Client(credentials_, Aws::Region::fromName(region_.c_str()));
//    }
  while (!stopFlag_.load()) {
    // Acquire lock to ensure thread-safe image upload
    uploadMutex_.lock();

    if (type == 1) {
        // Upload the image
//TODO: resolve error
//        if (!uploadImageToS3(imagePath, bucketName, objectKey, *s3Client)) {
//            std::cerr << "Failed to upload image in this iteration." << std::endl;
//        }
    } else  {
        _HttpUploader->postFile(imagePath, bucketName, time(NULL));
    }
    // Release lock and wait for the interval
    uploadMutex_.unlock();
    std::this_thread::sleep_for(interval);
  }
}

