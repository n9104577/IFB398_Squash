#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <limits>
#include <numeric>
using namespace cv;
using namespace std;

// We need 4 corresponding 2D points(x,y) to calculate homography.
vector<Point2f> imageCourtTemplatePoints;      // Stores 4 points(x,y) of the Court Template. Here the four points are 4 corners of image.
vector<Point2f> imageCourtPlayersPoints;    // stores 4 points that the user clicks(mouse left click) in the main image.
vector<Point2f> court_Points;
								// Image containers for main and logo image
Mat imageCourtPlayers;
Mat imageCourtTemplate;
Mat imageCourtPlayersCopy;


// Function to add main image and transformed logo image and show final output.
// Icon image replaces the pixels of main image in this implementation.
void showFinal(Mat src1, Mat src2)
{

	Mat gray, gray_inv, src1final, src2final;
	cvtColor(src2, gray, CV_BGR2GRAY);
	threshold(gray, gray, 0, 255, CV_THRESH_BINARY);
	//adaptiveThreshold(gray,gray,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,5,4);
	bitwise_not(gray, gray_inv);
	src1.copyTo(src1final, gray_inv);
	src2.copyTo(src2final, gray);
	Mat finalImage = src1final + src2final;
	namedWindow("output", WINDOW_AUTOSIZE);
	imshow("output", finalImage);
	cvWaitKey(0);

}

// Here we get four points from the user with left mouse clicks.
// On 5th click we output the overlayed image.
void on_mouse(int e, int x, int y, int d, void *ptr)
{
	if (e == EVENT_LBUTTONDOWN)
	{
		if (imageCourtPlayersPoints.size() < 4)
		{

			imageCourtPlayersPoints.push_back(Point2f(float(x), float(y)));
			cout << x << " " << y << endl;
		}
		else
		{
			cout << " Calculating Homography " << endl;
			
			

			// Create a black image
			Mat black(imageCourtPlayersCopy.rows, imageCourtPlayersCopy.cols, imageCourtPlayersCopy.type(), cv::Scalar::all(0));
			// create a white mask
			Mat mask(imageCourtPlayersCopy.rows, imageCourtPlayersCopy.cols, CV_8UC1, cv::Scalar(0));

			// Get the coordiantes of the polygon (court area we want) from the mouse clicks
			vector< vector<Point> >  co_ordinates;
			co_ordinates.push_back(vector<Point>());
			co_ordinates[0].push_back(imageCourtPlayersPoints[0]);
			co_ordinates[0].push_back(imageCourtPlayersPoints[1]);
			co_ordinates[0].push_back(imageCourtPlayersPoints[2]);
			co_ordinates[0].push_back(imageCourtPlayersPoints[3]);
			drawContours(mask, co_ordinates, 0, Scalar(255), CV_FILLED, 8);
			black.copyTo(imageCourtPlayersCopy, mask);

			// create a new image with the mask on the court 
			Mat dst1;
			imageCourtPlayers.copyTo(dst1, mask);
			namedWindow("black", WINDOW_AUTOSIZE);
			imshow("black", imageCourtPlayers);


			// Do some stuff to and store the ROI in result 
			Mat dst2;
			//bitwise_not(mask, mask);
			dst1.copyTo(dst2, mask);
			Mat ROI_result = dst1 + dst2;

			namedWindow("black", WINDOW_AUTOSIZE);
			imshow("black", ROI_result);

		

			// Deactivate callback
			cv::setMouseCallback("Display window", NULL, NULL);
			// once we get 4 corresponding points in both images calculate homography matrix
			Mat H = findHomography(imageCourtPlayersPoints, imageCourtTemplatePoints, 0);
			Mat logoWarped;
			Mat HomoResult;

			warpPerspective(ROI_result, HomoResult, H, imageCourtTemplate.size());
			// Warp the logo image to change its perspective
			

			showFinal(imageCourtTemplate, HomoResult);
		}

	}
}


int main(int argc, char** argv)
{
	//  We need tow argumemts. "Main image" and "logo image"
	if (argc != 3)
	{
		cout << " Usage: error" << endl;
		return -1;
	}


	// Load images from arguments passed.
	imageCourtPlayers = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	imageCourtTemplate = imread(argv[2], CV_LOAD_IMAGE_COLOR);
	imageCourtPlayersCopy = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	// Push the 4 corners of the logo image as the 4 points for correspondence to calculate homography.
	imageCourtTemplatePoints.push_back(Point2f(float(0), float(0)));
	imageCourtTemplatePoints.push_back(Point2f(float(0), float(imageCourtTemplate.rows)));
	imageCourtTemplatePoints.push_back(Point2f(float(imageCourtTemplate.cols), float(imageCourtTemplate.rows)));
	imageCourtTemplatePoints.push_back(Point2f(float(imageCourtTemplate.cols), float(0)));



	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Display window", imageCourtPlayers);
	namedWindow("Display window1", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Display window1", imageCourtTemplate);

	setMouseCallback("Display window", on_mouse, NULL);


	//  Press "Escape button" to exit
	while (1)
	{
		int key = cvWaitKey(10);
		if (key == 27) break;
	}


	return 0;
}
