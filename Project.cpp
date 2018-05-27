/* This is to verify that the below code is not for distribution.
Kashish Jain is the owner of this code */
#include "stdafx.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <sstream>
#include <fstream>
#include <ostream>
#include <opencv2/imgcodecs.hpp>
#include <algorithm>

using namespace std;
using namespace cv;
using namespace cv::face;

//Function to read ID and NAME from txt files using Tuple Data Type
/*tuple<int,string> FileRead() {
ifstream getName,getID;
getName.open("ReadName.txt");
getID.open("ReadID.txt");
tuple<int, string> NameID[100];
string Name;
int ID,i=1;
while(true) {
getName >> Name;
getID >> ID;
if (Name == " ") {
break;
}
NameID[i] = make_tuple(ID,Name);
i++;
}
getName.close();
getID.close();
return NameID[i];
}*/

//Global int array for ID to use in whole program anywhere and modify it
int ID[100];

// Function to get ID and Names from text files
string *FileRead() {
	ifstream getName, getID;
	getName.open("ReadName.txt");
	getID.open("ReadID.txt");
	string* Name = new string[100];
	int i = 0;
	while (!getID.eof()) {
		getName >> Name[i];
		getID >> ::ID[i];
		cout << ::ID[i] << "," << Name[i] << endl;
		i++;
	}
	getName.close();
	getID.close();
	return Name;
}

//Function to add name if person is not registered
int addName() {
	ofstream appendName, appendID;
	appendName.open("ReadName.txt", ios::app);
	appendID.open("ReadID.txt", ios::app);
	cout << "Enter Your Name : ";
	string Name;
	cin >> Name;
	string* NamePointer = FileRead();
	int i = 1;
	while (true) {
		if (::ID[i] == 0)
			break;
		i++;
	}
	cout << i + 1;
	int ID = i + 1;
	NamePointer[i + 1] = Name;
	appendID << endl << i + 1;
	appendName << "\n" << Name;
	return ID;
}

void recogniseFace(Mat &Img) {
	vector <Mat> Image;
	Image.push_back(imread("dataset/User.1.0.jpg", CV_LOAD_IMAGE_GRAYSCALE));
	Image.push_back(imread("dataset/User.1.1.jpg", CV_LOAD_IMAGE_GRAYSCALE));
	Image.push_back(imread("dataset/User.1.2.jpg", CV_LOAD_IMAGE_GRAYSCALE));
	vector <int> Label;
	Label.push_back(1);
	Label.push_back(1);
	Label.push_back(1);
	Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create(2, 2, 7, 7, 15);
	model->train(Image, Label);
	//model->save("results/Trained_LBPHrecognizer.xml");
	double conf = model->predict(Img);
	if (conf > 0) {
		cout << "\n\n\n**Kashish**";
	}
}

void predict(Mat &Img) {
	CascadeClassifier TrainedCascade;
	if (!TrainedCascade.load("results/Trained_LBPHrecognizer.xml")) {}
	Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create(2, 2, 7, 7, 15);
	double conf = model->predict(Img);
	if (conf > 0) {
		cout << "Kashish";
	}
}

void captureImage(Mat &Img) {
	int ID = addName(); static int count = 0;
	auto s = to_string(ID);
	int clock = count;
	auto s1 = to_string(clock);
	bool isSuccess = imwrite("dataset/User." + s + "." + s1 + ".jpg", Img);
	if (!isSuccess) {
		return;
	}
	//recogniseFace(Img);
	count++;
	cout << "Capturing image" << endl;
}

