#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <QCoreApplication>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int matMult(Mat vec, int multType)
{
    return 0;
}

int main(int argc, char *argv[])
{

    Mat image, vertiGradient, gradient;
    char* filename = argc == 3 ? argv[1] : (char*)"./images/lines1.png";
    image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

    vertiGradient.create(image.rows, image.cols, 4);
    gradient.create(image.rows, image.cols, 4);

    cout<< "image information:"<< endl;
    cout<< "   rols: "<< image.rows<< endl;
    cout<< "   cols: "<< image.cols<< endl;

    namedWindow( "Origional image", WINDOW_AUTOSIZE);
    imshow( "Origional image", image );

    Mat row, rowVector, horizGradient;
    horizGradient.create(image.rows, image.cols, 4);

    // create a 1x3 vector for each vector multiplication.
    rowVector.create(1,3,4);

    for(int i = 0; i < image.rows; i++)
    {
        // for each rows in image do:
        row = image.row(i);
        for(int j = 0; j < image.cols; j++)
        {
            // for each pixel in this row do:


            // outer pixel will be filled with nearest inside pixel
            // which means edges like: 0 0 0 51 60 ....
            // horizonal gradient of edge 51 will be 60-51
            if(j == 0)
            {
                rowVector.at<int>(0,0) = row.at<int>(j);
            } else {
                rowVector.at<int>(0,0) = row.at<int>(j-1);
            }

            rowVector.at<int>(0,1) = row.at<int>(j);

            // otherside of edge
            if(j == image.cols - 1)
            {
                rowVector.at<int>(0,2) = row.at<int>(j);
            } else {
                rowVector.at<int>(0,2) = row.at<int>(j+1);
            }

            // vector multiplication
            horizGradient.at<int> (i, j)= matMult(rowVector, 0);
        }
    }

    waitKey(0);
    return 0;
}


