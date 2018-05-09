#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <QCoreApplication>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    QCoreApplication a(argc, argv);
    Mat image;

    // default image source
    String imageName( "../data/HappyFish.jpg" );

    // if image source is selected
    if( argc > 1)
    {
        imageName = argv[1];
    }

    // Read the file
    image = imread( imageName, IMREAD_COLOR );

    // Check for invalid input
    if( image.empty() )
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    // Create a window for display.
    namedWindow( "Display window", WINDOW_AUTOSIZE );

    // Show our image inside it.
    imshow( "Display window", image );

    // Wait for a keystroke in the window
    waitKey(0);
    return a.exec();
}


