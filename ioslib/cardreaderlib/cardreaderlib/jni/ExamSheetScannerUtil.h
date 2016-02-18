#pragma once
#include "ExamSheetScannerCore.h"

typedef struct EssPoint{
	int x,y;
}EssPoint;

typedef struct EssPoints{
	int count;
	EssPoint * points;
}EssPoints;

typedef struct EssRect{
	int left,top,right,bottom;
}EssRect;

typedef struct EssLine{
	EssPoint start,end,center;
	double sn,cs;
	int length;
}EssLine;

typedef struct EssLines{
	int count;
	int maxLength;
	EssLine * lines;
}EssLines;

typedef struct EssCorner{
	EssPoint point;
	EssRect rect;
}EssCorner;

typedef struct EssCornerArray{
	int count;
	EssCorner * corners;
}EssCornerArray;

typedef struct EssCorners{
	EssCorner topLeft,topRight,lowerLeft,lowerRight;
}EssCorners;

typedef struct EssString{
	EssPoint start,end;
	double step;
	int bitCount;
	EssPoint points[32];
	unsigned char certainty[32];
	unsigned char bitBytes[32];
}EssString;

typedef struct EssStrings{
	int count;
	EssString * strings;
}EssStrings;

typedef struct EssLocations{
	EssLines * top;
	EssLines * bottom;
	EssLines * left;
	EssLines * right;
	EssLines * horizontal;
	EssLines * verticalTop;
	EssLines * verticalLower;
}EssLocations;

typedef struct EssInformation{
	EssCorners * corners;
	EssLocations * locations;
	int rows;
	int columns;
	int items;
	int numberStringLength;
	int answerStringLenght;
}EssInformation;

int strjn(char ** str1,char * str2);

EssInformation * createEssInformation();
void deleteEssInformation(EssInformation ** information);

int getCorners(EssContext * context);
	EssCornerArray * getCornerArray(EssBitmap * bitmap,double differRange);
	int getVersion1Corners(EssContext * context,EssCornerArray * essCornerArray);
	int getVersion2Corners(EssContext * context,EssCornerArray * essCornerArray);
	int likeCorner(EssCorner * corner1,EssCorner * corner2,double differRange);
	int swapCorner(EssCorner * corner1,EssCorner * corner2);
	int isCorner(EssBitmap * bitmap,int cdtX,int cdtY,EssCorner * corner,double differRange);

	EssLines * getLines(EssBitmap * bitmap,EssPoint * start,EssPoint * end,int clear);
		EssLines * createEssLines(int count);
		void deleteEssLines(EssLines ** lines);
		int clearNearPoints(EssBitmap * bitmap,int x,int y,EssPoints * points);


int getEssStrings(EssContext * context,EssStrings * numberStrings,EssStrings * answerStrings);
	int getEssString(EssContext * context,EssBitmap * mask,int x,int y,EssString * string);
	int sortEssStrings(EssContext * context,EssStrings * numberStrings,EssStrings * answerStrings);

int readEssStrings(EssContext * context);
	int readEssString(EssContext * context,EssString * string);

int getIntersectionPoint(int width,int height,EssPoint * line1Start,EssPoint * line1End,EssPoint * line2Start,EssPoint * line2End,EssPoint * pointOut);
unsigned char getCertaintyRange(unsigned char  centainty);
int readPoint(EssContext * context,EssPoint * point,int radius,double denstyThreshold,unsigned char * centainty);
int readPoints(EssContext * context);

int drawSign(EssBitmap * bitmap,EssPoint * point,int ratius,unsigned int colorARGB);
