#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <QCoreApplication>
#include <math.h>
#include <iostream>
#include <string>

#define _USE_MATH_DEFINES // for PI

using namespace cv;
using namespace std;

// calculate gradient for every pixel
int getGrad(int y, int x){
    int result;

    if(y == 0 && x == 0){
        // 500 is a unvalid value,
        // for (0,0) shoule not record any gradient.
        result = 500;
    } else if (x == 0) {
        // x == 0 and y != 0 means its a vertical gradient
        result = 90;
    } else {
        result = atan((float)y/(float)x) * 180 / M_PI;
    }

    return result;
}

int main(int argc, char *argv[])
{
    int grad, max = 0;
    Mat image, vertiGradientImage, horizGradientImage, gradientImage;

    // overall filter
    Mat overallMask=(Mat_<char>(3,3)<<  0,  1,  0,
                                       -1,  0,  1,
                                        0, -1,  0);

    Mat blu=(Mat_<char>(3,3)<<          1,  2,  1,
                                        2,  4,  2,
                                        1,  2,  1);

    // horizonal filter
    Mat horizMask=(Mat_<char>(1,3)<< -1,  0,  1);

    // vertical filter
    Mat vertiMask=(Mat_<char>(3,1)<<  1,
                                      0,
                                     -1);
    // result histalgram
    Mat gradientHisto(400,540,CV_8UC1, Scalar(0));

    // list of data in histalgram: 0-179 grad
    int histoList[180];
    std::fill_n(histoList, 180, 0); /// fill with default value

    // read image
//    char* filename = argc == 3 ? argv[1] : (char*)"./images/keyb7.jpg";
//    char* filename = argc == 3 ? argv[1] : (char*)"./images/lines1.png";
    char* filename = argc == 3 ? argv[1] : (char*)"./images/lines2.png";
    image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

    // blur image for better result
//    filter2D (image,image,CV_32F,blu);

    // calculate histal and vertical gradient
    filter2D (image,vertiGradientImage,CV_32F,vertiMask);

    filter2D (image,horizGradientImage,CV_32F,horizMask);

    // calculate overall gradient
    filter2D (image,gradientImage,CV_32F,overallMask);

    // calculate data in the gradient-mount list
    for(int row = 0; row < image.rows; row++)
    {
        for(int col = 0; col < image.cols; col++)
        {
            // for each pixel do:

            // calculate gradient
            grad = getGrad((int)vertiGradientImage.at<float>(Point(col,row)), (int)horizGradientImage.at<float>(Point(col,row)));

            // record gradient into gradient-mount list
            if(grad < 400)
            {
                // if gradient is valid
                histoList[grad + 89] += 1;
            }

            // get maximun of mount in list, will be used for normalisation of hight
            if(histoList[grad + 89] > max)
            {
                max = histoList[grad + 89];
            }
        }
    }

    // draw histaldiagram with list of data
    for(int col = 0; col < gradientHisto.cols; col++){

        // normalisation of hight
        int h = histoList[col/3] * (gradientHisto.rows-1) / max;

        // draw lines
        line( gradientHisto, Point(col, gradientHisto.rows-1) ,
                             Point(col, gradientHisto.rows-1 - h),
                               Scalar( 255, 0, 0), 1, 8, 0  );
    }

    // show images
    imshow("vertiGradientImage",vertiGradientImage);
    imshow("horizGradientImage",horizGradientImage);
    imshow("gradientImage",gradientImage);
    imshow("gradientHisto",gradientHisto);


    waitKey(0);
    return 0;
}


