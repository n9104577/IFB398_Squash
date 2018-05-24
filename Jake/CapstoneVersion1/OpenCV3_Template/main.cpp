
// Add all Includes 
// Open Cv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <limits>
#include <numeric>
#include <iostream>

// Header files
#include "Blob.h"
#include <stdio.h>

// Other
#include <iostream>
#include <sstream>


// Name Spaces 
using namespace cv;
using namespace std;

// load video
char* video = "squash.mp4";

// Homography
bool pointsFound = false;
bool usingMouseClick = false;
vector<Point2f> imageCourtTemplatePoints;  
vector<Point2f> imageCourtPlayersPoints;  
vector<Point2f> court_Points;

Mat imageCourtPlayersCopyblack;
Mat tracklines;
Mat imageCourtPlayers;
Mat imageCourtTemplate;
Mat imageCourtPlayersCopy;
Mat ROI_result;
Mat H;

// Globals 
Mat frame; // Store the current Frame
Mat MOG2FgMask; // MOG2 Mask
Ptr<BackgroundSubtractor> MOG2Bs; //MOG2 Background Subtractor

int keyboardInput; // Keyboard Input

// Track Bar variables 
const bool usingTrackbar = false;
const string trackbarWindowName = "Trackbars";
int B_MIN = 5; // Blur Min
int B_MAX = 30; // Blur Max
int T_MIN = 74; // Threshold Min
int T_MAX = 256; // Threshold Max
int minBlobSize = 2000; // blob size min
int maxBlobSize = 7000;
int D_MIN = 1;
int D_MAX = 30;
int E_MIN = 1;
int E_MAX = 30;


void onTrackbar(int, void*) {
	//Empty for some reason
}
void createTrackbar(void);

// Scalars
const Scalar SCALAR_BLACK = Scalar(0.0, 0.0, 0.0);
const Scalar SCALAR_WHITE = Scalar(255.0, 255.0, 255.0);
const Scalar SCALAR_YELLOW = Scalar(0.0, 255.0, 255.0);
const Scalar SCALAR_GREEN = Scalar(0.0, 200.0, 0.0);
const Scalar SCALAR_RED = Scalar(0.0, 0.0, 255.0);

//Function Stubs
void matchCurrentFrameBlobsToExistingBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, vector<Blob> &existingBlobs);
double distanceBetweenPoints(Point point1, Point point2);
void drawAndShowContours(Size imageSize, vector<vector<Point> > contours, string strImageName);
void drawAndShowContours(Size imageSize, vector<Blob> blobs, string strImageName);
void drawBlobInfoOnImage(vector<Blob> &blobs, Mat &imgFrame2Copy);
void analyseVideo(char* video);
void on_mouse(int e, int x, int y, int d, void *ptr);
void drawBlobBottomOnImage(vector<Blob> &blobs, Mat &imgFrame2Copy);


// Show windows
bool showimgFrame2Copy = true; // show the red rectangle
bool showDisplayWindow = false; // show the first image for picking points
bool showimgContours = false; // shows contours
bool showimgConvexHulls = false; // show convexHulls
bool showimgCurrentFrameBlobs = true; // shows the blob
bool showimgBlobs = false;
bool showimageCourtPlayersCopyblack = false; // 
bool showROI_result = false; // 
bool showHomoResult = false; // 
bool showframe = false; // shows the current frame
bool showtransparentResult = true; // shows the players feet marks on a transparent image
bool showMOG2FgMask = false; // show the fg mask

