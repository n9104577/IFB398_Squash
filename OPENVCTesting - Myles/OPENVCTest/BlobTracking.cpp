#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

//Constants 
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

const int MIN_OBJECT_AREA = 2200;

//Slider Control
const bool usingTrackbar = false;
const bool usingMorph = false;

//Varibles
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

int D_MIN = 1;
int D_MAX = 30;
int E_MIN = 1;
int E_MAX = 30;

const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string trackbarWindowName = "Trackbars";

//Stubs
void onTrackbar(int, void*);
void createTrackbar(void);

void onTrackbar(int, void*) {
	//Empty for some reason
}

void createTrackbar(void) {
	namedWindow(trackbarWindowName, WINDOW_AUTOSIZE);
	char TrackbarName[50];

	//Set Trackbar Names
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	
	//Create GUI   
	createTrackbar("Hue Min", trackbarWindowName, &H_MIN, H_MAX, onTrackbar);
	createTrackbar("Hue Max", trackbarWindowName, &H_MAX, H_MAX, onTrackbar);
	createTrackbar("Saturation Min", trackbarWindowName, &S_MIN, S_MAX, onTrackbar);
	createTrackbar("Saturation Max", trackbarWindowName, &S_MAX, S_MAX, onTrackbar);
	createTrackbar("Value Min", trackbarWindowName, &V_MIN, V_MAX, onTrackbar);
	createTrackbar("Value Max", trackbarWindowName, &V_MAX, V_MAX, onTrackbar);
}

void createTrackbarD(void) {
	namedWindow(trackbarWindowName, WINDOW_AUTOSIZE);
	char TrackbarName[50];

	//Set Trackbar Names
	sprintf(TrackbarName, "D_MIN", D_MIN);
	sprintf(TrackbarName, "E_MIN", E_MIN);

	//Create GUI  
	createTrackbar("Erorde", trackbarWindowName, &E_MIN, E_MAX, onTrackbar);
	createTrackbar("Dialate", trackbarWindowName, &D_MIN, D_MAX, onTrackbar);	
}

void morphFrame(Mat &Pframe) {
	//Initalise
	int eElement = 23;
	int dElement = 3;
	
	//Set Values
	if (usingMorph) {
		eElement = E_MIN;
		dElement = D_MIN;
	}
	
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(eElement, eElement));
 	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(dElement, dElement));

	erode(Pframe, Pframe, erodeElement);
	dilate(Pframe, Pframe, dilateElement);
}

void drawObject(int x, int y, Mat &frame) {
	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25<FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25<FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);
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
				cout << area << endl;
				if (area > MIN_OBJECT_AREA) {
					x = moment.m10 / area;
					y = moment.m01 / area;
					drawObject(x, y, frame);
				}
			}
	}
}

int main(void) {
	string file = "Video3.mp4";
	//int file = 0; //Webcame Testing
	VideoCapture cap(file);

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video file" << endl;
		return -1;
	}

	//Create Trackbar or Morph
	if (usingTrackbar) createTrackbar();
	if (usingMorph) createTrackbarD();

	while (1) {
		Mat frame;
		Mat HSVframe;
		Mat Pframe;

		//Resizing Frame
			// Not Implemented Yet

		// Catches Current Frame
		cap.read(frame);
		// Ends loop when frameEmpty
		if (frame.empty()) break;

		// Convert Frame to HSV Frame
		cvtColor(frame, HSVframe, CV_BGR2HSV);

		//Proccessed Frame Testing
		if (usingTrackbar) inRange(HSVframe, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), Pframe);	

		//Set Binary Variables
		int H = 0;
		int S = 0;
		int V = 18;

		if (!usingTrackbar)inRange(HSVframe, Scalar(H,S,V), Scalar(H_MAX, S_MAX, V_MAX), Pframe); //Proccessed Frame
		//inRange(frame, Scalar(0, 0, 0), Scalar(50, 50, 255), Pframe); //For Colour

		//Morph Image
		morphFrame(Pframe);

		//Track Filtered Objects
		Pframe = 255 - Pframe;
		trackFilteredObject(Pframe, frame);

		// Display frame
		imshow("Frame", frame);
		//imshow("HSV", HSVframe);
		imshow("Binary", Pframe);

		// Press  ESC on keyboard to exit
		char c = (char)waitKey(25);
		if (c == 27) break;
	}

	// When everything done, release the video capture object
	cap.release();

	return 0;
}