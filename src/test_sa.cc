#include "utils.h"
#include "imitation.h"
#include "feature/common.h"
#include "feature/img_saliency.h"
#include "base/logging.h"
#include "feature/om.h"
#include <stdio.h>
#include <dirent.h>
#include <highgui.h>

const int LIB_NUM = 2;
const char *lib[] = {
                     "../img/",
                     "/mnt/db/sample/",
                     "/mnt/db/1/",
                     "/mnt/db/2/",
                     "/mnt/db/3/"};
const int VIDEO_LIB_NUM = 2;
const char *video_lib2[] = {"/mnt"};
const char *video_lib[] = {"/mnt/share/queryVideos",
                           "/mnt/share/libVideos/"};
const int kMaxSize = 1024 * 1024 * 3;
uint8 img_data[kMaxSize];
uint8 buff[1024 * 960 * 3];

void test_hc_method(const char *path, cv::Mat *res) {
    if (path == NULL) {
        return ;
    }

    cv::Mat _src = cv::imread(path);
    VLOG(0, "_src dim: %d", _src.dims);

    if (!_src.data) {
        fprintf(stderr, "no data, %s\n", path);
        return;
    }

    cv::Mat src;
//    cv::Mat roi = _src;
    cv::Mat img = _src;
    cv::Rect margin;
    remove_margin(img, &margin);

    cv::Mat roi(img, margin);
    resize_mat_by_width(roi, src, 320);
    show_mat(src);

    cv::Mat result;
    vcd::Saliency::Get(src, result, vcd::Saliency::RC);
    vcd::Saliency::Evaluate(result, result);

    if (!result.data) {
        return;
    }

    *res = result;
}

int test_video_saliency(uint8 *data, int w, int h) {
    // 1= cvt the yuv data to rgb
    cv::Mat img_rgb;
    if (vcd::cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
        return 1;
    }

    cv::Mat img = img_rgb;
    cv::Rect margin;
    remove_margin(img, &margin);

    // 2= extract the main rectangle on rgb image
//    cv::Mat roi(img, margin);
    cv::Mat roi = img;
    resize_mat_by_width(roi, roi, 320);
    cv::Rect rect;
    cv::Mat saliency_map;
    vcd::Saliency::Get(roi, saliency_map);    
    if (vcd::Saliency::ExtractView(saliency_map, rect) == false) {
        return 1;
    }

    draw_rectangle(roi, rect);
    show_mat(roi, 2);

    return 0;
}

void test_dir(const char *path) {
    vcd::Imitation imi;
    imi.OpenJpgDb(path);
    imi.GetVideo(test_video_saliency);
}

void test_single_pic(const char *path) {
    cv::Mat mat;
    test_hc_method(path, &mat);
    show_mat(mat);
}

int main(int argc, char **argv) {
    test_dir(argv[1]);
//    test_single_pic("../img/1.jpg");
//    for (int i = 0; i < LIB_NUM; ++i) 
//        test_dir(lib[i]);
//    for (int i = 0; i < VIDEO_LIB_NUM; ++i) {
//        test_dir(video_lib[i]);
//    }
}
