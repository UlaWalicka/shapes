#include <opencv2\opencv.hpp>

using namespace cv;

void main(void)
{
	Mat obrazek = imread("lana.jpg");
	imshow("okienko", obrazek);
	waitKey();
}