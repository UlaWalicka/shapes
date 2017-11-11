/**
* Simple shape detector program.
* It loads an image and tries to find simple shapes (rectangle, triangle, circle, etc) in it.
* This program is a modified version of `squares.cpp` found in the OpenCV sample dir.
*/
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

/**
* Helper function to display text in the center of a contour
*/
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	Rect r = cv::boundingRect(contour);

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

int main()
{
	cv::Mat src;
	cv::Mat gray;
	cv::Mat bw;
	cv::Mat dst;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> approx;

	VideoCapture capture(0);
	int q;

	while (cvWaitKey(30) != 'q')
	{
		capture >> src;
		if (true)
		{
			// Convert to grayscale
			cv::cvtColor(src, gray, CV_BGR2GRAY);

			// Use Canny instead of threshold to catch squares with gradient shading
			blur(gray, bw, Size(3, 3));
			cv::Canny(gray, bw, 80, 240, 3);
			//cv::imshow("bw", bw);
			
			// Find contours
			cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

			src.copyTo(dst);

			for (int i = 0; i < contours.size(); i++)
			{
				// Approximate contour with accuracy proportional
				// to the contour perimeter
				cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

				// Skip small or non-convex objects 
				if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
					continue;

				if (approx.size() == 3)
				{
					setLabel(dst, "TRI", contours[i]);    // Triangles	
					
					//line(dst, approx[0], approx[1], cvScalar(0, 255, 0), 4);
				}
				else if (approx.size() >= 4 && approx.size() <= 6)
				{
					// Number of vertices of polygonal curve
					int vtc = approx.size();
					
					// Use the degrees obtained above and the number of vertices
					// to determine the shape of the contour
					if (vtc == 4) {
						setLabel(dst, "RECT", contours[i]);
						drawContours(dst, contours, i, cvScalar(255, 255, 0), 3);
					}
					else if (vtc == 5) {
						setLabel(dst, "PENTA", contours[i]);
						drawContours(dst, contours, i, cvScalar(0, 255, 0), 3);
					}
					else if (vtc == 6) {
						setLabel(dst, "HEXA", contours[i]);
						drawContours(dst, contours, i, cvScalar(255, 0, 0), 3);
					}
				}
				else
				{
					
					// Detect and label circles other way
					/*double area = cv::contourArea(contours[i]);
					cv::Rect r = cv::boundingRect(contours[i]);
					int radius = r.width / 2;

					if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
						std::abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.2)
						setLabel(dst, "CIR", contours[i]);
					*/
				}
				vector<Vec3f> circles;
				/// Apply the Hough Transform to find the circles
				HoughCircles(bw.clone(), circles, CV_HOUGH_GRADIENT, 1, bw.clone().rows / 8, 200, 100, 0, 0);

				/// Draw the circles detected
				for (size_t i = 0; i < circles.size(); i++)
				{
					Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
					int radius = cvRound(circles[i][2]);
					// circle center
					circle(dst, center, 3, Scalar(0, 255, 0), -1, 8, 0);
					// circle outline
					circle(dst, center, radius, Scalar(0, 0, 255), 3, 8, 0);
					setLabel(dst, "CIR", contours[i]);
				}
			}
			cv::imshow("src", src);
			cv::imshow("dst", dst);

		}
		else
		{
			break;
		}
	}

	return 0;
}