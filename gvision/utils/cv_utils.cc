//
// Created by ryan on 30/01/2024.
//
#include "cv_utils.h"


using namespace cv;
namespace gvision {
    void cvPutText(cv::Mat &frame, cv::Point textOrg, std::string text) {

        // Font type (Hershey fonts available in OpenCV)
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;

        // Font scale factor that is multiplied by the font-specific base size
        double fontScale = 1.0;

        // Font color (in BGR format)
        cv::Scalar fontColor = cv::Scalar(255, 255, 255); // White

        // Thickness of the lines used to draw the text
        int thickness = 2;

        // Put the text on the image
        cv::putText(frame, text, textOrg, fontFace, fontScale, fontColor, thickness);

    }
    void
    drawPolygon(cv::Mat &frame, std::vector<cv::Point2f> points, const cv::Scalar &color, int thickness, bool scale) {
        // Convert the vector of points to a vector of vectors of points
        if (scale) {
            int frameWidth = frame.cols;
            int frameHeight = frame.rows;

            // Scale the points
            for (cv::Point2f &point: points) {
                point.x *= frameWidth;
                point.y *= frameHeight;
            }
        }
        std::vector<std::vector<cv::Point>> contours;
        contours.emplace_back(std::vector<cv::Point>(points.begin(), points.end()));

        // Draw the polygon
        cv::polylines(frame, contours, true, color, thickness);
    }

    void scalePolygon(std::vector<cv::Point2f> &points, int width, int height) {
        for (cv::Point2f &point: points) {
            point.x *= width;
            point.y *= height;
        }
    }

    cv::Rect2f getRectFromPoints(std::vector<cv::Point2f> points) {
        float tlx = 1.0;
        float tly = 1.0;
        float brx = 0.0;
        float bry = 0.0;
        for (auto pt: points) {
            if (pt.x < tlx) tlx = pt.x;
            if (pt.y < tly) tly = pt.y;
            if (pt.x > brx) brx = pt.x;
            if (pt.y > bry) bry = pt.y;
        }
        cv::Rect2f rect = cv::Rect2f(tlx,tly,brx-tlx,bry-tly);
        return rect;
    }

    cv::Point2f scalePoint2f(cv::Point2f point, int width, int height) {
        return {point.x * width, point.y * height};
    }

    cv::Rect2f scaleRect2f(cv::Rect2f r, int width, int height) {
        return {r.x * width, r.y * height, r.width * width, r.height * height};
    }

    bool cvCheckPointInPolygon(const cv::Point2f point, const std::vector<cv::Point2f> &points) {
        int crossings = 0;
        int count = points.size();

        for (int i = 0; i < count; i++) {
            cv::Point2f a = points[i];
            cv::Point2f b = points[(i + 1) % count];

            if (((a.y <= point.y) && (point.y < b.y)) ||
                ((b.y <= point.y) && (point.y < a.y))) {
                if (point.x < (b.x - a.x) * (point.y - a.y) / (b.y - a.y) + a.x) {
                    crossings++;
                }
            }
        }

        return (crossings % 2 != 0);
    }

// Function to resize image
    cv::Mat resizeImage(const cv::Mat &inputImage, int newWidth, int newHeight) {
        cv::Mat resizedImage;
        cv::resize(inputImage, resizedImage, cv::Size(newWidth, newHeight));
        return resizedImage;
    }

    bool resizeImage(const cv::Mat &inputImage, cv::Mat &outputImage, int newWidth, int newHeight) {
        if (inputImage.empty() || newWidth <= 0 || newHeight <= 0) {
            std::cerr << "Invalid input." << std::endl;
            return false;
        }

        double aspectRatio = static_cast<double>(inputImage.cols) / inputImage.rows;

        double targetAspectRatio = static_cast<double>(newWidth) / newHeight;
        int targetWidth, targetHeight;

        if (targetAspectRatio > aspectRatio) {
            // Calculate new width and height based on target height
            targetWidth = static_cast<int>(newHeight * aspectRatio);
            targetHeight = newHeight;
        } else {
            // Calculate new width and height based on target width
            targetWidth = newWidth;
            targetHeight = static_cast<int>(newWidth / aspectRatio);
        }

        // Resize the image while preserving aspect ratio
        cv::Mat resizedImage;
        cv::resize(inputImage, resizedImage, cv::Size(targetWidth, targetHeight));

        // Create output image with padding
        outputImage = cv::Mat::zeros(newHeight, newWidth, inputImage.type());

        // Calculate padding
        int offsetX = (newWidth - targetWidth) / 2;
        int offsetY = (newHeight - targetHeight) / 2;

        // Copy resized image to the center of the output image
        resizedImage.copyTo(outputImage(cv::Rect(offsetX, offsetY, resizedImage.cols, resizedImage.rows)));

        return true;
    }

