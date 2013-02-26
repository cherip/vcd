#ifndef _IMG_SALIENCY_H_
#define _IMG_SALIENCY_H_

//#include <pair>
#include <vector>

namespace cv {
    class Mat;
};

namespace vcd {

typedef std::pair<float, int> CostfIdx;

class Saliency {
public:
    // Get saliency map of the giving src img
    static bool Get(const cv::Mat &src, cv::Mat &result);
    //static bool Evaluate(const std::string &src);

    static cv::Mat GetHC(const cv::Mat &img3f);

    static int Quantize(const cv::Mat &img3f, cv::Mat &idx1i, cv::Mat &_color3f,
                        cv::Mat &_color_num, double ratio = 0.95);

private:
    static bool GetHC(const cv::Mat &bin_color3f, const cv::Mat &weights1f,
                      cv::Mat &color_saliency);
    static bool SmoothSaliency(const cv::Mat &bin_color3f, cv::Mat &sal1d,
                               float delta,
                               const std::vector<std::vector<CostfIdx> > &similar);
};

} // namespace vcd

#endif
