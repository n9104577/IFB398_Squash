#define _CRT_SECURE_NO_WARNINGS

//C++
#include <iostream>
#include <sstream>
//OpenCV Libraries
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

//Constants 
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

const int MIN_OBJECT_AREA = 3000;

//Slider Control
const bool usingMorph = true;

//Varibles
Ptr<BackgroundSubtractor> model;
//BackgroundSubtractorMOG2 * model;
Mat fgmask;

int D_MIN = 1;
int D_MAX = 30;
int E_MIN = 1;
int E_MAX = 30;

const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string trackbarWindowMorph = "Morph";

//Stubs
void onTrackbar(int, void*);

void onTrackbar(int, void*) {
	//No settings Change
}

void createTrackbarMorph(void) {
	namedWindow(trackbarWindowMorph, WINDOW_AUTOSIZE);
	char TrackbarName[50];

	//Set Trackbar Names
	sprintf(TrackbarName, "D_MIN", D_MIN);
	sprintf(TrackbarName, "E_MIN", E_MIN);

	//Create GUI  
	createTrackbar("Dialate", trackbarWindowMorph, &D_MIN, D_MAX, onTrackbar);
	createTrackbar("Erorde", trackbarWindowMorph, &E_MIN, E_MAX, onTrackbar);
}

void morphFrame(Mat &Pframe) {
	//Initalise
	int dElement = 6;
	int eElement = 4;
	
	//Set Values
	if (usingMorph) {
		dElement = D_MIN;
		eElement = E_MIN;	
	}

	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(dElement, dElement));
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(eElement, eElement));

	erode(Pframe, Pframe, erodeElement);
	dilate(Pframe, Pframe, dilateElement);
}

void trackFilteredObject(Mat PImage, Mat &frame) {
	int x = 0;
	int y = 0;
	Mat temp;
	PImage.copyTo(temp);

	//Contours and Hierarchy
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Find Blobs 
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		for (int index = 0; index >= 0; index = hierarchy[index][0]) {
			Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;
			//cout << area << endl;
			if (area > MIN_OBJECT_AREA) {
				x = moment.m10 / area;
				y = moment.m01 / area;
				circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
			}
		}
	}
}

int main(void) {
	//File Paths
	string file = "squash.mp4";
	string bg = "Background.mp4";

	//Varialbes
	model = createBackgroundSubtractorMOG2().dynamicCast<BackgroundSubtractor>();
	Mat frame, Pframe, background;

	//Footage
	VideoCapture cap(file);
	

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video file" << endl;
		return -1;
	}
	
	//Create Trackbar or Morph
	if (usingMorph) createTrackbarMorph();

	while (1) {
		// Catches Current Frame
		cap.read(frame);
		if (frame.empty()) break;

		//Resize Frame
		resize(frame, frame, Size(769, 576));

		//Apply Mask
		model->apply(frame, Pframe);
		//threshold(Pframe, Pframe, 174, 276, THRESH_BINARY);

		//Apply Morphological Functions
		morphFrame(Pframe);
		
		//Track Filtered Objects
		trackFilteredObject(Pframe, frame);

		// Display frame
		imshow("Frame", frame);
		imshow("Binary", Pframe);
		//imshow("Background", background);

		// Press  ESC on keyboard to exit
		char c = (char)waitKey(25);
		if (c == 27) break;
	}
	// When everything done, release the video capture object
	cap.release();
	return 0;
}