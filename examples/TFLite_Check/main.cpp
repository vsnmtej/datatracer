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
#include <ImageSampler.h>
#include <imageprofile.h>
#include <modelprofile.h>

/* 
#include "datatracr/image/profile.h"
#include "datatracr/model/inference_profile.h"
#include "datatracr/model/activation_profile.h"
#include "datatracr/data/sampling.h"
*/

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

    std::string model_name = "Classify_123";
    uint16_t no_classes = 20;
    
    //imageProfile<int> pImg(DT.CH_3, DT.IMG_TYP_RGB, {DT.METRICS_ALL});
    //modelProfile<float> pModel(model_name, no_classes, DT.CLASSIFICATION);

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
    //pImg.log(&image);
    // Copy image to input tensor
    cv::resize(frame, image, cv::Size(width, height), cv::INTER_NEAREST);
    memcpy(interpreter->typed_input_tensor<unsigned char>(0), image.data, image.total() * image.elemSize());

    // Inference
    std::chrono::steady_clock::time_point start, end;
    start = std::chrono::steady_clock::now();
    interpreter->Invoke();
    end = std::chrono::steady_clock::now();
    auto inference_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    int _out = interpreter->outputs()[0];
    TfLiteIntArray *_out_dims = interpreter->tensor(_out)->dims;
    int _out_row   = _out_dims->data[1];   // 25200
    int _out_colum = _out_dims->data[2];

    // Get Output
    int output = interpreter->outputs()[0];
    const TfLiteTensor* outputTensor = interpreter->tensor(output);
    //std::vector<std::vector<float>> predV = tensorToVector2D(outputTensor, _out_row, _out_colum); 

    int numDims = outputTensor->dims->size;
    std::vector<int> outputShape(numDims);
    for (int i = 0; i < numDims; ++i) {
        outputShape[i] = outputTensor->dims->data[i];
    }
    

    if (numDims == 2) {
        int batchSize = outputShape[0];
        int numClasses = outputShape[1];

        // Loop through the output tensor and print its values
        for (int batch = 0; batch < batchSize; ++batch) {
            for (int cls = 0; cls < numClasses; ++cls) {
                float value = outputTensor->data.f[batch * numClasses + cls];
                std::cout << "Batch " << batch << ", Class " << cls << ": " << value << std::endl;
            }
        }
    } else {
        std::cerr << "Unsupported output tensor shape. Expected 2D tensor." << std::endl;
        return 1;
    }

    int num_boxes = interpreter->tensor(output)->dims->data[1];
    float* bounding_boxes = interpreter->typed_output_tensor<float>(output);
    float* class_scores = interpreter->typed_output_tensor<float>(output);
    std::cout << "number of boxes " << num_boxes << std::endl;
    // Process the bounding boxes and class scores
    int output_box_tensor_index = -1;
    int output_score_tensor_index = -1;

    // Check if the tensors were found
    if (output_box_tensor_index == -1 || output_score_tensor_index == -1) {
        // Handle the case where the tensors were not found
        std::cerr << "Output tensors not found" << std::endl;
    } else {
        // You've found the indices of the output tensors
    }

    TfLiteIntArray *output_dims = interpreter->tensor(output)->dims;
    auto output_size = output_dims->data[output_dims->size - 1];
    std::vector<std::pair<float, int>> top_results;
    float threshold = 0.01f;

    switch (interpreter->tensor(output)->type)
    {
    case kTfLiteInt32:
        tflite::label_image::get_top_n<float>(interpreter->typed_output_tensor<float>(0), output_size, 10, threshold, &top_results, kTfLiteFloat32);
        break;
    case kTfLiteUInt8:
        tflite::label_image::get_top_n<uint8_t>(interpreter->typed_output_tensor<uint8_t>(0), output_size, 10, threshold, &top_results, kTfLiteUInt8);
        break;
    default:
        fprintf(stderr, "cannot handle output type\n");
        exit(-1);
	break;
    }
    // Print inference ms in input image
    cv::putText(frame, "Inference Time in ms: " + std::to_string(inference_time), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);

    // Load Labels
    auto labels = load_labels(labelFile);

    /* Print labels with confidence in input image
    for (const auto &result : top_results)
    {
        const float confidence = result.first;
        const int index = result.second;
	std::cout << result.first << " " <<result.second << " " << std::endl;
        std::string output_txt = "Label :" + labels[index] + " Confidence : " + std::to_string(confidence);
        cv::putText(frame, output_txt, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }*/

    int save_interval = 2000; // seconds
    //Saver<distributionBox>& saver = Saver<distributionBox>::GetInstance(save_interval);
    ///////////////Profile Image///////////////////////////////////////////////////////
    std::cout << " entering image profile " << std::endl;
    ImageProfile image_profile("config.ini", 1, 4);
    std::cout << "profiling image profile" <<std::endl;
    image_profile.profile(frame, true);
    //imgstats.profile(&img, sample=true);
    ////////////////////////////////////////////////////////////////////////////////////    
    
    ///////////////////Log Model Metrics/////////////////////////////////////////
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    ModelProfile modelprofile("test_model", "config.ini", 1, 3);
    std::cout << "Logging classification models" <<std::endl;
    modelprofile.log_classification_model_stats(10.0, top_results);
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    //modelstats(&outputTensor);
    ///////////////// Log Uncertainity Samples /////////////////////////////////
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    ImageSampler img_sampler("config.ini", 1);
    img_sampler.sample(top_results, image, true);
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    //std::vector<std::pair<int, bool>> uncertainity_sampling;
    //img_sampler.sampling(&top_results, &uncertainity_sampling);
    ////////////////////////////////////////////////////////////////////////////
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    std::chrono::seconds dura( 5);
    std::this_thread::sleep_for( dura ); 
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    //////////////////////////////////// Custom Log /////////////////////////////////
    //custom_log(x);


    //modelprofile(&outputTensor);
    //uncertainity_sampling(&top_results);
    //custom_log(x);

    // Display image
      std::cout << __func__ << ":" << __LINE__ << std::endl;
    cv::imshow("Output", frame);
    cv::waitKey(0);
      std::cout << __func__ << ":" << __LINE__ << std::endl;

    return 0;
}