//Function to detect glass eye and enable tilt
/*Mat DetectGlassEyes(Mat Img) {
string faceCascadePath = "Haar/haarcascade_frontalcatface.xml";
string glassCascade = "Haar/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier faceCascade,glass_cascade;
if (!faceCascade.load(faceCascadePath)) {
}
if (!glass_cascade.load(glassCascade)) {
}
vector<Rect> glass,faces;
glass_cascade.detectMultiScale(Img, glass, 1.3, 5, CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE, Size(30, 30));
Mat croppedImageGray;
int rows = Img.rows;
int cols = Img.cols;
//detecting eyeglasses
for (int i = 0; i < glass.size(); i++) {
int sx = glass[i].x;
int sy = glass[i].y;
int sh = glass[i].height;
int sw = glass[i].width;
faceCascade.detectMultiScale(Img, faces, 1.3, 5, CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE, Size(30, 30));
for (int j = 0; j < faces.size(); j++) {
croppedImageGray = Img(faces[j]);
}
}
return croppedImageGray;
}*/

int main() {
	string faceCascadePath, eyeCascadePath;									// string for Cascade path
	CascadeClassifier faceCascade, eyeCascade;				// Cascade classifier to load cascade
															//Loading (Face Cascade)
	faceCascadePath = "Haar/haarcascade_frontalcatface.xml";
	//Loading (Eye Cascade)
	eyeCascadePath = "Haar/haarcascade_eye.xml";
	//Exit if can't load HaarCascade file
	if (!faceCascade.load(faceCascadePath)) {
		cout << "cannot load face cascade";
		return -1;
	}
	if (!eyeCascade.load(eyeCascadePath)) {
		cout << "Cannot Load eye cascade";
		return -1;
	}
	//vector for finding coordinates of eyes and face to draw rectangle
	vector <Rect> faces, eyes;
	VideoCapture cap(0);
	//Name of Window
	string myWindow = "Face - Detection";
	//string myCropped = "CroppedImage";
	//get the width of frames of the video
	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH);
	//get the height of frames of the video
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
	//Printing the Resolution of Video
	cout << "Resolution of the video : " << dWidth << " x " << dHeight << endl;
	//Creating Window To echo result
	//namedWindow(myCropped, 1);
	namedWindow(myWindow, 1);
	//Loo for continous detection
	while (true) {
		//Matrix to Store colored and grayScale images
		Mat frame, gray, gray_face, croppedImage;
		//Capturing image
		cap >> frame;
		//Converting BGR TO GRAYSCALE
		cvtColor(frame, gray, CV_BGR2GRAY);
		//Equalizing Hist for Better Results
		equalizeHist(gray, gray);
		//Detecting Faces
		faceCascade.detectMultiScale(gray, faces, 1.3, 5, CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		//Creating Rectangle over Detected Faces
		for (int i = 0; i < faces.size(); i++) {
			int x = faces[i].x;
			int y = faces[i].y;
			int height = faces[i].height;
			int width = faces[i].width;
			int tlY = faces[i].y;
			if (tlY < 0) {
				tlY = 0;
			}
			int drY = faces[i].y + faces[i].height;
			if (drY > frame.rows)
			{
				drY = frame.rows;
			}
			Point tl(faces[i].x, tlY);
			Point dr(faces[i].x + faces[i].width, drY);

			Rect myROI(tl, dr);

			croppedImage = gray(myROI);
			//Passing face coordinates for detection of eyes
			gray_face = croppedImage;
			putText(frame, "FACE DETECTED", Point(x + (width / 2), (y - 5)), FONT_HERSHEY_DUPLEX, .4, Scalar(255, 255, 255), 1);
			eyeCascade.detectMultiScale(gray_face, eyes, 1.3, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
			for (int j = 0; j < eyes.size(); j++)
			{
				//captureImage(gray_face);
				recogniseFace(gray_face);
				//predict(gray_face);
				rectangle(frame, Point(x, y), Point(x + width, y + height), Scalar(0, 255, 0), 2, 4);
			}
		}
		//Echo the image processing
		imshow(myWindow, frame);
		//imshow(myCropped, croppedImage);
		//Break When Escape is Pressed OR Exit when Escape is Pressed
		if (waitKey(10) == 27) {
			cout << "Exit\n";
			destroyAllWindows();
			break;
		}
	}
	return 0;
}