int main(void) {
	// Load Images
	imageCourtPlayers = imread("squashCourt.png", CV_LOAD_IMAGE_COLOR);
	imageCourtTemplate = imread("squashCourtTop.jpg", CV_LOAD_IMAGE_COLOR);
	imageCourtPlayersCopy = imread("squashCourt.png", CV_LOAD_IMAGE_COLOR);

	//Resize Images
	resize(imageCourtPlayers, imageCourtPlayers, Size(768, 576));
	//resize(imageCourtTemplate, imageCourtTemplate, Size(768, 576));
	resize(imageCourtPlayersCopy, imageCourtPlayersCopy, Size(768, 576));

	// Create a Black of the video 
	Mat black(imageCourtPlayersCopy.rows, imageCourtPlayersCopy.cols, imageCourtPlayersCopy.type(), Scalar(255, 255, 255, 0));
	imageCourtPlayersCopyblack = black;
	// Push the 4 corners of the logo image as the 4 points for correspondence to calculate homography.
	imageCourtTemplatePoints.push_back(Point2f(float(0), float(0)));
	imageCourtTemplatePoints.push_back(Point2f(float(0), float(imageCourtTemplate.rows)));
	imageCourtTemplatePoints.push_back(Point2f(float(imageCourtTemplate.cols), float(imageCourtTemplate.rows)));
	imageCourtTemplatePoints.push_back(Point2f(float(imageCourtTemplate.cols), float(0)));
	
	
	// create Trackbar
	if (usingTrackbar) createTrackbar();

	// create Background Subtractor - MOG2 approach
	MOG2Bs = createBackgroundSubtractorMOG2().dynamicCast<BackgroundSubtractor>();
	
	// Homography
	
	
	// if we want to click an image to pick our points else set hard coded points for homography
	if (!usingMouseClick) {
		imageCourtPlayersPoints.push_back(Point2f(float(219), float(291)));
		imageCourtPlayersPoints.push_back(Point2f(float(39), float(574)));
		imageCourtPlayersPoints.push_back(Point2f(float(717), float(574)));
		imageCourtPlayersPoints.push_back(Point2f(float(535), float(291)));
		// once we get 4 corresponding points in both images calculate homography matrix
		H = findHomography(imageCourtPlayersPoints, imageCourtTemplatePoints, 0);
	}
	else {
		namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
		imshow("Display window", imageCourtPlayers);
		setMouseCallback("Display window", on_mouse, NULL);
		cout << " Click the four corners of the court starting from top left going counter clockwise " << endl;
		cout << " Click any where on the fifth click " << endl;
		while (1) {
			int key = cvWaitKey(10);
			if (key == 27) break;
		}
	}
	
	
	analyseVideo(video);
	cout << " Press escape to exit " << endl;
	while (1) {
		
		int key = cvWaitKey(10);
		if (key == 27) break;
	}
	destroyAllWindows();
	return EXIT_SUCCESS;
}

<<<<<<< HEAD


=======
/// <summary>
/// 
/// </summary>
/// <param name="src1"></param>
/// <param name="src2"></param>
void showFinal(Mat src1, Mat src2) {
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
	

}

/// <summary>
/// Gets X, Y of Mouse
/// </summary>
/// <param name="e"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="d"></param>
/// <param name="ptr"></param>
>>>>>>> 232a30fafeba3af67640d30a6c6d9a58ab0b49e1
void on_mouse(int e, int x, int y, int d, void *ptr) {

	if (e == EVENT_LBUTTONDOWN) {
		if (imageCourtPlayersPoints.size() < 4) {
			imageCourtPlayersPoints.push_back(Point2f(float(x), float(y)));
			cout << x << " " << y << endl;
		}
		else {
			cout << " Press ESC to continue " << endl;	
			// Deactivate callback
			setMouseCallback("Display window", NULL, NULL);
			// once we get 4 corresponding points in both images calculate homography matrix
			H = findHomography(imageCourtPlayersPoints, imageCourtTemplatePoints, 0);	
		}
	}
	
}