    cv::Mat resizeWithPadding(const cv::Mat &inputImage, int targetWidth, int targetHeight) {
        cv::Mat outputImage(targetHeight, targetWidth, inputImage.type(), cv::Scalar(0));

        // Compute scale factors for resizing
        float widthRatio = static_cast<float>(targetWidth) / inputImage.cols;
        float heightRatio = static_cast<float>(targetHeight) / inputImage.rows;
        float scaleFactor = std::min(widthRatio, heightRatio);

        // Resize the image while preserving aspect ratio
        cv::Mat resizedImage;
        cv::resize(inputImage, resizedImage, cv::Size(), scaleFactor, scaleFactor, cv::INTER_LINEAR);

        // Calculate padding
        int xOffset = (targetWidth - resizedImage.cols) / 2;
        int yOffset = (targetHeight - resizedImage.rows) / 2;

        // Copy resized image to output image with padding
        resizedImage.copyTo(outputImage(cv::Rect(xOffset, yOffset, resizedImage.cols, resizedImage.rows)));

        return outputImage;
    }

    void cvDrawCircle(cv::Mat &frame, cv::Point2f center, int radius, const cv::Scalar &color, int thickness) {
        // Get the width and height of the frame
        int frameWidth = frame.cols;
        int frameHeight = frame.rows;

        // Scale the center coordinates
        center.x *= frameWidth;
        center.y *= frameHeight;

        // Draw the circle
        cv::circle(frame, center, radius, color, thickness);
    }

    void
    drawCircleWithName(cv::Mat &frame, cv::Point2f center, int radius, const std::string &name, const cv::Scalar &color,
                       int thickness) {
        // Get the width and height of the frame
        int frameWidth = frame.cols;
        int frameHeight = frame.rows;

        // Scale the center coordinates
        center.x *= frameWidth;
        center.y *= frameHeight;

        // Scale the radius

        // Draw the circle
        cv::circle(frame, center, radius, color, thickness);

        // Define the text position
        cv::Point textPosition(center.x - radius, center.y - radius - 10); // Adjust text position as needed

        // Draw the text
        cv::putText(frame, name, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, color, thickness);
    }


    float calculateIoU(const cv::Rect &rect1, const cv::Rect &rect2) {
        // Calculate coordinates of intersection rectangle
        int x1_int = std::max(rect1.x, rect2.x);
        int y1_int = std::max(rect1.y, rect2.y);
        int x2_int = std::min(rect1.x + rect1.width, rect2.x + rect2.width);
        int y2_int = std::min(rect1.y + rect1.height, rect2.y + rect2.height);

        // Calculate width and height of intersection rectangle
        int intersection_width = std::max(0, x2_int - x1_int);
        int intersection_height = std::max(0, y2_int - y1_int);

        // Calculate area of intersection
        int intersection_area = intersection_width * intersection_height;

        // Calculate area of each rectangle
        int area1 = rect1.width * rect1.height;
        int area2 = rect2.width * rect2.height;

        // Calculate union area
        int union_area = area1 + area2 - intersection_area;

        // Calculate IoU
        float iou = static_cast<float>(intersection_area) / area2;

        return iou;
    }

