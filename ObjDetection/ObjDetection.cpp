// ObjDetection.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
/**
* @file objectDetection2.cpp
* @author A. Huaman ( based in the classic facedetect.cpp in samples/c )
* @brief A simplified version of facedetect.cpp, show how to load a cascade classifier and how to find objects (Face + eyes) in a video stream - Using LBP here
*/
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2\core\ocl.hpp"
#include <iostream>
#include <stdio.h>
#include <conio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay(Mat frame);
int readFromCamera();
int readFromVideo();
void matrixTime();
bool callMethod(int choise);
void initSession();
void setCascade();
void setCascadeParams();

/** Global variables */
String cascade_name = "cascade";
CascadeClassifier cascade;
String window_name = "Capture - Pedestrian detection";
/**
* @function main
*/

float scaleFactor = 1.1;
int minNeighbors = 2;
int minSizeWidth = 6;
int minSizeHeight = 16;
int maxSizeWidth = 120;
int maxSizeHeight = 320;

int main()
{
	setlocale(LC_ALL, "Russian");
	initSession();
	return 0;
}


void matrixTime() {
	string matr("matrix");
	string buf;
	cin.clear();
	cin >> buf;
	int f = 0, i;
	i = 0;
	if (buf == matr) {
		char mas[6] = { '0','0' ,'0' ,'0' ,'0', '0' };
		system("cls");
		system("color 02");
		while (true) {
			cout << 10 + rand() % 90 << " ";
			if (_kbhit()) {
				if (i > 4) {
					for (int j = 0; j < 5; j++) {
						mas[j] = mas[j + 1];
					}
					i = 5;
				}
				mas[i] = _getch();
				i++;
			}
			if (mas[0] == matr.c_str()[0] &&
				mas[1] == matr.c_str()[1] &&
				mas[2] == matr.c_str()[2] &&
				mas[3] == matr.c_str()[3] &&
				mas[4] == matr.c_str()[4] &&
				mas[5] == matr.c_str()[5])
				break;
		}
		system("color 08");
		cout << "Bye!" << endl;
		system("cls");
	}
}


bool callMethod(int choise) {
	switch (choise) {
	case 0:
		return false;
		break;
	case 1:
		setCascade();
		return true;
		break;
	case 2:
		setCascadeParams();
		return true;
		break;
	case 3:
		readFromCamera();
		return true;
		break;
	case 4:
		readFromVideo();
		return true;
	default:
		return false;
		break;
	}
}


void initSession()
{
	const string ch = "Пожалуйста, сделайте выбор --> ";
	stringstream menu;
	menu << "0. Выход" << endl << "1. Задать классификатор" << endl << "2. Задать параметры классификатора" << endl << "3. Распознавание с камеры" << endl << "4. Распознавание с видео" << endl;


	cout << menu.str().c_str() << ch.c_str();
	int choise = -1;
	cin >> choise;
	while (choise < 0 || choise > 3) {
		matrixTime();
		cout << "Вы ошиблись, пожалуйста, повторите --> ";
		cin >> choise;
	}
	while (callMethod(choise)) {
		system("cls");
		cout << menu.str().c_str() << ch.c_str();
		cin >> choise;
		while (choise < 0 || choise > 5) {
			matrixTime();
			cout << "Вы ошиблись, пожалуйста, повторите --> ";
			cin >> choise;
		}
	}
}


void setCascade() {
	cout << "Введите путь к файлу каскада: ";
	string name;
	cin >> name;
	cascade_name = name.c_str();
	if (!cascade.load(cascade_name)) { cout << "Не удалось загрузить классификатор! Проверьте путь к файлу!" << endl; }
	else cout << "Классификатор успешно задан!";
}


void setCascadeParams() {
	cout << "Введите значение scaleFactor: ";
	cin >> scaleFactor;
	cout << "Введите значение minNeighbors: ";
	cin >> minNeighbors;
	cout << "Введите значение minSizeWidth: ";
	cin >> minSizeWidth;
	cout << "Введите значение minSizeHeight: ";
	cin >> minSizeHeight;
	cout << "Введите значение maxSizeWidth: ";
	cin >> maxSizeWidth;
	cout << "Введите значение maxSizeHeight: ";
	cin >> maxSizeHeight;
}


int readFromVideo() {
	if (cascade_name == "cascade") {
		cout << "Каскад не задан!"; return 1;
	}
	CvCapture* capture = 0;
	IplImage *frame = 0;

	cout << "Введите путь к файлу с видео: ";
	string name;
	cin >> name;
	const char* filename = name.c_str();

	printf("[i] file: %s\n", filename);

	// получаем информацию о видео-файле
	capture = cvCreateFileCapture(filename);

	cv::Mat m;
	IplImage* imageClone = 0;
	while (1) {
		// получаем следующий кадр
		frame = cvQueryFrame(capture);   //FRAME НЕ ДОЛЖЕН МОДИФИЦИРОВАТЬСЯ
		if (!frame) {
			break;
		}

		m = cv::cvarrToMat(frame);
		detectAndDisplay(m);

		char c = cvWaitKey(7);
		if (c == 27) { // если нажата ESC - выходим
			destroyAllWindows();
			break;
		}
	}

	// освобождаем ресурсы
	cvReleaseCapture(&capture);
	// удаляем окно
	return 0;
}


int readFromCamera() {
	if (cascade_name == "cascade") {
		cout << "Каскад не задан!"; return 1;
	}
	VideoCapture capture;
	Mat frame;

	//-- 1. Load the cascade
	if (!cascade.load(cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };

	//-- 2. Read the video stream
	capture.open(-1);
	if (!capture.isOpened()) { printf("--(!)Error opening video capture\n"); return -1; }

	while (capture.read(frame))
	{
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		//-- 3. Apply the classifier to the frame
		detectAndDisplay(frame);

		//-- bail out if escape was pressed
		int c = waitKey(1);
		if ((char)c == 27) { 
			destroyAllWindows();
			break; 
		}
	}
}


/**
* @function detectAndDisplay  Good function
*/
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	Mat frame_gray2 = frame_gray;

	cv::ocl::setUseOpenCL(false);
	//-- Detect faces
	cascade.detectMultiScale(frame_gray2, faces, scaleFactor, minNeighbors, 0, Size(minSizeWidth,minSizeHeight), Size(maxSizeWidth, maxSizeHeight));
	
	for (size_t i = 0; i < faces.size(); i++)
	{
		rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(0, 255, 0), 3, 8, 0);
		
	}
	
	//-- Show what you got
	imshow(window_name, frame);
}