/// <summary>
/// 
/// </summary>
/// <param name="video">Video File Path</param>
void analyseVideo(char* video) {
	//create the capture object
	VideoCapture capture(video);
	// Check the video can open
	if (!capture.isOpened()) {		
		cerr << "cannot open video: " << video << endl;
		exit(EXIT_FAILURE);
	}

	// Create blobs vector
	vector<Blob> blobs;
	bool blnFirstFrame = true;


	//loop through each frame of the video. ESC or 'q' for quitting
	while ((char)keyboardInput != 'q' && (char)keyboardInput != 27) {
		if (usingTrackbar) createTrackbar();
		else {
			B_MIN = 1;
			B_MAX = 16;
			T_MIN = 174;
			T_MAX = 256;
		}
		vector<Blob> currentFrameBlobs;

		//read the current frame
		if (!capture.read(frame)) {
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}

		resize(frame, frame, Size(768, 576));
		//update the background model
<<<<<<< HEAD


=======
>>>>>>> 232a30fafeba3af67640d30a6c6d9a58ab0b49e1
		Mat imgFrame2Copy = frame.clone();
		MOG2Bs->apply(frame, MOG2FgMask);
		
		// I added the dilate and erode just for testing, wanted to see if it would
		// help, kinda does 
		int dElement;
		int eElement;
		if (usingTrackbar) {
			dElement = D_MIN;
			eElement = E_MIN;
		}
		else {
			dElement = 6;
			eElement = 4;
		}
		Mat dilateElement = getStructuringElement(MORPH_RECT, Size(dElement, dElement));
		Mat erodeElement = getStructuringElement(MORPH_RECT, Size(eElement, eElement));
		dilate(MOG2FgMask, MOG2FgMask, dilateElement);
		erode(MOG2FgMask, MOG2FgMask, dilateElement);


		// blur the image, fills in gaps in blobs and get smoother edges
		blur(MOG2FgMask, MOG2FgMask, Size(B_MIN, B_MAX), Point(-1, -1));
		
		// get rid of shadows 
		threshold(MOG2FgMask, MOG2FgMask, T_MIN, T_MAX, THRESH_BINARY);
		
		//imshow("FG Mask MOG 2", MOG2FgMask);

		//  find and draw contours
		vector<vector<Point> > contours;
		Mat imgThreshCopy = MOG2FgMask.clone();
		findContours(imgThreshCopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
<<<<<<< HEAD
		if (showimgContours) { drawAndShowContours(MOG2FgMask.size(), contours, "imgContours"); }
		
		
		// get the convex hull of the contours
=======
		//drawAndShowContours(fgMaskMOG2.size(), contours, "imgContours");
>>>>>>> 232a30fafeba3af67640d30a6c6d9a58ab0b49e1
		vector<vector<Point> > convexHulls(contours.size());
		for (unsigned int i = 0; i < contours.size(); i++) {
			convexHull(contours[i], convexHulls[i]);
		}
		// showimgConvexHulls = true than show it 
		if (showimgConvexHulls) { drawAndShowContours(MOG2FgMask.size(), convexHulls, "imgConvexHulls"); }

		for (auto &convexHull : convexHulls) {
			Blob possibleBlob(convexHull);

			// check if the blob is large enough to be a person and not just random noise or a section of a person
			if (possibleBlob.currentBoundingRect.area() > minBlobSize) {
				cerr << "blob area: " << possibleBlob.currentBoundingRect.area() << endl;
				currentFrameBlobs.push_back(possibleBlob);
			}
		}

		// show the left over blobs if we want to 
		if (showimgCurrentFrameBlobs) { drawAndShowContours(MOG2FgMask.size(), currentFrameBlobs, "imgCurrentFrameBlobs"); }

		// if its first frame push back the blobs
		if (blnFirstFrame == true) {
			for (auto &currentFrameBlob : currentFrameBlobs) {
				blobs.push_back(currentFrameBlob);
			}
		}
		// else try match the new blobs to the existing ones
		else {
			matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
		}

<<<<<<< HEAD
		// show the blobs if we want
		if (showimgBlobs) { drawAndShowContours(MOG2FgMask.size(), blobs, "imgBlobs"); }
=======
		drawAndShowContours(MOG2FgMask.size(), blobs, "imgBlobs");

		imgFrame2Copy = frame.clone();          // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above
>>>>>>> 232a30fafeba3af67640d30a6c6d9a58ab0b49e1

		// get a copy of the original frame and draw the rectangle and center point on it
		imgFrame2Copy = frame.clone();       
		drawBlobInfoOnImage(blobs, imgFrame2Copy);
		drawBlobBottomOnImage(blobs, imgFrame2Copy);
		if (showimgFrame2Copy) { imshow("imgFrame2Copy", imgFrame2Copy); }

		// draw the players feet position on the black copy of the court
		drawBlobBottomOnImage(blobs, imageCourtPlayersCopyblack);
		//drawBlobInfoOnImage(blobs, imageCourtPlayersCopy);
		if (showimageCourtPlayersCopyblack) { imshow("tracklines", imageCourtPlayersCopyblack); }
	
		// Homography
		Mat homoFrame = imageCourtPlayersCopyblack.clone();
		Mat homoFrameClone = imageCourtPlayersCopyblack.clone();
		// Create a black image
		Mat black(homoFrameClone.rows, homoFrameClone.cols, homoFrameClone.type(), Scalar::all(0));
		// create a white mask
		Mat mask(homoFrameClone.rows, homoFrameClone.cols, CV_8UC1, Scalar(0));

		// Get the coordiantes of the polygon (court area we want) from the mouse clicks
		vector< vector<Point> >  co_ordinates;
		co_ordinates.push_back(vector<Point>());
		co_ordinates[0].push_back(imageCourtPlayersPoints[0]);
		co_ordinates[0].push_back(imageCourtPlayersPoints[1]);
		co_ordinates[0].push_back(imageCourtPlayersPoints[2]);
		co_ordinates[0].push_back(imageCourtPlayersPoints[3]);
		drawContours(mask, co_ordinates, 0, Scalar(255), CV_FILLED, 8);
		black.copyTo(homoFrameClone, mask);
		// create a new image with the mask on the court 
		Mat dst1;
		
		homoFrame.copyTo(dst1, mask);
		
		// Do some stuff to and store the ROI in result 
		Mat dst2;
		bitwise_not(mask, mask);
		dst1.copyTo(dst2, mask);
		ROI_result = dst1 + dst2;

<<<<<<< HEAD
		//namedWindow("black", WINDOW_AUTOSIZE);
		if (showROI_result) { imshow("black", ROI_result); }
		
=======
		namedWindow("black", WINDOW_AUTOSIZE);
		imshow("black", ROI_result);

>>>>>>> 232a30fafeba3af67640d30a6c6d9a58ab0b49e1
		Mat HomoResult;
		
		warpPerspective(ROI_result, HomoResult, H, imageCourtTemplate.size());
		// Warp the logo image to change its perspective
		//namedWindow("RESULTS", WINDOW_AUTOSIZE);
		if (showHomoResult) { imshow("RESULTS", HomoResult); }

		Mat transparentResult;

		imageCourtTemplate.copyTo(transparentResult, HomoResult);
		namedWindow("trans", WINDOW_AUTOSIZE);
		if (showtransparentResult) { imshow("trans", transparentResult); }
		



		// its no longer the first frame
		blnFirstFrame = false;
		//show the current frame and the fg masks
		if (showframe) { imshow("Frame", frame); }
		if (showMOG2FgMask) { imshow("FG Mask MOG 2", MOG2FgMask); }
		//get the input from the keyboard
		keyboardInput = waitKey(30);
	}
	//delete capture object
	capture.release();
}

void createTrackbar(void) {
	namedWindow("Trackbars", WINDOW_FREERATIO);
	char TrackbarName[100];

	//Set Trackbar Names
	sprintf_s(TrackbarName, "Blur min %d", B_MIN);
	sprintf_s(TrackbarName, "blur max %d", B_MAX);
	sprintf_s(TrackbarName, "Thres_MIN %d", T_MIN);
	sprintf_s(TrackbarName, "Thres_MAX %d", T_MAX);
	sprintf_s(TrackbarName, "blob min %d", minBlobSize);
	sprintf_s(TrackbarName, "blob max %d", maxBlobSize);
	sprintf(TrackbarName, "D_MIN", D_MIN);
	sprintf(TrackbarName, "E_MIN", E_MIN);
	
	//Create GUI   
	createTrackbar("Blur min", trackbarWindowName, &B_MIN, B_MAX, onTrackbar);
	createTrackbar("Blur max", trackbarWindowName, &B_MAX, B_MAX, onTrackbar);
	createTrackbar("Saturation Min", trackbarWindowName, &T_MIN, T_MAX, onTrackbar);
	createTrackbar("Saturation Max", trackbarWindowName, &T_MAX, T_MAX, onTrackbar);
	createTrackbar("blob Min", trackbarWindowName, &minBlobSize, maxBlobSize, onTrackbar);
	createTrackbar("blob Max", trackbarWindowName, &maxBlobSize, maxBlobSize, onTrackbar);
	createTrackbar("Dialate", trackbarWindowName, &D_MIN, D_MAX, onTrackbar);
	createTrackbar("Erorde", trackbarWindowName, &E_MIN, E_MAX, onTrackbar);
}


void drawAndShowContours(Size imageSize, vector<vector<Point> > contours, string strImageName) {
	Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	drawContours(image, contours, -1, SCALAR_WHITE, -1);

	imshow(strImageName, image);
}

void drawAndShowContours(Size imageSize, vector<Blob> blobs, string strImageName) {

	Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	vector<vector<Point> > contours;

	for (auto &blob : blobs) {
		if (blob.blnStillBeingTracked == true) {
			contours.push_back(blob.currentContour);
		}
	}

	drawContours(image, contours, -1, SCALAR_WHITE, -1);

	imshow(strImageName, image);
}

/// <summary>
/// Find the hypotinues bettween two points
/// c^2 = a^2 + b^2
/// </summary>
/// <param name="point1">X, Y point</param>
/// <param name="point2">X, Y point</param>
/// <returns>Distance bettween two points or c</returns>
double distanceBetweenPoints(Point point1, Point point2) {
	//Find length a and b
	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);
	
	//c^2 = a^2 + b^2
	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}


