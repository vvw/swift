#pragma once
#ifndef ExamSheetScannerCore
	#define  ExamSheetScannerCore
#include "EssBitmap.h"

#define  unknowError 0
#define  outMemory 1
#define  lostCorner 2
#define  lostLocation 3
#define  tooFarAway 4
#define  success 5

typedef struct EssContext{
	int differRange;
	int luminanceSansitivity;
	int denstyThreshold;
	int withVersion;
	int minSize;
	//Դͼ��
	EssBitmap * sourceEssBitmap;
	//�ֶ�ֵͼ
	EssBitmap * binaryEssBitmap;
	//ϸ��ֵͼ
	EssBitmap * balanceBinaryEssBitmap;
	//ʶ����ͼ
	EssBitmap * outputEssBitmap;
	int version;
	int lastErrorCode;
	void * information;
	int numberDigit;
	int subjectDigit;
	int answerItems;
	int answerCount;
	int certainty;
	unsigned char * numberBuffer;
	unsigned char * subjectBuffer;
	unsigned char * answerBuffer;
	unsigned char * numberCertainties;
	unsigned char * subjectCertainties;
	unsigned char * answerCertainties;
}EssContext;


int readEssBitmapBuffer(char * jsonBuffer,int bufferSize,int version,int bpp,int width,int height,unsigned char * bitsBuffer);
int readEssBitmap(char * jsonBuffer,int bufferSize,int version,EssBitmap * bitmap);




EssContext * createEssContext();
void deleteEssContext(EssContext ** context);
int setEssBitmapBuffer(EssContext * context,int bpp,int width,int height,unsigned char * bitsBuffer);
int setEssBitmap(EssContext * context,EssBitmap * bitmap);
int createEssBitmapForOutput(EssContext * context,int bitmapIndex,int bpp);
int setDifferRange(EssContext * context,int value);
int setLuminanceSansitivity(EssContext * context,int value);
int setDenstyThreshold(EssContext * context,int value);
int setReadWithVersion(EssContext * context,int version);
int readContext(EssContext * context);
EssBitmap * getSourceEssBitmap(EssContext * context,int bpp);
EssBitmap * getBinaryEssBitmap(EssContext * context,int bpp);
EssBitmap * getBalanceBinaryEssBitmap(EssContext * context,int bpp);
EssBitmap * getOutputEssBitmap(EssContext * context,int bpp);
int getVersion(EssContext * context);
int getLastErrorCode(EssContext * context);
int getNumberDigit(EssContext * context);
int getSubjectDigit(EssContext * context);
int getAnswerItems(EssContext * context);
int getAnswerCount(EssContext * context);
int getCertainty(EssContext * context);
unsigned char * getNumberBufferPtr(EssContext * context);
unsigned char * getSubjectBufferPtr(EssContext * context);
unsigned char * getAnswerBufferPtr(EssContext * context);
int copyNumberBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize);
int copySubjectBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize);
int copyAnswerBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize);
unsigned char * getNumberCertaintiesBufferPtr(EssContext * context);
unsigned char * getSubjectCertaintiesBufferPtr(EssContext * context);
unsigned char * getAnswerCertaintiesBufferPtr(EssContext * context);
int copyNumberCertaintiesBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize);
int copySubjectCertaintiesBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize);
int copyAnswerCertaintiesBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize);


#endif
