// Blob.cpp
#include "Blob.h"

using namespace std;
using namespace cv;

//Global Variables
cv::Point currentBottom;
cv::Point currentCenter;

Blob::Blob(std::vector<cv::Point> _contour) {
	currentContour = _contour;
	currentBoundingRect = boundingRect(currentContour);

	//Declaring X Y Points
	currentCenter.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2;
	currentCenter.y = (currentBoundingRect.y + currentBoundingRect.y + currentBoundingRect.height) / 2;
	currentBottom.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2;
	currentBottom.y = (currentBoundingRect.y + currentBoundingRect.height);
	
	centerPositions.push_back(currentCenter);
	dblCurrentDiagonalSize = sqrt(pow(currentBoundingRect.width, 2) + pow(currentBoundingRect.height, 2));
	dblCurrentAspectRatio = (float)currentBoundingRect.width / (float)currentBoundingRect.height;
	blnStillBeingTracked = true;
	blnCurrentMatchFoundOrNewBlob = true;

	//This is never modified
	intNumOfConsecutiveFramesWithoutAMatch = 0;
}

cv::Point Blob::getBottom() {
	return currentBottom;
}

/// <summary>
/// You have to explain wtf this does - idk what it does
/// </summary>
void Blob::predictNextPosition(void) {
	//Initalizing Variables 
	int numPos = (int)centerPositions.size();
	int sumX = 0;
	int sumY = 0;
	int div = 0;

	//Predict X and Y
	if (numPos == 1) {
		predictedNextPosition.x = centerPositions.back().x;
		predictedNextPosition.y = centerPositions.back().y;
		

	} else {
		for (int i = 1; i <= (int)centerPositions.size() - 1; i++) {
			sumX += (centerPositions[i].x - centerPositions[i - 1].x) * i;
			sumY += (centerPositions[i].y - centerPositions[i - 1].y) * i;
			div += i;
			
		}
		//Round Numbers
		int deltaX = (int)round((float)sumX / div);
		int deltaY = (int)round((float)sumX / div);

		predictedNextPosition.x = centerPositions.back().x + deltaX;
		predictedNextPosition.y = centerPositions.back().y + deltaY;
	
	}
}

