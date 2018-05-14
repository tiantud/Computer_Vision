#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <QCoreApplication>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{

    Mat image;
    char* filename = argc == 3 ? argv[1] : (char*)"./images/lines1.png";
    image = imread(filename, CV_LOAD_IMAGE_COLOR);

    namedWindow( "Origional image", WINDOW_AUTOSIZE);
    imshow( "Origional image", image );



    waitKey(0);
    return 0;
}

