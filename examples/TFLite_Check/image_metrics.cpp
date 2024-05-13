#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

class Image_metrics {
    public:
	Image_Metrics(Mat &img, std::vector<std::string> &metrics_list){
	        img_ = img;
                metricsList_ = metrics_list;
	}
    
    void Prepare_metrics(){
        for (const std::string& str : metricsList_){
	    if (str == "SNR")
	
	}
    
    } 	
    
}
