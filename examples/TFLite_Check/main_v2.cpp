#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/examples/label_image/get_top_n.h"
#include "tensorflow/lite/model.h"
#include <frequent_items_sketch.hpp>

std::vector<std::string> load_labels(std::string labels_file)
{
    std::ifstream file(labels_file.c_str());
    if (!file.is_open())
    {
        fprintf(stderr, "unable to open label file\n");
        exit(-1);
    }
    std::string label_str;
    std::vector<std::string> labels;

    while (std::getline(file, label_str))
    {
        if (label_str.size() > 0)
            labels.push_back(label_str);
    }
    file.close();
    return labels;
}

int main(int argc, char **argv)
{
    typedef datasketches::frequent_items_sketch<std::string> frequent_strings_sketch;
    frequent_strings_sketch sketch1(64);
    sketch1.update("a");
    sketch1.update("a");

    std::ofstream os1("freq_str_sketch1.bin");
    sketch1.serialize(os1);

    // Get Model label and input image
    if (argc != 4)
    {
        fprintf(stderr, "TfliteClassification.exe modelfile labels image\n");
        exit(-1);
    }
    const char *modelFileName = argv[1];
    const char *labelFile = argv[2];
    const char *imageFile = argv[3];

    // Load Model
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile(modelFileName);
    if (model == nullptr)
    {
        fprintf(stderr, "failed to load model\n");
        exit(-1);
    }
    // Initiate Interpreter
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model.get(), resolver)(&interpreter);
    if (interpreter == nullptr)
    {
        fprintf(stderr, "Failed to initiate the interpreter\n");
        exit(-1);
    }

    if (interpreter->AllocateTensors() != kTfLiteOk)
    {
        fprintf(stderr, "Failed to allocate tensor\n");
        exit(-1);
    }
    // Configure the interpreter
    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(1);
    // Get Input Tensor Dimensions
    int input = interpreter->inputs()[0];
    auto height = interpreter->tensor(input)->dims->data[1];
    auto width = interpreter->tensor(input)->dims->data[2];
    auto channels = interpreter->tensor(input)->dims->data[3];
    // Load Input Image
    cv::Mat image;
    auto frame = cv::imread(imageFile);
    if (frame.empty())
    {
        fprintf(stderr, "Failed to load image\n");
        exit(-1);
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
    int num_boxes = interpreter->tensor(output)->dims->data[1];
    float* bounding_boxes = interpreter->typed_output_tensor<float>(output);
    float* class_scores = interpreter->typed_output_tensor<float>(output);
    std::cout << "number of boxes " << num_boxes << std::endl;
    // Process the bounding boxes and class scores
    int output_box_tensor_index = -1;
    int output_score_tensor_index = -1;

    for (int i = 0; i < output.size(); ++i) {
	std::cout << "i " <<i << std::endl;
        TfLiteTensor* output_tensor = interpreter->tensor(output[i]);
        // Check the shape of the tensor
        if (output_tensor->dims->size == 2) {
            if (output_tensor->dims->data[1] == 4) {
                output_box_tensor_index = i;
            } else if (output_tensor->dims->data[1] == 1000) {
                output_score_tensor_index = i;
            }
        }
    }

    // Check if the tensors were found
    if (output_box_tensor_index == -1 || output_score_tensor_index == -1) {
        // Handle the case where the tensors were not found
        std::cerr << "Output tensors not found" << std::endl;
    } else {
        // You've found the indices of the output tensors
    }

    //std::cout << interpreter->outputs.size()  << std::endl;
    //std::vector<BoundingBox> boxes(num_boxes);

    //for (int i= 0; i < num_boxes; i++){
    //	float* box_data = output_tensors[1]->data + i*4;    
    //    std::cout << "Box " << i <<": " << box_data[0] << ", " << box_data[1] << ", "<< box_data[2] << ", " << box_data[3] << std::endl;
    //}

    TfLiteIntArray *output_dims = interpreter->tensor(output)->dims;
    auto output_size = output_dims->data[output_dims->size - 1];
    std::vector<std::pair<float, int>> top_results;
    float threshold = 0.01f;

    switch (interpreter->tensor(output)->type)
    {
    case kTfLiteInt32:
        tflite::label_image::get_top_n<float>(interpreter->typed_output_tensor<float>(0), output_size, 1, threshold, &top_results, kTfLiteFloat32);
        break;
    case kTfLiteUInt8:
        tflite::label_image::get_top_n<uint8_t>(interpreter->typed_output_tensor<uint8_t>(0), output_size, 1, threshold, &top_results, kTfLiteUInt8);
        break;
    default:
        fprintf(stderr, "cannot handle output type\n");
        exit(-1);
    }
    // Print inference ms in input image
    cv::putText(frame, "Infernce Time in ms: " + std::to_string(inference_time), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);

    // Load Labels
    auto labels = load_labels(labelFile);

    // Print labels with confidence in input image
    for (const auto &result : top_results)
    {
        const float confidence = result.first;
        const int index = result.second;
	std::cout << result.first << " " <<result.second << " " << std::endl;
        std::string output_txt = "Label :" + labels[index] + " Confidence : " + std::to_string(confidence);
        cv::putText(frame, output_txt, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }

    // Display image
    //cv::imshow("Output", frame);
    //cv::waitKey(0);

    return 0;
}