    float calculateDistance(cv::Point2f point1, cv::Point2f point2) {
        // Calculating distance
        return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2) * 1.0);
    }

    cv::Mat meanAxis0(const cv::Mat &src) {
        int num = src.rows;
        int dim = src.cols;

        // x1 y1
        // x2 y2

        cv::Mat output(1, dim, CV_32F);
        for (int i = 0; i < dim; i++) {
            float sum = 0;
            for (int j = 0; j < num; j++) {
                sum += src.at<float>(j, i);
            }
            output.at<float>(0, i) = sum / num;
        }

        return output;
    }


    cv::Mat elementwiseMinus(const cv::Mat &A, const cv::Mat &B) {
        cv::Mat output(A.rows, A.cols, A.type());

        assert(B.cols == A.cols);
        if (B.cols == A.cols) {
            for (int i = 0; i < A.rows; i++) {
                for (int j = 0; j < B.cols; j++) {
                    output.at<float>(i, j) = A.at<float>(i, j) - B.at<float>(0, j);
                }
            }
        }
        return output;
    }


    cv::Mat varAxis0(const cv::Mat &src) {
        cv::Mat temp_ = elementwiseMinus(src, meanAxis0(src));
        cv::multiply(temp_, temp_, temp_);
        return meanAxis0(temp_);

    }


    int matrixRank(const cv::Mat &M) {
        cv::Mat w, u, vt;
        cv::SVD::compute(M, w, u, vt);
        cv::Mat1b nonZeroSingularValues = w > 0.0001;
        int rank = cv::countNonZero(nonZeroSingularValues);
        return rank;
    }

//    Anthor:Jack Yu
    cv::Mat SimilarTransform(const cv::Mat &src, const cv::Mat &dst) {
        int num = src.rows;
        int dim = src.cols;
        cv::Mat src_mean = meanAxis0(src);
        cv::Mat dst_mean = meanAxis0(dst);
        cv::Mat src_demean = elementwiseMinus(src, src_mean);
        cv::Mat dst_demean = elementwiseMinus(dst, dst_mean);
        cv::Mat A = (dst_demean.t() * src_demean) / static_cast<float>(num);
        cv::Mat d(dim, 1, CV_32F);
        d.setTo(1.0f);
        if (cv::determinant(A) < 0) {
            d.at<float>(dim - 1, 0) = -1;

        }
        cv::Mat T = cv::Mat::eye(dim + 1, dim + 1, CV_32F);
        cv::Mat U, S, V;
        cv::SVD::compute(A, S, U, V);

        // the SVD function in opencv differ from scipy .


        int rank = matrixRank(A);
        if (rank == 0) {
            assert(rank == 0);

        } else if (rank == dim - 1) {
            if (cv::determinant(U) * cv::determinant(V) > 0) {
                T.rowRange(0, dim).colRange(0, dim) = U * V;
            } else {
                int s = d.at<float>(dim - 1, 0) = -1;
                d.at<float>(dim - 1, 0) = -1;

                T.rowRange(0, dim).colRange(0, dim) = U * V;
                cv::Mat diag_ = cv::Mat::diag(d);
                cv::Mat twp = diag_ * V; //np.dot(np.diag(d), V.T)
                cv::Mat B = cv::Mat::zeros(3, 3, CV_8UC1);
                cv::Mat C = B.diag(0);
                T.rowRange(0, dim).colRange(0, dim) = U * twp;
                d.at<float>(dim - 1, 0) = s;
            }
        } else {
            cv::Mat diag_ = cv::Mat::diag(d);
            cv::Mat twp = diag_ * V.t(); //np.dot(np.diag(d), V.T)
            cv::Mat res = U * twp; // U
            T.rowRange(0, dim).colRange(0, dim) = U * diag_ * V;
        }
        cv::Mat var_ = varAxis0(src_demean);
        float val = cv::sum(var_).val[0];
        cv::Mat res;
        cv::multiply(d, S, res);
        float scale = 1.0 / val * cv::sum(res).val[0];
        cv::Mat temp1 = T.rowRange(0, dim).colRange(0, dim) * src_mean.t();
        cv::Mat temp2 = scale * temp1;
        cv::Mat temp3 = dst_mean - temp2.t();
        T.at<float>(0, 2) = temp3.at<float>(0);
        T.at<float>(1, 2) = temp3.at<float>(1);
        T.rowRange(0, dim).colRange(0, dim) *= scale; // T[:dim, :dim] *= scale

        return T;
    }

    double cosineSimilarity(std::vector<float> A, std::vector<float> B) {
        double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
        for (unsigned int i = 0u; i < A.size(); ++i) {
            dot += A[i] * B[i];
            denom_a += A[i] * A[i];
            denom_b += B[i] * B[i];
        }
        return dot / (sqrt(denom_a) * sqrt(denom_b));
    }

    void drawFaceLandmarks(cv::Mat &image, std::vector<cv::Point2f> faceLandmarks) {
        cv::circle(image, cv::Point(faceLandmarks[0].x, faceLandmarks[0].y), 1, cv::Scalar(0, 0, 255), 4);
        cv::circle(image, cv::Point(faceLandmarks[1].x, faceLandmarks[1].y), 1, cv::Scalar(0, 255, 255), 4);
        cv::circle(image, cv::Point(faceLandmarks[2].x, faceLandmarks[2].y), 1, cv::Scalar(255, 0, 255), 4);
        cv::circle(image, cv::Point(faceLandmarks[3].x, faceLandmarks[3].y), 1, cv::Scalar(0, 255, 0), 4);
        cv::circle(image, cv::Point(faceLandmarks[4].x, faceLandmarks[4].y), 1, cv::Scalar(255, 0, 0), 4);
    }

    double distancePointToLine(const cv::Point &point, const cv::Point &linePoint1, const cv::Point &linePoint2) {
        // Calculate A, B, C parameters of the line equation Ax + By + C = 0
        double A = linePoint2.y - linePoint1.y;
        double B = linePoint1.x - linePoint2.x;
        double C = linePoint2.x * linePoint1.y - linePoint1.x * linePoint2.y;

        // Calculate distance using the formula
        double distance = std::abs(A * point.x + B * point.y + C) / std::sqrt(A * A + B * B);

        return distance;
    }

    void parsePolygon(std::vector<cv::Point2f> &points, const YAML::Node &pointsNode) {
        for (const auto &pointNode: pointsNode) {
            cv::Point2f point;
            point.x = pointNode[0].as<float>();
            point.y = pointNode[1].as<float>();
            points.push_back(point);
        }
    }

