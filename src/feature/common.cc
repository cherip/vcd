#include "common.h"
#include <highgui.h>
#include <cv.h>

namespace vcd {

int get_ipl_data(const IplImage *src, int nChannel, uint8 *data) {
    int w = src->width;
    int h = src->height;
    int n = src->nChannels;
    int ws = src->widthStep;

    int idx = 0;
    for (int i = 0; i < h; ++i) {
        uint8 *ptr = reinterpret_cast<uint8*>(src->imageData + i * ws);
        for (int j = 0, c = nChannel; j < w; ++j, c += n) {
            data[idx++] = ptr[c];
        }
    }
//    printf("?? %d %d\n", src->nChannels, src->widthStep);
//    printf("?? %d %d %d\n", src->width, src->height, idx);

    return idx;
}

bool cvt_YUV2RGB(const uint8 *data_, int w, int h, cv::Mat *rgb) {
    uint8 *data = const_cast<uint8*>(data_);
    cv::Mat channel[4];

    cv::Mat yy(h, w, CV_8UC1, data, w);
    cv::Mat u(h / 2, w / 2, CV_8UC1, data + w * h, w / 2);
    cv::Mat v(h / 2, w / 2, CV_8UC1, data + w * h * 5 / 4, w / 2);
    cv::Mat alpa(h, w, CV_8UC1, cv::Scalar::all(0));

    channel[0] = yy;
    cv::resize(u, channel[1], yy.size(), cv::INTER_CUBIC);
    cv::resize(v, channel[2], yy.size(), cv::INTER_CUBIC);
    channel[3] = alpa;

    cv::Mat yuv;
    cv::merge(channel, 3, yuv);

    cv::cvtColor(yuv, *rgb, CV_YUV2BGR);
//    show_mat(*rgb);
    return true;
}

bool cvt_YUV2RGB(const uint8 *data_, int w, int h, IplImage *rgb) {
    IplImage *y = cvCreateImageHeader(cvSize(w, h), IPL_DEPTH_8U, 1); 
    IplImage *u = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1); 
    IplImage *v = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1); 

    IplImage *hu = cvCreateImageHeader(cvSize(w/2, h/2), IPL_DEPTH_8U, 1); 
    IplImage *hv = cvCreateImageHeader(cvSize(w/2, h/2), IPL_DEPTH_8U, 1); 


    uint8 *data = const_cast<uint8*>(data_);
    cvSetData(y, data, w);
    cvSetData(hu, data + w * h, w / 2);
    cvSetData(hv, data + static_cast<int>(w * h * 1.25), w / 2);

    cvResize(hu, u, CV_INTER_LINEAR);
    cvResize(hv, v, CV_INTER_LINEAR);

    cvMerge(y, u, v, NULL, rgb);
    cvCvtColor(rgb, rgb, CV_YUV2BGR);

    cvReleaseImage(&u);
    cvReleaseImage(&v);
    cvReleaseImageHeader(&y);
    cvReleaseImageHeader(&hu);
    cvReleaseImageHeader(&hv);

    return true;
}

bool cvt_RGB2YUV(const IplImage *src, uint8 *data, int nbuf, int *w, int *h) {
    // if data is length is short
    if (src->width * src->height * 3/2 >= nbuf) {
        return false;
    }

    IplImage *yuv_image = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U,
                                        src->nChannels);
    IplImage *comp_yy = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *comp_uu = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *comp_vv = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    cvCvtColor(src, yuv_image, CV_BGR2YUV);
    cvSplit(yuv_image, comp_yy, comp_uu, comp_vv, NULL);

    IplImage *comp_u = cvCreateImage(cvSize(src->width / 2, src->height / 2),
                                     IPL_DEPTH_8U, 1);
    IplImage *comp_v = cvCreateImage(cvSize(src->width / 2, src->height / 2),
                                     IPL_DEPTH_8U, 1);
    cvResize(comp_uu, comp_u);
    cvResize(comp_vv, comp_v);

    if (w != NULL) *w = src->width;
    if (h != NULL) *h = src->height;

    int offset = 0;
    offset = get_ipl_data(comp_yy, 0, data);
    offset += get_ipl_data(comp_u, 0, data + offset);
    offset += get_ipl_data(comp_v, 0, data + offset);
    
    cvReleaseImage(&yuv_image);
    cvReleaseImage(&comp_yy);
    cvReleaseImage(&comp_uu);
    cvReleaseImage(&comp_vv);
    cvReleaseImage(&comp_u);
    cvReleaseImage(&comp_v);

    return true;
}

uint64 get_simply_feature(const cv::Mat &_src) {
    int n = 8;
    int sw = _src.cols / n;
    int sh = _src.rows / n;

    uint64 feat = 0;
    cv::Mat src;
    cv::cvtColor(_src, src, CV_BGR2GRAY); 
    cv::Scalar whole_mean = cv::mean(src);
    //printf("total:%f\n", whole_mean.val[0]);
    for (int i = 0; i < n; i++) {
        for (int j =0; j < n; j++) {
            cv::Mat roi(src, cv::Rect(sw *j, sh *i, sw, sh));

            cv::Scalar me = cv::mean(roi); 
            //printf("%.2f ", me.val[0]);
            uint64 seed = 1;
            if (whole_mean.val[0] >= me.val[0] + 0.5) {
                feat = feat | (seed << (i *n + j));
            //    printf("1");
            } else {
            //    printf("0");
            }
        }
        //printf("\n");
    }
    //show_mat(src);
    return feat;
}

} // namespace vcd
