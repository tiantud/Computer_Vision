// TU Dresden 2017, Inf, CV1 Ex4, Holger Heidrich
// Aufg 4, sampling theorem and correct filtering
// --
// a linear object (bar, stem, road, chair leg, ...)
// is imaged arbitrary somewhere in the middle of your (512 x 512) image
// its width is 0.1 ... 10 Pixel (selectable)
// show this image correctly filtered and sampled according to the sampling theorem
//-----------------------------------------------------------------------------
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>  //includes #include <complex>

#define EPS 6.0e-8         // Relative error, or absolute error near a zero of Ci(x).
#define EULER 0.57721566   // Euler's constant.
#define MAXIT 100          // Maximum number of iterations allowed.
#define PIBY2 1.5707963    // PI/2.
#define FPMIN 1.0e-30      // Close to smallest representable floating-point number.
#define TMIN 2.0           // Dividing line between using the series and continued fraction.
#define TRUE 1
#define ONE complex<double>(1.0,0.0)
#define C_i complex<double>(0.0,1.0)

using namespace cv;
using namespace std;

//-------------------------------------------------------------------
// init

const int slider_value_max = 360;                      // max angle for slider
int slider_value;                                      // current value from slider
char* filename;
Mat image_ori, image, image_with_center, rot_90;
int rot_center_x, rot_center_y;                        // rotation center
float pos_lt, pos_lb, pos_rt, pos_rb;                  // bilinear interpolation

//-------------------------------------------------------------------
// helper function to draw rotation center
// to display the center of rotation visually
void drawFilledCircle(cv::Mat img, cv::Point center){
    int thickness = -1;
    int lineType = 8;
    int radius = 3;

    cv::circle(img,
               center,
               radius,
               cv::Scalar(0,0,255),
               thickness,
               lineType);
}

//-------------------------------------------------------------------
// rotation

Point2f rotation_from(Point2f center, Point2f new_position, int angle){

    // angle from destination to the position before
    angle = (-1) * angle;

    // vector from center to destination
    float diff_x = new_position.x - center.x;
    float diff_y = new_position.y - center.y;

    // destination matrix
    Mat after_rot=(Mat_<float>(1,3)<<   diff_x,  diff_y,  1.0 );

    float si = sin( angle * CV_PI / 180 );
    float co = cos( angle * CV_PI / 180 );

    // rotation matrix
    Mat rotation_matrix = (Mat_<float>(3,3)<<  co, -si,   0,
                                               si,  co,   0,
                                                0,   0,   1);

    // get origional position
    Mat ori_position = after_rot * rotation_matrix;

//    cout<< "rotation from x: " << ori_position.at<float>(0,0) + center.x << "| y: " << ori_position.at<float>(0,1) + center.y << endl;
//    cout<< "to x: " << new_position.x << "| y: " << new_position.y << endl;
//    cout<< endl;

    // return ori_position(col, row)
    return Point2f(ori_position.at<float>(0,0) + center.x, ori_position.at<float>(0,1) + center.y);
}

void rotation(Mat src, Mat dst, Point2f center, int rot_angle){

    // initialize result position
    Point2f ori_pos(0,0);

    // for each pixel in result image do:
    for(int col = 0; col < dst.cols; col++)
    {
        for(int row = 0; row < dst.rows; row++)
        {
            // get the responce pixel from input image
            ori_pos = rotation_from( center, Point2f( col, row ), rot_angle);

            if((int)ori_pos.x < 512 && (int)ori_pos.x >= 0 && (int)ori_pos.y >= 0 && (int)ori_pos.y < 512)
            {
                // related pixel
                pos_lt = src.at<uchar>(Point2f((int)ori_pos.x, (int)ori_pos.y));
                pos_lb = src.at<uchar>(Point2f((int)ori_pos.x, (int)ori_pos.y + 1));
                pos_rt = src.at<uchar>(Point2f((int)ori_pos.x + 1, (int)ori_pos.y));
                pos_rb = src.at<uchar>(Point2f((int)ori_pos.x + 1, (int)ori_pos.y + 1));


                // matrix of related pixel
                Mat around_matrix = (Mat_<float>(2,2)<<  pos_lb,  pos_lt,
                                                         pos_rb,  pos_rt);


                // vectors in derection x
                Mat x_matrix = (Mat_<float>(1,2)<<  ((int)ori_pos.x + 1) - ori_pos.x,
                                                               ori_pos.x - (int)ori_pos.x);

                // vectors in derection y
                Mat y_matrix = (Mat_<float>(2,1)<<  ((int)ori_pos.y + 1) - ori_pos.y,
                                                               ori_pos.y - (int)ori_pos.y);

                // bilinear interpolation algo (wikipedia)
                Mat resout = x_matrix * around_matrix * y_matrix;

                // set result into result image
                dst.at<uchar>(Point2f( col, row )) = resout.at<float>(0, 0);
            } else {
                // for pixel from outside of origional image
                dst.at<uchar>(Point2f( col, row )) = 0;
            }
        }
    }
}



