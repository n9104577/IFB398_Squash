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

const int MIN_OBJECT_AREA = 2000;

//Slider Control
const bool usingTrackbar = true;
const bool usingMorph = true;

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
const string trackbarWindowHSV = "HSV";
const string trackbarWindowMorph = "Morph";

//Stubs
void onTrackbar(int, void*);
void createTrackbar(void);

void onTrackbar(int, void*) {
	//No settings Change
}

void createTrackbarHSV(void) {
	namedWindow(trackbarWindowHSV, WINDOW_AUTOSIZE);
	char TrackbarName[50];

	//Set Trackbar Names
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);

	//Create GUI   
	createTrackbar("Hue Min", trackbarWindowHSV, &H_MIN, H_MAX, onTrackbar);
	createTrackbar("Hue Max", trackbarWindowHSV, &H_MAX, H_MAX, onTrackbar);
	createTrackbar("Saturation Min", trackbarWindowHSV, &S_MIN, S_MAX, onTrackbar);
	createTrackbar("Saturation Max", trackbarWindowHSV, &S_MAX, S_MAX, onTrackbar);
	createTrackbar("Value Min", trackbarWindowHSV, &V_MIN, V_MAX, onTrackbar);
	createTrackbar("Value Max", trackbarWindowHSV, &V_MAX, V_MAX, onTrackbar);
}

void createTrackbarMorph(void) {
	namedWindow(trackbarWindowMorph, WINDOW_AUTOSIZE);
	char TrackbarName[50];

	//Set Trackbar Names
	sprintf(TrackbarName, "D_MIN", D_MIN);
	sprintf(TrackbarName, "E_MIN", E_MIN);

	//Create GUI  
	createTrackbar("Erorde", trackbarWindowMorph, &E_MIN, E_MAX, onTrackbar);
	createTrackbar("Dialate", trackbarWindowMorph, &D_MIN, D_MAX, onTrackbar);
}

void morphFrame(Mat &Pframe) {
	//Initalise
	int eElement = 7;
	int dElement = 13;

	//Set Values
	if (usingMorph) {
		eElement = E_MIN;
		dElement = D_MIN;
	}

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(eElement, eElement));
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(dElement, dElement));

	erode(Pframe, Pframe, erodeElement);
	dilate(Pframe, Pframe, dilateElement);
	medianBlur(Pframe, Pframe, 7);
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
	string file = "CVideo.mp4";
	//int file = 0; //Webcame Testing
	VideoCapture cap(file);

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video file" << endl;
		return -1;
	}

	//Create Trackbar or Morph
	if (usingTrackbar) createTrackbarHSV();
	if (usingMorph) createTrackbarMorph();

	while (1) {
		Mat frame;
		Mat HSVframe;
		Mat Pframe;

		// Catches Current Frame
		cap.read(frame);
		// Ends loop when frameEmpty
		if (frame.empty()) break;

		// Convert Frame to HSV Frame
		cvtColor(frame, HSVframe, CV_BGR2HSV);

		//Proccessed Frame Testing
		if (usingTrackbar) inRange(HSVframe, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), Pframe);
		//if (usingTrackbar) inRange(frame, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), Pframe);

		//Set Binary Variables
		int Hm = 0;
		int HM = H_MAX;
		int Sm = 0;
		int SM = 160;
		int Vm = 50;
		int VM = V_MAX;

		if (!usingTrackbar)inRange(HSVframe, Scalar(Hm, Sm, Vm), Scalar(HM, SM, VM), Pframe); //Proccessed Frame
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