void drawBlobInfoOnImage(vector<Blob> &blobs, Mat &imgFrame2Copy) {
	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].blnStillBeingTracked == true) {
			
			rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);
			
			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
			int intFontThickness = (int)round(dblFontScale * 1.0);

			//putText(imgFrame2Copy, to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}

void drawBlobBottomOnImage(vector<Blob> &blobs, Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].blnStillBeingTracked == true) {
			Point points = blobs[i].getBottom();
			circle(imgFrame2Copy, points, 4, SCALAR_GREEN, FILLED);
			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
			int intFontThickness = (int)round(dblFontScale * 1.0);

			//putText(imgFrame2Copy, to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}


void addBlobToExistingBlobs(Blob &currentFrameBlob, vector<Blob> &existingBlobs, int &intIndex) {

	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

void addNewBlob(Blob &currentFrameBlob, vector<Blob> &existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs.push_back(currentFrameBlob);
}


void matchCurrentFrameBlobsToExistingBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;

		existingBlob.predictNextPosition();
	}


	for (auto &currentFrameBlob : currentFrameBlobs) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {
			if (existingBlobs[i].blnStillBeingTracked == true) {
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 1.15) {
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}

	}

	for (auto &existingBlob : existingBlobs) {

		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlob.blnStillBeingTracked = false;
		}

	}

}
