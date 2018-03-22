#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

int main(void) {
	//string file = "Video.mp4";
	int file = 0; //Webcame Testing
	VideoCapture cap(file);

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video file" << endl;
		return -1;
	}

	int frameEmpty = 0;
	while (!frameEmpty) {

		Mat frame;
		Mat HSVframe;
		Mat Pframe;

		// Catches Current Frame
		cap.read(frame);
		// Ends loop when frameEmpty
		if (frame.empty()) frameEmpty = 1;

		// Convert Frame to HSV Frame
		cvtColor(frame, HSVframe, CV_BGR2HSV);
		//inRange(HSVframe, Scalar(0, 0, 0), Scalar(100, 100, 100), Pframe); //Proccessed Frame
		inRange(frame, Scalar(0, 0, 0), Scalar(50, 50, 255), Pframe);


		// Display frame
		imshow("Frame", frame);
		imshow("Processed", Pframe);

		// Press  ESC on keyboard to exit
		char c = (char)waitKey(25);
		if (c == 27) break;
	}

	// When everything done, release the video capture object
	cap.release();

	// Closes all the frames
	destroyAllWindows();

	return 0;
}