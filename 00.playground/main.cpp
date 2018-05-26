#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

const int slider_value_max = 360;
int slider_value;
char* filename;
Mat image_ori, image, image_with_center;
int rot_center_x, rot_center_y;

void drawFilledCircle(cv::Mat img, cv::Point center){
    int thickness = -1;
    int lineType = 8;
    int radius = 5;

    cv::circle(img,
               center,
               radius,
               cv::Scalar(0,0,255),
               thickness,
               lineType);
}

// -------------------------------track bars------------------------------------//

void angle_trackbar( int, void* )
{
    Point2f src_center(rot_center_x, rot_center_y);
    Mat rot_mat = getRotationMatrix2D(src_center, slider_value, 1.0);
    warpAffine(image_with_center, image, rot_mat, image_with_center.size());

    imshow("original image",image);
}

void center_x_trackbar( int, void* )
{
    image_with_center = image_ori.clone();

    setTrackbarPos("rotation angle", "original image", 0 );

    Point2f src_center(rot_center_x, rot_center_y);
    drawFilledCircle(image_with_center, src_center);

    imshow("original image",image_with_center);
}

void center_y_trackbar( int, void* )
{
    image_with_center = image_ori.clone();

    setTrackbarPos("rotation angle", "original image", 0 );

    Point2f src_center(rot_center_x, rot_center_y);
    drawFilledCircle(image_with_center, src_center);

    imshow("original image",image_with_center);
}



// ----------------------------------main_--------------------------------------//

int main(int argc, char *argv[])
{
    filename = argc == 3 ? argv[1] : (char*)"./images/badass2.jpg";
    image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
    image_ori = image.clone();
    image_with_center = image.clone();

    rot_center_x = image_ori.cols/2.0F;
    rot_center_y = image_ori.rows/2.0F;

    Point2f src_center(rot_center_x, rot_center_y);
    drawFilledCircle(image_with_center, src_center);

    imshow("original image",image_with_center);

    createTrackbar("rotation angle", "original image", &slider_value, slider_value_max, angle_trackbar );

    createTrackbar("rotation center x", "original image", &rot_center_x, image_ori.cols, center_x_trackbar );

    createTrackbar("rotation center y", "original image", &rot_center_y, image_ori.rows, center_y_trackbar );


    float abc = 1.345;
    cout<< "float: " << abc << endl;

    cout<< "int: " << (int)abc << endl;

    cout<< "diff: " << abc - (int)abc << endl;

    Mat rotation_matrix = (Mat_<float>(4,4)<<  1,  2,  3,  4,
                                               5,  6,  7,  8,
                                               9,  10, 11, 12,
                                               13, 14, 15, 16);

    cout<< "diff: " << rotation_matrix.at<float>(0,2) << endl;
    cout<< "diff: " << rotation_matrix.at<float>(Point2f(0,2)) << endl;


    float pos_lt = 163;
    float pos_lb = 165;
    float pos_rt = 165;
    float pos_rb = 166;


    // rotation matrix
    Mat around_matrix = (Mat_<float>(2,2)<<  pos_lt,  pos_rt,
                                             pos_lb,  pos_rb);
    float a1 = 1.429;
    float a2 = -0.429;

    Mat x_matrix = (Mat_<float>(1,2)<<  a1, a2);

    float b1 = 0.437;
    float b2 = 0.563;

    Mat y_matrix = (Mat_<float>(2,1)<<  b1, b2);

    Mat result1 = x_matrix * around_matrix;

    cout << "result: ( " << result1.at<float>(0,0) << " " << result1.at<float>(0,1) << ")" << endl;

    Mat result = result1 * y_matrix;

    float abcd = result.at<float>( 0, 0);

    cout << "abcd: " << abcd << endl;

    waitKey();

    return 0;
}