//-------------------------------------------------------------------
// trackbars

void angle_trackbar( int, void* )
{
    Point2f src_center(rot_center_x, rot_center_y);
    Mat rot_mat = getRotationMatrix2D(src_center, slider_value, 1.0);
    warpAffine(image_with_center, image, rot_mat, image_with_center.size());

//    rotation(image_with_center, image, src_center, slider_value);

    imshow("image",image);
}

void center_x_trackbar( int, void* )
{
    image_with_center = image_ori.clone();

    setTrackbarPos("rotation angle", "image", 0 );

    Point2f src_center(rot_center_x, rot_center_y);
    drawFilledCircle(image_with_center, src_center);

    imshow("image",image_with_center);
}

void center_y_trackbar( int, void* )
{
    image_with_center = image_ori.clone();

    setTrackbarPos("rotation angle", "image", 0 );

    Point2f src_center(rot_center_x, rot_center_y);
    drawFilledCircle(image_with_center, src_center);

    imshow("image",image_with_center);
}

//-------------------------------------------------------------------
// helper function that produces visible images from out of 0..255 range images
void Debugimg(Mat img, const String& filename = "")
{
   Mat gray_img(cvSize(img.rows, img.cols), CV_8U);
   double min_val, max_val;
   Point min_loc, max_loc;
   minMaxLoc(img, &min_val, &max_val, &min_loc, &max_loc);
   convertScaleAbs(img, gray_img, 255.0 / (max_val - min_val), -255.0*min_val / (max_val - min_val));
   if (filename.empty()) imwrite("debug.png", gray_img);
   else                  imwrite(filename, gray_img);
}

//-------------------------------------------------------------------
// helper function
void Normalize_function_plot(vector<double> *f, int lowest, int highest)
{
   double min_val, max_val;
   Point min_loc, max_loc;
   minMaxLoc(*f, &min_val, &max_val, &min_loc, &max_loc);
   for (int i = 0; i < (*f).size(); i++)
   {
      (*f)[i] *= (highest - lowest) / (max_val - min_val);
      (*f)[i] += -(highest - lowest) * min_val / (max_val - min_val) + lowest;
   }
}

//--------------------------------------------------------------
double sinc(double x)
{
   if (x == 0.0) return 1.0;
   return sin(CV_PI*x) / (CV_PI*x);
}

//--------------------------------------------------------------
// see https://en.wikipedia.org/wiki/Trigonometric_integral
//
//
void cisi(double x, double *ci, double *si)
{
   // adapted from http://apps.nrbook.com/empanel/index.html
   // 6.8.2, p 301

   int i, k, odd;
   double a, err, fact, sign, sum, sumc, sums, t, term;
   complex<double> h, b, c, d, del;
   t = fabs(x);
   if (t == 0.0) {      // Special case.
      *si = 0.0;
      *ci = -1.0 / FPMIN;
      return;
   }
   if (t > TMIN) {
      // Evaluate continued fraction by modied
      b = complex<double>(1.0, t); // Lentz's method (x5.2).
      c = complex<double>(1.0 / FPMIN, 0.0);
      d = h = complex<double>(1.0, 0.0) / b;
      for (i = 2; i <= MAXIT; i++) {
         a = -(i - 1)*(i - 1);
         b += complex<double>(2.0, 0.0);
         d = ONE / (a*d + b);
         c = b + complex<double>(a, 0.0) / c;
         del = c * d;
         h = h*del;
         if (fabs(del.real() - 1.0) + fabs(del.imag()) < EPS) break;
      }
      if (i > MAXIT) cout << "cf failed in cisi";
      h = complex<double>(cos(t), -sin(t)) * h;
      *ci = -h.real();
      *si = PIBY2 + h.imag();
   }
   else {
        // Evaluate both series simultaneously.
         if (t < sqrt(FPMIN)) {  // Special case: avoid failure of convergence test because of underflow.
               sumc = 0.0;
               sums = t;
         }
         else {
            sum = sums = sumc = 0.0;
            sign = fact = 1.0;
            odd = TRUE;
            for (k = 1; k <= MAXIT; k++) {
               fact *= t / k;
               term = fact / k;
               sum += sign*term;
               err = term / fabs(sum);
               if (odd) {
                  sign = -sign;
                  sums = sum;
                  sum = sumc;
               }
               else {
                  sumc = sum;
                  sum = sums;
               }
               if (err < EPS) break;
               odd = !odd;
            }
            if (k > MAXIT) cout << "maxits exceeded in cisi";
         }
         *si = sums;
         *ci = sumc + log(t) + EULER;
   }
   if (x < 0.0) *si = -(*si);
}
//--------------------------------------------------------------
void help()
{
   cout <<
      "\n// TU Dresden 2017, Inf, CV1 Ex4, Holger Heidrich\n"
      "// a linear object (bar, stem, road, chair leg, ...)\n"
      "// is imaged arbitrary somewhere in the middle of your (512 x 512) image\n"
      "// its width is 0.1 ... 10 Pixel (selectable)\n"
      "// show this image correctly filtered\n";
      "// and sampled according to the sampling theorem\n";
}

