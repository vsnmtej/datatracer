#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"

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
    // Get Model label and input image
    if (argc != 4)
    {
        fprintf(stderr, "Usage: TfliteClassification.exe modelfile labels image\n");
        exit(-1);
    }
    const char *modelFileName = argv[1];
    const char *labelFile = argv[2];
    const char *imageFile = argv[3];

    // Load Labels
    auto labels = load_labels(labelFile);

    // Load Image
    cv::Mat image;
    auto frame = cv::imread(imageFile);
    if (frame.empty())
    {
        fprintf(stderr, "Failed to load image\n");
        exit(-1);
    }

    // Load TensorFlow model
    tensorflow::Session* session;
    tensorflow::Status status = tensorflow::NewSession(tensorflow::SessionOptions(), &session);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        return 1;
    }

    // Read the protobuf file of the model
    tensorflow::GraphDef graph_def;
    status = tensorflow::ReadBinaryProto(tensorflow::Env::Default(), modelFileName, &graph_def);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        return 1;
    }

    // Add the graph to the session
    status = session->Create(graph_def);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        return 1;
    }

    // Get the input and output operations
    std::vector<std::string> input_names = {"input_1"};
    std::vector<std::string> output_names = {"dense_2/Softmax"};
    tensorflow::TensorShape input_shape({1, frame.rows, frame.cols, frame.channels()});

    // Create input tensor
    tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, input_shape);
    float* input_data = input_tensor.flat<float>().data();
    frame.convertTo(frame, CV_32FC3);
    cv::Mat normalized_image;
    cv::normalize(frame, normalized_image, 0, 1, cv::NORM_MINMAX);
    std::memcpy(input_data, normalized_image.data, normalized_image.total() * normalized_image.elemSize());

    // Run the model
    std::vector<tensorflow::Tensor> outputs;
    status = session->Run({{input_names[0], input_tensor}}, output_names, {}, &outputs);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        return 1;
    }

    // Process the output
    auto output = outputs[0].flat<float>();
    int num_classes = output.size();
    std::cout << "Number of classes: " << num_classes << std::endl;
    std::vector<std::pair<float, int>> top_results;
    for (int i = 0; i < num_classes; ++i) {
        top_results.emplace_back(output(i), i);
    }
    std::sort(top_results.begin(), top_results.end(), std::greater<std::pair<float, int>>());

    // Print top results
    int num_top_results = std::min(10, num_classes);
    for (int i = 0; i < num_top_results; ++i) {
        float confidence = top_results[i].first;
        int class_index = top_results[i].second;
        std::cout << "Class: " << labels[class_index] << ", Confidence: " << confidence << std::endl;
    }

    return 0;
}

