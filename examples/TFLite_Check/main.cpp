#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/examples/label_image/get_top_n.h"
#include "tensorflow/lite/model.h"
#include <frequent_items_sketch.hpp>
#include <ImageSampler.h>
#include <imageprofile.h>
#include <modelprofile.h>

namespace fs = std::__fs::filesystem;

std::vector<std::string> load_labels(const std::string& labels_file) {
    std::ifstream file(labels_file);
    if (!file.is_open()) {
        fprintf(stderr, "Unable to open label file\n");
        exit(-1);
    }
    std::string label_str;
    std::vector<std::string> labels;

    while (std::getline(file, label_str)) {
        if (!label_str.empty())
            labels.push_back(label_str);
    }
    file.close();
    return labels;
}

void run_inference_on_image(const std::string& imageFile, tflite::Interpreter* interpreter, const std::vector<std::string>& labels, const std::string& model_name, ImageProfile &image_profile, 
	       ModelProfile &model_profile, ImageSampler &image_sampler) {
    // Get Input Tensor Dimensions
    int input = interpreter->inputs()[0];
    auto height = interpreter->tensor(input)->dims->data[1];
    auto width = interpreter->tensor(input)->dims->data[2];
    auto channels = interpreter->tensor(input)->dims->data[3];

    // Load Input Image
    cv::Mat image;
    auto frame = cv::imread(imageFile);
    if (frame.empty()) {
        fprintf(stderr, "Failed to load image: %s\n", imageFile.c_str());
        return;
    }

    // Copy image to input tensor
    cv::resize(frame, image, cv::Size(width, height), cv::INTER_NEAREST);
    memcpy(interpreter->typed_input_tensor<unsigned char>(0), image.data, image.total() * image.elemSize());

    // Inference
    std::chrono::steady_clock::time_point start, end;
    start = std::chrono::steady_clock::now();
    interpreter->Invoke();
    end = std::chrono::steady_clock::now();
    auto inference_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Get Output
    int output = interpreter->outputs()[0];
    const TfLiteTensor* outputTensor = interpreter->tensor(output);

    // Get Output Shape
    int numDims = outputTensor->dims->size;
    std::vector<int> outputShape(numDims);
    for (int i = 0; i < numDims; ++i) {
        outputShape[i] = outputTensor->dims->data[i];
    }

    // Print Output
    if (numDims == 2) {
        int batchSize = outputShape[0];
        int numClasses = outputShape[1];

        for (int batch = 0; batch < batchSize; ++batch) {
            for (int cls = 0; cls < numClasses; ++cls) {
                float value = outputTensor->data.f[batch * numClasses + cls];
                std::cout << "Batch " << batch << ", Class " << cls << ": " << value << std::endl;
            }
        }
    } else {
        std::cerr << "Unsupported output tensor shape. Expected 2D tensor." << std::endl;
        return;
    }

    std::vector<std::pair<float, int>> top_results;
    float threshold = 0.01f;

    switch (interpreter->tensor(output)->type) {
    case kTfLiteInt32:
        tflite::label_image::get_top_n<float>(interpreter->typed_output_tensor<float>(0), outputShape[1], 10, threshold, &top_results, kTfLiteFloat32);
        break;
    case kTfLiteUInt8:
        tflite::label_image::get_top_n<uint8_t>(interpreter->typed_output_tensor<uint8_t>(0), outputShape[1], 10, threshold, &top_results, kTfLiteUInt8);
        break;
    default:
        fprintf(stderr, "Cannot handle output type\n");
        exit(-1);
    }

    // Print inference time on the image
    cv::putText(frame, "Inference Time: " + std::to_string(inference_time) + " ms", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);

    // Print labels with confidence on the image
    for (const auto& result : top_results) {
        const float confidence = result.first;
        const int index = result.second;
        std::string output_txt = "Label: " + labels[index] + " Confidence: " + std::to_string(confidence);
	std::cout << output_txt << std::endl;
        //cv::putText(frame, output_txt, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }

    std::cout << "profiling image profile" <<std::endl;
    image_profile.profile(frame, true);

    std::cout << "profiling model profile" << std::endl;
    model_profile.log_classification_model_stats(10.0, top_results);

    std::cout << "profiling samper" << std::endl;
    image_sampler.sample(top_results, image, true);

    // Display image
    //cv::imshow("Output", frame);
    //cv::waitKey(0);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <modelfile> <labelfile> <image_folder>\n", argv[0]);
        return -1;
    }
    const char* modelFileName = argv[1];
    const char* labelFile = argv[2];
    const char* imageFolder = argv[3];

    // Load Model
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile(modelFileName);
    if (!model) {
        fprintf(stderr, "Failed to load model\n");
        return -1;
    }

    // Initiate Interpreter
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
    if (!interpreter) {
        fprintf(stderr, "Failed to initiate the interpreter\n");
        return -1;
    }

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        fprintf(stderr, "Failed to allocate tensors\n");
        return -1;
    }

    // Configure the interpreter
    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(1);

    // Load Labels
    auto labels = load_labels(labelFile);

    ImageProfile image_profile("config.ini", 1, 4);
    ModelProfile model_profile("test_model", "config.ini", 1, 3);
    ImageSampler image_sampler("config.ini", 1);

    // Traverse image folder and run inference on each image
    for (const auto& entry : fs::directory_iterator(imageFolder)) {
        if (entry.is_regular_file() && (entry.path().extension() == ".JPEG" || entry.path().extension() == ".png")) {
            std::cout << "Processing image: " << entry.path().string() << std::endl;
            run_inference_on_image(entry.path().string(), interpreter.get(), labels, modelFileName,
			    image_profile, model_profile, image_sampler);
        }
    }

    return 0;
}