int main( int argc, char** argv )
{
    help();

   Mat barImg;
   Mat smoothedBarCross;

   barImg.create(512, 512, CV_8U);
   barImg = 0;
   smoothedBarCross = barImg.clone();

   float barWidth = 1.2;      // select this with slider
   float barLocation = 255.3; // select this with slider
   float rotAngle = 0.0;      // select this with slider
   double ci;

   std::vector<double> si_values(1000);
   si_values[0] = 1.0;
   for (int i = 0; i < 1000; i++)
   {
      cisi(i*0.1*CV_PI, &ci, &si_values[i]);
   }

   std::vector<double> sinc_values(1000);
   sinc_values[0] = 1.0;
   for (int i = 0; i < 1000; i++)
   {
      sinc_values[i] = sinc(i*0.1);
   }

   // show sinc
   Normalize_function_plot(&sinc_values, 20,220);
   Mat funcImg;
   funcImg.create(260, sinc_values.size(), CV_8U);
   funcImg = 0;
   Point CurvePoint_1 = Point(0, 0);
   Point CurvePoint_2;

   for (int i = 0; i<sinc_values.size(); i++)
   {
      CurvePoint_2 = Point(i, 260 - sinc_values[i]);
      line(funcImg, CurvePoint_1, CurvePoint_2, CV_RGB(255, 255, 255));  // lines between each point
      CurvePoint_1 = CurvePoint_2;
   }
//   imshow("Sinc !", funcImg);

   // show si
   Normalize_function_plot(&si_values, 20, 220);
   Mat SiImg;
   SiImg.create(260, si_values.size(), CV_8U);
   SiImg = 0;
   CurvePoint_1 = Point(0, 0);

   for (int i = 0; i<si_values.size(); i++)
   {
      CurvePoint_2 = Point(i, 260 - si_values[i]);
      line(SiImg, CurvePoint_1, CurvePoint_2, CV_RGB(255, 255, 255));  // lines between each point
      CurvePoint_1 = CurvePoint_2;
   }
//   imshow("Si !", SiImg);

   filename = argc == 3 ? argv[1] : (char*)"./images/img1.jpg";
   image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

//   imshow("origional image",image);

   image_ori = image.clone();
   image_with_center = image.clone();
   rot_90 = image.clone();

   rot_center_x = image_ori.cols/2.0F;
   rot_center_y = image_ori.rows/2.0F;

   Point2f src_center(rot_center_x, rot_center_y);
   drawFilledCircle(image_with_center, src_center);

   imshow("image",image_with_center);

   createTrackbar("rotation angle", "image", &slider_value, slider_value_max, angle_trackbar );

   createTrackbar("rotation center x", "image", &rot_center_x, image_ori.cols, center_x_trackbar );

   createTrackbar("rotation center y", "image", &rot_center_y, image_ori.rows, center_y_trackbar );

   rotation(image_with_center, rot_90, src_center, 85);

   imshow("90 rotation",rot_90);

   waitKey();

   return 0;
}