//
    bool isRedFrame(cv::Mat bgrFrame, int percentageThresh) {
        cv::Mat hsvFrame;
        cvtColor(bgrFrame, hsvFrame, cv::COLOR_BGR2HSV);

        // Define the HSV range for red color
        cv::Mat lowerRedMask, upperRedMask;
        inRange(hsvFrame, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lowerRedMask);
        inRange(hsvFrame, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upperRedMask);

        // Combine the masks
        cv::Mat redMask = lowerRedMask | upperRedMask;

        // Count the number of red pixels
        int redPixels = countNonZero(redMask);
        int totalPixels = bgrFrame.cols * bgrFrame.rows;

        // Calculate the percentage of red pixels
        double redPercentage = ((double) redPixels / totalPixels) * 100;
        // Check if the frame is predominantly red (e.g., more than 30% red pixels)
        return redPercentage > percentageThresh;
    }
    bool isBlueFrame(cv::Mat bgrFrame, int percentageThresh) {
        cv::Mat hsvFrame;
        cvtColor(bgrFrame, hsvFrame, cv::COLOR_BGR2HSV);

        // Define the HSV range for green color
        cv::Mat greenMask;
        inRange(hsvFrame, cv::Scalar(35, 100, 100), cv::Scalar(85, 255, 255), greenMask);

        // Count the number of green pixels
        int greenPixels = countNonZero(greenMask);
        int totalPixels = bgrFrame.cols * bgrFrame.rows;

        // Calculate the percentage of green pixels
        double greenPercentage = ((double) greenPixels / totalPixels) * 100;
        // Check if the frame is predominantly green (e.g., more than threshold % green pixels)
        return greenPercentage > percentageThresh;
    }
    bool isYellowFrame(cv::Mat bgrFrame, int percentageThresh) {
        cv::Mat hsvFrame;
        cvtColor(bgrFrame, hsvFrame, cv::COLOR_BGR2HSV);

        // Define the HSV range for red color
        cv::Mat yellowMask;
        inRange(hsvFrame, cv::Scalar(20, 100, 100), cv::Scalar(30, 255, 255), yellowMask);
        // Count the number of red pixels
        int yPixels = countNonZero(yellowMask);
        int totalPixels = bgrFrame.cols * bgrFrame.rows;

        // Calculate the percentage of red pixels
        double redPercentage = ((double) yPixels / totalPixels) * 100;
        // Check if the frame is predominantly red (e.g., more than 30% red pixels)
        return redPercentage > percentageThresh;

    }

// Function to check if a point q lies on the segment pr
    bool onSegment(Point2f p, Point2f q, Point2f r) {
        if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) {
            return true;
        }
        return false;
    }

// Function to find the orientation of the triplet (p, q, r)
// 0 -> p, q and r are collinear
// 1 -> Clockwise
// 2 -> Counterclockwise
    int orientation(Point2f p, Point2f q, Point2f r) {
        int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (val == 0) return 0;  // collinear
        return (val > 0) ? 1 : 2; // clock or counterclock wise
    }

