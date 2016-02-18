
#include "ExamSheetScannerCore.h"
#include <jni.h>
#include <malloc.h>

#include "log.h"
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_readEssBitmapBuffer
	(JNIEnv * env, jobject thiz,jbyteArray jsonBuffer,jint bufferSize,jint version,jint bpp,jint width,jint height,jbyteArray bitsBuffer){
	int bitSize=bpp*width/8;
	if(bitSize%4>0)bitSize+=4-bitSize%4;
	bitSize*=height;
	unsigned char * bits=(unsigned char *)malloc(bitSize);
	if(bits==NULL)
		return 0;
	unsigned char * json=(unsigned char *)malloc(40960);
	if(json==NULL){
		free(bits);
		return 0;
	}
	(*env)->GetByteArrayRegion(env,bitsBuffer,0,bitSize,(jbyte*)bits);
	int ret=readEssBitmapBuffer(json,40960,version,bpp,width,height,bits);
	(*env)->SetByteArrayRegion(env,jsonBuffer,0,ret,json);
	free(bits);
	free(json);
	return ret;
}

jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_readEssBitmapBuffer32Bpp
	(JNIEnv * env, jobject thiz,jbyteArray jsonBuffer,jint bufferSize,jint version,jint bpp,jint width,jint height,jintArray bitsBuffer){
	int bitSize=bpp*width/8;
	if(bitSize%4>0)bitSize+=4-bitSize%4;
	bitSize*=height;
	unsigned char * bits=(unsigned char *)malloc(bitSize);
	if(bits==NULL)
		return 0;
	unsigned char * json=(unsigned char *)malloc(40960);
	if(json==NULL){
		free(bits);
		return 0;
	}
	(*env)->GetIntArrayRegion(env,bitsBuffer,0,bitSize/4,(jint*)bits);
	int ret=readEssBitmapBuffer(json,40960,version,bpp,width,height,bits);
	(*env)->SetByteArrayRegion(env,jsonBuffer,0,ret,json);
	free(bits);
	free(json);
	return ret;
}



jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getEssBitmapBpp
	(JNIEnv * env, jobject thiz,jlong bitmap){
	EssBitmap * bmp=(EssBitmap *)(long)bitmap;
	if(isEssBitmap(bmp)==NULL)
		return 0;
	return bmp->bpp;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getEssBitmapStride
	(JNIEnv * env, jobject thiz,jlong bitmap){
	EssBitmap * bmp=(EssBitmap *)(long)bitmap;
	if(isEssBitmap(bmp)==NULL)
		return 0;
	return bmp->stride;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getEssBitmapWidth
	(JNIEnv * env, jobject thiz,jlong bitmap){
	EssBitmap * bmp=(EssBitmap *)(long)bitmap;
	if(isEssBitmap(bmp)==NULL)
		return 0;
	return bmp->width;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getEssBitmapHeight
	(JNIEnv * env, jobject thiz,jlong bitmap){
	EssBitmap * bmp=(EssBitmap *)(long)bitmap;
	if(isEssBitmap(bmp)==NULL)
		return 0;
	return bmp->height;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copyEssBitmapBits
	(JNIEnv * env, jobject thiz,jbyteArray outputBufferPtr,jint outputBufferSize,jlong bitmap){
	EssBitmap * bmp=(EssBitmap *)(long)bitmap;
	if(isEssBitmap(bmp)==NULL)
		return 0;
	int size=outputBufferSize < bmp->stride*bmp->height ? outputBufferSize : bmp->stride*bmp->height;
	(*env)->SetByteArrayRegion(env,outputBufferPtr,0,size,bmp->bits);
	return size;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copyEssBitmapBits32Bpp
	(JNIEnv * env, jobject thiz,jintArray outputBufferPtr,jint outputBufferSize,jlong bitmap){
	EssBitmap * bmp=(EssBitmap *)(long)bitmap;
	if(isEssBitmap(bmp)==NULL)
		return 0;
	int size=outputBufferSize < bmp->stride*bmp->height?outputBufferSize : bmp->stride*bmp->height;
	(*env)->SetIntArrayRegion(env,outputBufferPtr,0,size/4,(jint*)bmp->bits);
	return size/4;
}


jlong Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_createEssContext
	(JNIEnv * env, jobject thiz){
	return (jlong)(long)createEssContext();
}
void Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_deleteEssContext
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	deleteEssContext(&ctt);
	context=0;
}

void Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_deleteEssBitmap
	(JNIEnv * env, jobject thiz,jlong bitmap){
	EssBitmap * bmp=(EssBitmap *)(long)bitmap;
	deleteEssBitmap(&bmp);
	bitmap=0;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_setEssBitmapBuffer
	(JNIEnv * env, jobject thiz,jlong context,jint bpp,jint width,jint height,jbyteArray bitsBuffer){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=bpp*width/8;
	if(size%4>0)size+=4-size%4;
	size*=height;
	unsigned char * bits=(unsigned char *)malloc(size);
	if(bits==NULL)
		return 0;
	(*env)->GetByteArrayRegion(env,bitsBuffer,0,size,bits);
	int ret=setEssBitmapBuffer(ctt,bpp,width,height,bits);
	free(bits);
	return ret;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_setEssBitmapBuffer32Bpp
	(JNIEnv * env, jobject thiz,jlong context,jint bpp,jint width,jint height,jintArray bitsBuffer){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=bpp*width/8;
	if(size%4>0)size+=4-size%4;
	size*=height;
	unsigned char * bits=(unsigned char *)malloc(size);
	if(bits==NULL)
		return 0;
	LOGI("size=%d",size);
	(*env)->GetIntArrayRegion(env,bitsBuffer,0,size/4,(jint*)bits);
	LOGI("bpp=%d",bpp);
	int ret=setEssBitmapBuffer(ctt,bpp,width,height,bits);
	LOGI("width=%d",width);
	free(bits);
	return ret;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_createEssBitmapForOutput
	(JNIEnv * env, jobject thiz,jlong context,jint bitmapIndex,jint bpp){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return createEssBitmapForOutput(ctt,bitmapIndex,bpp);
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_setDifferRange
	(JNIEnv * env, jobject thiz,jlong context,jint value){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return setDifferRange(ctt,value);
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_setLuminanceSansitivity
	(JNIEnv * env, jobject thiz,jlong context,jint value){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return setLuminanceSansitivity(ctt,value);
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_setDenstyThreshold
	(JNIEnv * env, jobject thiz,jlong context,jint value){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return setDenstyThreshold(ctt,value);
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_setReadWithVersion
	(JNIEnv * env, jobject thiz,jlong context,jint version){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return setReadWithVersion(ctt,version);
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_setMinSize
	(JNIEnv * env, jobject thiz,jlong context,jint minSize){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return setMinSize(ctt,minSize);
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_readContext
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return readContext(ctt);
}
jlong Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getSourceEssBitmap
	(JNIEnv * env, jobject thiz,jlong context,jint bpp){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	EssBitmap * bmp=getSourceEssBitmap(ctt,bpp);
	return (jlong)(long)bmp;
}
jlong Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getBinaryEssBitmap
	(JNIEnv * env, jobject thiz,jlong context,jint bpp){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	EssBitmap * bmp=getBinaryEssBitmap(ctt,bpp);
	return (jlong)(long)bmp;
}
jlong Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getBalanceBinaryEssBitmap
	(JNIEnv * env, jobject thiz,jlong context,jint bpp){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	EssBitmap * bmp=getBalanceBinaryEssBitmap(ctt,bpp);
	return (jlong)(long)bmp;
}
jlong Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getOutputEssBitmap
	(JNIEnv * env, jobject thiz,jlong context,jint bpp){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	EssBitmap * bmp=getOutputEssBitmap(ctt,bpp);
	return (jlong)(long)bmp;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getVersion
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return ctt->version;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getLastErrorCode
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return (int)ctt->lastErrorCode;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getNumberDigit
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return ctt->numberDigit;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getSubjectDigit
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return ctt->subjectDigit;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getAnswerItems
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return ctt->answerItems;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getAnswerCount
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return ctt->answerCount;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_getCertainty
	(JNIEnv * env, jobject thiz,jlong context){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	return ctt->certainty;
}

jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copyNumberBuffer
	(JNIEnv * env, jobject thiz,jlong context,jbyteArray outputBufferPtr,jint outputBufferSize){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=ctt->numberDigit*10<outputBufferSize?ctt->numberDigit*10:outputBufferSize;
	(*env)->SetByteArrayRegion(env,outputBufferPtr,0,size,(jbyte*)ctt->numberBuffer);
	return size;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copySubjectBuffer
	(JNIEnv * env, jobject thiz,jlong context,jbyteArray outputBufferPtr,jint outputBufferSize){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=ctt->subjectDigit*10<outputBufferSize?ctt->subjectDigit*10:outputBufferSize;
	(*env)->SetByteArrayRegion(env,outputBufferPtr,0,size,(jbyte*)ctt->subjectBuffer);
	return size;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copyAnswerBuffer
	(JNIEnv * env, jobject thiz,jlong context,jbyteArray outputBufferPtr,jint outputBufferSize){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=ctt->answerCount*ctt->answerItems<outputBufferSize?ctt->answerCount*ctt->answerItems:outputBufferSize;
	(*env)->SetByteArrayRegion(env,outputBufferPtr,0,size,(jbyte*)ctt->answerBuffer);
	return size;
}

jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copyNumberCertaintiesBuffer
	(JNIEnv * env, jobject thiz,jlong context,jbyteArray outputBufferPtr,jint outputBufferSize){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=ctt->numberDigit*10<outputBufferSize?ctt->numberDigit*10:outputBufferSize;
	(*env)->SetByteArrayRegion(env,outputBufferPtr,0,size,(jbyte*)ctt->numberCertainties);
	return size;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copySubjectCertaintiesBuffer
	(JNIEnv * env, jobject thiz,jlong context,jbyteArray outputBufferPtr,jint outputBufferSize){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=ctt->subjectDigit*10<outputBufferSize?ctt->subjectDigit*10:outputBufferSize;
	(*env)->SetByteArrayRegion(env,outputBufferPtr,0,size,(jbyte*)ctt->subjectCertainties);
	return size;
}
jint Java_kingwaysoft_examsheetscanner_natives_ExamSheetScanner_copyAnswerCertaintiesBuffer
	(JNIEnv * env, jobject thiz,jlong context,jbyteArray outputBufferPtr,jint outputBufferSize){
	EssContext * ctt=(EssContext *)(long)context;
	if(ctt==NULL)
		return 0;
	int size=ctt->answerCount*ctt->answerItems < outputBufferSize?ctt->answerCount*ctt->answerItems:outputBufferSize;
	(*env)->SetByteArrayRegion(env,outputBufferPtr,0,size,(jbyte*)ctt->answerCertainties);
	return size;
}
