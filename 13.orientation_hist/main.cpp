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

    Mat image, horizGradient, vertiGradient, gradient;
    char* filename = argc == 3 ? argv[1] : (char*)"./images/lines1.png";
    image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

    cout<< "image information:"<< endl;
    cout<< "   rols: "<< image.rows<< endl;
    cout<< "   cols: "<< image.cols<< endl;

    namedWindow( "Origional image", WINDOW_AUTOSIZE);
    imshow( "Origional image", image );

    Mat row;
    for(int i = 0; i < image.rows; i++)
    {
        // for each rows in image do:
        row = image.row(i);
        for(int j = 0; j < image.cols; j++)
        {
            // for each pixel in this row do:

        }
    }


    waitKey(0);
    return 0;
}