// Function to check if two segments p1q1 and p2q2 intersect
    bool doIntersect(Point2f p1, Point2f q1, Point2f p2, Point2f q2) {
        // Find the four orientations needed for general and special cases
        int o1 = orientation(p1, q1, p2);
        int o2 = orientation(p1, q1, q2);
        int o3 = orientation(p2, q2, p1);
        int o4 = orientation(p2, q2, q1);

        // General case
        if (o1 != o2 && o3 != o4) {
            return true;
        }

        // Special cases
        // p1, q1 and p2 are collinear and p2 lies on segment p1q1
        if (o1 == 0 && onSegment(p1, p2, q1)) return true;

        // p1, q1 and q2 are collinear and q2 lies on segment p1q1
        if (o2 == 0 && onSegment(p1, q2, q1)) return true;

        // p2, q2 and p1 are collinear and p1 lies on segment p2q2
        if (o3 == 0 && onSegment(p2, p1, q2)) return true;

        // p2, q2 and q1 are collinear and q1 lies on segment p2q2
        if (o4 == 0 && onSegment(p2, q1, q2)) return true;

        // Doesn't fall in any of the above cases
        return false;
    }
// Function to calculate the direction vector between two points
    Point2f getDirectionVector(Point p, Point q) {
        return Point2f(q.x - p.x, q.y - p.y);
    }

// Function to normalize a vector
    Point2f normalizeVector(Point2f vec) {
        float magnitude = sqrt(vec.x * vec.x + vec.y * vec.y);
        return Point2f(vec.x / magnitude, vec.y / magnitude);
    }

// Function to check if two vectors are nearly the same (within a small tolerance)
    bool areVectorsSameDirection(Point2f vec1, Point2f vec2, float tolerance) {
        vec1 = normalizeVector(vec1);
        vec2 = normalizeVector(vec2);
        float dotProduct = vec1.x * vec2.x + vec1.y * vec2.y;
        std::cout<< "dot=" << abs(dotProduct - 1.0) << std::endl;
        return abs(dotProduct - 1.0) <= tolerance;
    }

// Function to check if the object is moving in the same direction as another line segment
    bool isMovingInSameDirection(Point2f p1, Point2f q1, Point2f p2, Point2f q2, float tolerance) {
        Point2f dir1 = getDirectionVector(p1, q1);
        Point2f dir2 = getDirectionVector(p2, q2);
        return areVectorsSameDirection(dir1, dir2,tolerance);
    }

    bool mergeVideos(const std::vector<std::string> &videoPaths, const std::string &outputFilePath) {
        if (videoPaths.empty()) {
            std::cerr << "No video files provided." << std::endl;
            return false;
        }

        cv::VideoWriter writer;
        int frameWidth = 0, frameHeight = 0, fps = 0;
        bool isWriterInitialized = false;

        for (const auto &videoPath: videoPaths) {
            cv::VideoCapture cap(videoPath);
            if (!cap.isOpened()) {
                std::cerr << "Error: Could not open video file: " << videoPath << std::endl;
                continue;
            }

            // Get video properties
            int currentWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
            int currentHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
            int currentFPS = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
            int fourcc = static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));

            // Initialize VideoWriter if not already initialized
            if (!isWriterInitialized) {
                frameWidth = currentWidth;
                frameHeight = currentHeight;
                fps = currentFPS;

                writer.open(outputFilePath, fourcc, fps, cv::Size(frameWidth, frameHeight));
                if (!writer.isOpened()) {
                    std::cerr << "Error: Could not open VideoWriter for file: " << outputFilePath << std::endl;
                    return false;
                }
                isWriterInitialized = true;
            }

            // Ensure all videos have the same resolution and FPS

            std::cout << "mergeVideos: "<< outputFilePath << " -> "<< videoPath << "\n";
            // Read and write frames from the current video
            cv::Mat frame;
            while (cap.read(frame)) {
                if (currentWidth != frameHeight || currentHeight != frameHeight ) {
                    cv::Mat f = resizeImage(frame,frameWidth,frameHeight);
                    writer.write(f);
                    f.release();
                } else {
                    writer.write(frame);
                }

            }
            cap.release();
        }
        std::cout << "mergeVideos: created " << outputFilePath << "successfully \n";
        return isWriterInitialized;
    }



}