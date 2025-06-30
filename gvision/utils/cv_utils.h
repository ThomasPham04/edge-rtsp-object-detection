//
// Created by ryan on 30/01/2024.
//

#ifndef GSV_HUMAN_CV_UTILS_H
#define GSV_HUMAN_CV_UTILS_H

#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <unistd.h>
}
using namespace cv;
namespace gvision {

    void cvPutText(cv::Mat &frame, cv::Point textOrg, std::string text);

    void drawPolygon(cv::Mat &frame, std::vector<cv::Point2f> points,
                     const cv::Scalar &color = cv::Scalar{255, 0, 0},
                     int thickness = 2,
                     bool scale = true
    );
    cv::Rect2f getRectFromPoints( std::vector<cv::Point2f> points);
    void scalePolygon(std::vector<cv::Point2f> &points, int width, int height);

    cv::Point2f scalePoint2f(cv::Point2f point, int width, int height);

    cv::Rect2f scaleRect2f(cv::Rect2f point, int width, int height);

    bool cvCheckPointInPolygon(const cv::Point2f point, const std::vector<cv::Point2f> &points);

    cv::Mat resizeImage(const cv::Mat &inputImage, int newWidth, int newHeight);

    bool resizeImage(const cv::Mat &inputImage, cv::Mat &outputImage, int newWidth, int newHeight);

    cv::Mat resizeWithPadding(const cv::Mat &inputImage, int targetWidth, int targetHeight);

    void cvDrawCircle(cv::Mat &frame, cv::Point2f center, int radius, const cv::Scalar &color, int thickness);

    void
    drawCircleWithName(cv::Mat &frame, cv::Point2f center, int radius, const std::string &name, const cv::Scalar &color,
                       int thickness);


    float calculateIoU(const cv::Rect &rect1, const cv::Rect &rect2);

    float calculateDistance(cv::Point2f point1, cv::Point2f point2);

    cv::Mat meanAxis0(const cv::Mat &src);

    cv::Mat elementwiseMinus(const cv::Mat &A, const cv::Mat &B);

    cv::Mat varAxis0(const cv::Mat &src);

    int matrixRank(const cv::Mat &M);

    cv::Mat SimilarTransform(const cv::Mat &src, const cv::Mat &dst);

    double cosineSimilarity(std::vector<float> A, std::vector<float> B);

    void drawFaceLandmarks(cv::Mat &image, std::vector<cv::Point2f> faceLandmarks);

    double distancePointToLine(const cv::Point &point, const cv::Point &linePoint1, const cv::Point &linePoint2);

    void parsePolygon(std::vector<cv::Point2f> &points, const YAML::Node &pointsNode);

    bool isRedFrame(cv::Mat bgrFrame, int percentageThresh = 20);

    bool isYellowFrame(cv::Mat bgrFrame, int percentageThresh = 20);

    bool isBlueFrame(cv::Mat bgrFrame, int percentageThresh = 20);



    bool onSegment(Point2f p, Point2f q, Point2f r);

    int orientation(Point2f p, Point2f q, Point2f r);

    bool doIntersect(Point2f p1, Point2f q1, Point2f p2, Point2f q2);

    Point2f getDirectionVector(Point p, Point q);

    Point2f normalizeVector(Point2f vec);

    bool areVectorsSameDirection(Point2f vec1, Point2f vec2, float tolerance = 0.01);

    bool isMovingInSameDirection(Point2f p1, Point2f q1, Point2f p2, Point2f q2, float tolerance = 0.5);

    bool mergeVideos(const std::vector<std::string> &videoPaths, const std::string &outputFilePath);


}
#endif //GSV_HUMAN_CV_UTILS_H

