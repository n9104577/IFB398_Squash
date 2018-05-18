// Blob.h

#ifndef MY_BLOB
#define MY_BLOB

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class Blob {
public:
	//Blob Bounding Boxes
	std::vector<cv::Point> currentContour;
	cv::Rect currentBoundingRect;
	std::vector<cv::Point> centerPositions;

	//Blob Size
	double dblCurrentDiagonalSize;
	double dblCurrentAspectRatio;

	//Boolian Specifiers
	bool blnCurrentMatchFoundOrNewBlob;
	bool blnStillBeingTracked;

	//Other
	int intNumOfConsecutiveFramesWithoutAMatch;
	cv::Point predictedNextPosition;

	//Initalize Blob
	Blob(std::vector<cv::Point> _contour);

	//Public Functions
	void predictNextPosition(void);
	cv::Point getBottom(void);
};

#endif    // MY_BLOB


