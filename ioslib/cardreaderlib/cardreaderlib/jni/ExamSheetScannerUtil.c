

#include "ExamSheetScannerUtil.h"
#include "ExamSheetScannerCore.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

//字符串拼接
int strjn(char ** str1,char * str2){
	if(str1==NULL)
		return 0;
	char * s1=*str1;
	char * s2=str2;
	if(s1==NULL && s2==NULL)
		return 0;
	int sl1=0;
	int sl2=0;
	if(s1!=NULL){
		while(s1[sl1]!=0){
			sl1++;
		}
		sl1++;
	}
	if(s2!=NULL){
		while(s2[sl2]!=0){
			sl2++;
		}
		sl2++;
	}
	if(s1!=NULL && s2==NULL)
		return sl1;
	int i,j;
	if(s1==NULL && s2!=NULL){
		s1=(char*)malloc(sl2);
		if(s1==NULL)
			return 0;
		for(i=0;i<sl2;i++){
			s1[i]=s2[i];
		}
		*str1=s1;
		return sl2;
	}
	int sl3=sl1+sl2-1;
	char * s3=(char*)malloc(sl3);
	if(s3==NULL)
		return sl1;
	for(i=0;i<sl1-1;i++)
		s3[i]=s1[i];
	for(j=0;j<sl2;j++){
		s3[i]=s2[j];
		i++;
	}
	free(*str1);
	*str1=s3;
	return sl3;
}
//答题卡格式信息
EssInformation * createEssInformation()
{
	EssInformation * ret=(EssInformation*)malloc(sizeof(EssInformation));
	if(ret==NULL)
		return NULL;
	ret->columns=0;
	ret->corners=NULL;
	ret->locations=NULL;
	ret->items=0;
	ret->rows=0;
	ret->answerStringLenght=0;
	ret->numberStringLength=0;
	return ret;
}

void deleteEssInformation(EssInformation ** information)
{
	if(information==NULL)
		return;
	if(*information==NULL)
		return;
	if((*information)->corners!=NULL)
		free((*information)->corners);
	EssLocations * locations=(*information)->locations;
	if(locations!=NULL){
		if(locations->bottom!=NULL)
			deleteEssLines(&locations->bottom);
		if(locations->horizontal!=NULL)
			deleteEssLines(&locations->horizontal);
		if(locations->left!=NULL)
			deleteEssLines(&locations->left);
		if(locations->right!=NULL)
			deleteEssLines(&locations->right);
		if(locations->top!=NULL)
			deleteEssLines(&locations->top);
		if(locations->verticalTop!=NULL)
			deleteEssLines(&locations->verticalTop);
		if(locations->verticalLower!=NULL)
			deleteEssLines(&locations->verticalLower);
		free((*information)->locations);
	}
	free(*information);
	*information=NULL;
}
//一版答题卡角点查找
int getVersion1Corners(EssContext * context,EssCornerArray * essCornerArray){

	if(context==NULL)
		return 0;
	EssBitmap * bitmap=context->binaryEssBitmap;
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	EssCornerArray * eca=essCornerArray;
	if(eca==NULL)
		return 0;
	EssInformation * information=createEssInformation();
	if(information==NULL)
		return 0;
	EssCorners * corners=(EssCorners *)malloc(sizeof(EssCorners));
	if(corners==NULL){
		deleteEssInformation(&information);
		return 0;
	}
	EssLocations * locations=(EssLocations*)malloc(sizeof(EssLocations));
	if(locations==NULL){
		deleteEssInformation(&information);
		free(corners);
		return 0;
	}
	EssBitmap * outBitmap=context->outputEssBitmap;
	int topIndex[2];//没有标志的边为顶边
	int lowerIndex[2];
	int found=1;
	int i,j,radius;
	int bit,bits[6];
	int retv=0;
	unsigned char tmp;
	EssPoint point;
	int rt;
	EssLines * topLines=NULL;
	EssLines * lines1=NULL;
	EssLines * lines2=NULL;
	EssLines * lines3=NULL;
	EssLines * lines4=NULL;
	EssLines * lines5=NULL;
	EssLines * lines6=NULL;
	EssLines * lines7=NULL;
	double currentLength,maxLength=0;
	int issw=0;
	int topLeft,  topRight,  lowerLeft,  lowerRight;
	int breakf,ret;
	ret=1;
	for(breakf=0;breakf<1;breakf++){
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(i!=j){
					topLines=getLines(bitmap,&eca->corners[i].point,&eca->corners[j].point,0);
					//saveEssBitmap(bitmap);
					if(topLines==NULL){
						ret=0;
						break;
					}
					found=topLines->count;
					deleteEssLines(&topLines);
					if(found==0){
						topIndex[0]=i;
						topIndex[1]=j;
						break;
					}
				}
			}
			if(found==0)
				break;
			if(ret==0)
				break;
		}
		if(ret==0 || found!=0)
			break;
		for(i=0;i<2;i++){//根据对角边最长找对角
			maxLength=0;
			for(j=0;j<4;j++){
				if(j!=topIndex[0] && j!=topIndex[1]){
					currentLength=sqrt(pow((double)(eca->corners[topIndex[i]].point.x-eca->corners[j].point.x),2)+pow((double)(eca->corners[topIndex[i]].point.y-eca->corners[j].point.y),2));
					if(currentLength>maxLength){
						maxLength=currentLength;
						lowerIndex[i]=j;
					}
				}
			}
		}
		i=lowerIndex[0];//除去对角，剩下的一个底边角就是邻角
		lowerIndex[0]=lowerIndex[1];
		lowerIndex[1]=i;
		//扫描两个侧边
		lines1=getLines(bitmap,&eca->corners[topIndex[0]].point,&eca->corners[lowerIndex[0]].point,0);
		lines2=getLines(bitmap,&eca->corners[topIndex[1]].point,&eca->corners[lowerIndex[1]].point,0);
		if(lines1==NULL || lines2==NULL)
			break;
		if(lines1->count<13 || lines2->count!=lines1->count)
			break;
		if(outBitmap!=NULL){
			radius=lines1->lines[lines1->count/2].length/2;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines1->count;i++){
				drawSign(outBitmap,&lines1->lines[i].center,radius,0x7f0000ff);
			}
			radius=lines2->lines[lines2->count/2].length/2;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines2->count;i++){
				drawSign(outBitmap,&lines2->lines[i].center,radius,0x7f0000ff);
			}
		}
		//扫描顶边
		lines3=getLines(bitmap,&lines1->lines[0].center,&lines2->lines[0].center,0);
		//扫描中间的水平边
		lines4=getLines(bitmap,&lines1->lines[11].center,&lines2->lines[11].center,0);
		if(lines3==NULL || lines4==NULL)
			break;
		if(lines3->count<8 || lines4->count!=lines3->count)
			break;
		if(outBitmap!=NULL){
			radius=lines3->lines[lines3->count/2].length/3;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines3->count;i++){
				drawSign(outBitmap,&lines3->lines[i].center,radius,0x7f0000ff);
			}
			radius=lines4->lines[lines4->count/2].length/3;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines4->count;i++){
				drawSign(outBitmap,&lines4->lines[i].center,radius,0x7f0000ff);
			}
		}
		lines5=getLines(bitmap,&lines3->lines[6].center,&lines4->lines[6].center,0);
		if(lines5==NULL)
			break;
		if(lines5->count!=10){
			deleteEssLines(&lines5);
			lines5=getLines(bitmap,&lines3->lines[lines3->count-7].center,&lines4->lines[lines4->count-7].center,0);
			if(lines5==NULL)
				break;
			if(lines5->count!=10)
				break;
			issw=1;
		}
		if(outBitmap!=NULL){
			radius=lines5->lines[lines5->count/2].length/3;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines5->count;i++){
				drawSign(outBitmap,&lines5->lines[i].center,radius,0x7f0000ff);
			}
		}
		deleteEssLines(&lines1);
		deleteEssLines(&lines2);
		deleteEssLines(&lines3);
		deleteEssLines(&lines4);
		deleteEssLines(&lines5);
		if(issw==0){//根据左侧特征确定四个角的关系
			topLeft=topIndex[0];
			topRight=topIndex[1];
			lowerLeft=lowerIndex[0];
			lowerRight=lowerIndex[1];
		}
		else{
			topLeft=topIndex[1];
			topRight=topIndex[0];
			lowerLeft=lowerIndex[1];
			lowerRight=lowerIndex[0];

		}
		memcpy(&corners->topLeft,&eca->corners[topLeft],sizeof(EssCorner));
		memcpy(&corners->topRight,&eca->corners[topRight],sizeof(EssCorner));
		memcpy(&corners->lowerLeft,&eca->corners[lowerLeft],sizeof(EssCorner));
		memcpy(&corners->lowerRight,&eca->corners[lowerRight],sizeof(EssCorner));
		//重新按顺序提取定位点
		lines1=getLines(bitmap,&corners->topLeft.point,&corners->lowerLeft.point,1);//左右
		lines2=getLines(bitmap,&corners->topRight.point,&corners->lowerRight.point,1);
		lines5=getLines(bitmap,&corners->lowerLeft.point,&corners->lowerRight.point,1);//底部
		if(lines1==NULL || lines2==NULL || lines5==NULL)
			break;
		lines3=getLines(bitmap,&lines1->lines[0].center,&lines2->lines[0].center,1);//顶部
		lines4=getLines(bitmap,&lines1->lines[11].center,&lines2->lines[11].center,1);//中间水平隔开准考证与答案的
		if(lines1->count<13 || lines1->count!=lines2->count || lines3->count<8 || lines3->count!=lines4->count || lines4->count!=lines5->count)
			break;
		lines6=getLines(bitmap,&lines3->lines[6].center,&lines4->lines[6].center,0);//准考证前面
		i=lines4->count/2-1;
		lines7=getLines(bitmap,&lines4->lines[i].center,&lines5->lines[i].center,0);//答案中间垂直的
		if(lines6==NULL || lines7==NULL)
			break;
		if(lines6->count+lines7->count+2!=lines1->count)//总数判断，准考证前面的+答案中间的=侧边的
			break;
		if(outBitmap!=NULL){
			radius=lines5->lines[lines5->count/2].length/2;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines5->count;i++){
				drawSign(outBitmap,&lines5->lines[i].center,radius,0x7f0000ff);
			}
			radius=lines6->lines[lines6->count/2].length/2;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines6->count;i++){
				drawSign(outBitmap,&lines6->lines[i].center,radius,0x7f0000ff);
			}
			radius=lines7->lines[lines7->count/2].length/2;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines7->count;i++){
				drawSign(outBitmap,&lines7->lines[i].center,radius,0x7f0000ff);
			}
		}
		rt=(int)(0.5*lines6->lines[5].length);
		if(rt<1)rt=1;
		for(i=0;i<6;i++){
			bits[i]=0;
			bit=1;
			for(j=0;j<10;j++){
				if(getIntersectionPoint(bitmap->width,bitmap->height,&lines1->lines[j+1].center,&lines6->lines[j].center,&lines3->lines[i].center,&lines4->lines[i].center,&point)==0){
					ret=0;
					break;
				}
				retv=readPoint(context,&point,rt,(double)context->denstyThreshold/100,&tmp);
				if(retv==0){
					ret=0;
					break;
				}
				if(outBitmap!=NULL)
					drawSign(outBitmap,&point,rt,0x7f0000ff);
				if(retv>0)
					bits[i]|=bit;
				bit<<=1;
			}
			if(ret==0)
				break;
		}
		if(ret==0)
			break;
		retv=bits[0];
		for(i=1;i<5;i++)
			retv^=bits[i];
		if(retv!=bits[5])
			break;
		locations->left=lines1;
		locations->right=lines2;
		locations->top=lines3;
		locations->horizontal=lines4;
		locations->bottom=lines5;
		locations->verticalTop=lines6;
		locations->verticalLower=lines7;
		i=(int)sqrt(pow((double)(corners->topRight.point.x-corners->topLeft.point.x),2)+pow((double)(corners->topRight.point.y-corners->topLeft.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		i=(int)sqrt(pow((double)(corners->lowerRight.point.x-corners->lowerLeft.point.x),2)+pow((double)(corners->lowerRight.point.y-corners->lowerLeft.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		i=(int)sqrt(pow((double)(corners->topLeft.point.x-corners->lowerLeft.point.x),2)+pow((double)(corners->topLeft.point.y-corners->lowerLeft.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		i=(int)sqrt(pow((double)(corners->topRight.point.x-corners->lowerRight.point.x),2)+pow((double)(corners->topRight.point.y-corners->lowerRight.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		information->locations=locations;
		information->corners=corners;
		context->numberDigit=bits[0];
		context->subjectDigit=bits[1];
		information->rows=bits[2];
		information->columns=bits[3];
		information->items=bits[4];
		context->answerCount=bits[2]*bits[3];
		context->answerItems=bits[4];
		if(context->information!=NULL)
			deleteEssInformation(&information);
		context->information=information;
		context->version=1;
		return 1;
	}
	deleteEssLines(&topLines);
	deleteEssLines(&lines1);
	deleteEssLines(&lines2);
	deleteEssLines(&lines3);
	deleteEssLines(&lines4);
	deleteEssLines(&lines5);
	deleteEssLines(&lines6);
	deleteEssLines(&lines7);
	deleteEssInformation(&information);
	free(locations);
	free(corners);
	return 0;
}
//二版答题卡角点查找
int getVersion2Corners(EssContext * context,EssCornerArray * essCornerArray){

	if(context==NULL)
		return 0;
	EssBitmap * bitmap=context->binaryEssBitmap;
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	EssCornerArray * eca=essCornerArray;
	if(eca==NULL)
		return 0;
	EssInformation * information=createEssInformation();
	if(information==NULL)
		return 0;
	EssCorners * corners=(EssCorners *)malloc(sizeof(EssCorners));
	if(corners==NULL){
		deleteEssInformation(&information);
		return 0;
	}
	EssBitmap * outBitmap=context->outputEssBitmap;
	int topIndex[2];//没有标志的边为顶边
	int lowerIndex[2];
	int found=1;
	int i,j;
	EssLines * topLines=NULL;
	EssLines *leftLines=NULL;
	EssLines *rightLines=NULL;
	EssLines *lowerLines=NULL;
	EssLines *lines1=NULL;
	EssLines *lines2=NULL;
	double currentLength,maxLength=0;
	int length1,length2,radius;
	int topLeft,  topRight,  lowerLeft,  lowerRight;
	int oneEnd=0;
	int breakf,ret;
	ret=0;
	for(breakf=0;breakf<1;breakf++){
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(i!=j){
					topLines=getLines(bitmap,&eca->corners[i].point,&eca->corners[j].point,0);
					//saveEssBitmap(bitmap);
					if(topLines==NULL){
						ret=1;
						break;
					}
					found=topLines->count;
					deleteEssLines(&topLines);
					if(found==0){
						topIndex[0]=i;
						topIndex[1]=j;
						break;
					}
				}
			}
			if(found==0 || ret==1)
				break;
		}
		if(found!=0 || ret==1)
			break;
		for(i=0;i<2;i++){//根据对角边最长找对角
			maxLength=0;
			for(j=0;j<4;j++){
				if(j!=topIndex[0] && j!=topIndex[1]){
					currentLength=sqrt(pow((double)(eca->corners[topIndex[i]].point.x-eca->corners[j].point.x),2)+pow((double)(eca->corners[topIndex[i]].point.y-eca->corners[j].point.y),2));
					if(currentLength>maxLength){
						maxLength=currentLength;
						lowerIndex[i]=j;
					}
				}
			}
		}
		i=lowerIndex[0];//除去对角，剩下的一个底边角就是邻角
		lowerIndex[0]=lowerIndex[1];
		lowerIndex[1]=i;
		//扫描两个侧边
		lines1=getLines(bitmap,&eca->corners[topIndex[0]].point,&eca->corners[lowerIndex[0]].point,0);
		if(lines1==NULL)
			break;
		if(outBitmap!=NULL){
			radius=lines1->lines[lines1->count/2].length;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines1->count;i++){
				drawSign(outBitmap,&lines1->lines[i].center,radius,0x7f0000ff);
			}
		}
		length1=lines1->maxLength;
		lines2=getLines(bitmap,&eca->corners[topIndex[1]].point,&eca->corners[lowerIndex[1]].point,0);
		if(lines2==NULL)
			break;
		if(outBitmap!=NULL){
			radius=lines2->lines[lines2->count/2].length;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lines2->count;i++){
				drawSign(outBitmap,&lines2->lines[i].center,radius,0x7f0000ff);
			}
		}
		length2=lines2->maxLength;
		//扫描底边
		lowerLines=getLines(bitmap,&eca->corners[lowerIndex[0]].point,&eca->corners[lowerIndex[1]].point,0);
		if(lowerLines==NULL || lines1->count<1 || lines2->count<1)
			break;
		if(outBitmap!=NULL){
			radius=lowerLines->lines[lowerLines->count/3].length;
			if(radius<0)radius*=-1;
			if(radius<1)radius=1;
			for(i=0;i<lowerLines->count;i++){
				drawSign(outBitmap,&lowerLines->lines[i].center,radius,0x7f0000ff);
			}
		}
		if(length1<length2){//根据左侧特征确定四个角的关系
			topLeft=topIndex[0];
			topRight=topIndex[1];
			lowerLeft=lowerIndex[0];
			lowerRight=lowerIndex[1];
			leftLines=lines1;
		}else{
			topLeft=topIndex[1];
			topRight=topIndex[0];
			lowerLeft=lowerIndex[1];
			lowerRight=lowerIndex[0];
			leftLines=lines2;

		}
		memcpy(&corners->topLeft,&eca->corners[topLeft],sizeof(EssCorner));
		memcpy(&corners->topRight,&eca->corners[topRight],sizeof(EssCorner));
		memcpy(&corners->lowerLeft,&eca->corners[lowerLeft],sizeof(EssCorner));
		memcpy(&corners->lowerRight,&eca->corners[lowerRight],sizeof(EssCorner));
		information->answerStringLenght=lowerLines->maxLength;
		deleteEssLines(&lines1);
		deleteEssLines(&lines2);
		//扫描左侧边，获得行数
		leftLines=getLines(bitmap,&corners->topLeft.point,&corners->lowerLeft.point,1);
		if(leftLines==NULL)
			break;
		if(leftLines->count<1 || leftLines->maxLength<1)
			break;
		//扫描右侧边获得准考证号位数+主观题位数的总位数
		information->rows=leftLines->count;
		deleteEssLines(&leftLines);
		rightLines=getLines(bitmap,&corners->topRight.point,&corners->lowerRight.point,1);
		if(rightLines==NULL)
			break;
		if(rightLines->count<1 || rightLines->maxLength<1)
			break;
		deleteEssLines(&lowerLines);
		lowerLines=getLines(bitmap,&corners->lowerLeft.point,&corners->lowerRight.point,1);
		if(lowerLines==NULL)
			break;
		if(lowerLines->count<1 || lowerLines->maxLength<1)
			break;
		context->numberDigit=0;//获取准考证号位数与主观题位数
		context->subjectDigit=0;
		information->numberStringLength=rightLines->maxLength;
		for(i=0;i<rightLines->count;i++){
			if(oneEnd==0)
				context->subjectDigit++;
			else
				context->numberDigit++;
			if(rightLines->lines[i].length==rightLines->maxLength)
				oneEnd=1;
		}
		information->columns=0;//获取列数与题目选项数
		information->items=0;
		oneEnd=0;
		for(i=0;i<lowerLines->count;i++){
			if(oneEnd==0)
				information->columns++;
			else
				information->items++;
			if(lowerLines->lines[i].length==lowerLines->maxLength)
				oneEnd=1;
		}
		deleteEssLines(&rightLines);
		deleteEssLines(&lowerLines);
		if(context->numberDigit<1 || information->items<1)
			break;
		context->answerItems=information->items;
		context->answerCount=information->rows*information->columns;
		//saveEssBitmap(bitmap);
		EssBitmap * obmp=context->outputEssBitmap;
		if(isEssBitmap(obmp)!=NULL){
			drawSign(obmp,&corners->topLeft.point,information->answerStringLenght/information->items/2,0x7f0000ff);
		}
		i=(int)sqrt(pow((double)(corners->topRight.point.x-corners->topLeft.point.x),2)+pow((double)(corners->topRight.point.y-corners->topLeft.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		i=(int)sqrt(pow((double)(corners->lowerRight.point.x-corners->lowerLeft.point.x),2)+pow((double)(corners->lowerRight.point.y-corners->lowerLeft.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		i=(int)sqrt(pow((double)(corners->topLeft.point.x-corners->lowerLeft.point.x),2)+pow((double)(corners->topLeft.point.y-corners->lowerLeft.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		i=(int)sqrt(pow((double)(corners->topRight.point.x-corners->lowerRight.point.x),2)+pow((double)(corners->topRight.point.y-corners->lowerRight.point.y),2));
		if(i<context->minSize){
			context->lastErrorCode=tooFarAway;
			break;
		}
		information->corners=corners;
		if(context->information!=NULL)
			deleteEssInformation(&information);
		context->information=information;
		context->version=2;
		//saveEssBitmap(bitmap);
		return 2;
	}
	deleteEssLines(&lines1);
	deleteEssLines(&lines2);
	deleteEssLines(&topLines);
	deleteEssLines(&lowerLines);
	deleteEssLines(&leftLines);
	deleteEssLines(&rightLines);
	deleteEssInformation(&information);
	free(corners);
	return 0;
}
//角点查找
int getCorners(EssContext * context)
{
	if(context==NULL)
		return 0;
	EssBitmap * bitmap=context->binaryEssBitmap;
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	EssBitmap * outBitmap=context->outputEssBitmap;
	EssCornerArray * eca=getCornerArray(bitmap,(double)context->differRange/100.0);
	//saveEssBitmap(bitmap);
	if(eca==NULL)
		return 0;
	int i,j,radius;
	if(outBitmap!=NULL && eca->count>0){
		radius=(eca->corners[0].rect.right-eca->corners[0].rect.left)/2;
		if(radius<0)radius*=-1;
		if(radius<1)radius=1;
		for(i=0;i<eca->count;i++){
			drawSign(outBitmap,&eca->corners[i].point,radius,0x7f0000ff);
		}
	}
	if(eca->count<4){
		context->lastErrorCode=lostCorner;
		free(eca->corners);
		free(eca);
		return 0;
	}

	for(i=0;i<eca->count-1;i++){
		for(j=i;j<eca->count;j++){
			if(eca->corners[j].rect.right  -  eca->corners[j].rect.left  >  eca->corners[i].rect.right  -  eca->corners[i].rect.left){
				swapCorner(&eca->corners[j],&eca->corners[i]);
			}
		}
	}
	j=0;
	for(i=1;i<eca->count;i++){
		if(likeCorner(&eca->corners[0],&eca->corners[i],(double)context->differRange/100.0))
			j++;
	}
	if(j!=3){
		context->lastErrorCode=lostCorner;
		free(eca->corners);
		free(eca);
		return 0;
	}
	//saveEssBitmap(bitmap);
	int x,y,s;
	unsigned char *sb,*sp;
	s=bitmap->stride;
	sb=bitmap->bits;
	if(eca->count>4){//修补误判的角点
		for(i=4;i<eca->count;i++){
			for(y=eca->corners[i].rect.bottom;y<=eca->corners[i].rect.top;y++){
				sp=sb + s * y + eca->corners[i].rect.left;
				for(x=eca->corners[i].rect.left;x<=eca->corners[i].rect.right;x++){
					if(*sp<200)*sp=0;
					sp++;
				}
			}
		}
	}
	int ret=0;
	if(context->withVersion==0){
		ret=getVersion1Corners(context,eca);
		if(ret==0)
			ret=getVersion2Corners(context,eca);
	}
	if(context->withVersion==1)
		ret=getVersion1Corners(context,eca);
	if(context->withVersion==2)
		ret=getVersion2Corners(context,eca);
	return ret;
}
//查找所有可能的角点
EssCornerArray * getCornerArray(EssBitmap * bitmap,double differRange){
	if(isEssBitmap(bitmap)==NULL)
		return NULL;
	int w,h,x,y;
	int s1,s2,s3,e;
	double l1,l2,l3,m,mc,mr,wc;
	EssCornerArray * acr;
	w=bitmap->width;
	h=bitmap->height;
	acr=(EssCornerArray *)malloc(sizeof(EssCornerArray));
	if(acr==NULL)
		return NULL;
	acr->corners=(EssCorner *)malloc(sizeof(EssCorner)*w);
	if(acr->corners ==NULL){
		free(acr);
		return NULL;
	}
	mr=1.0f/3.0f;
	mc=mr*differRange;
	wc=5.0f;
	acr->count=0;
	unsigned char * p=NULL;
	for(y=0;y<h;y++){
		s1=s2=s3=e=-1;
		l1=l2=l3=0;
		p=bitmap->bits +bitmap->stride * y;
		for(x=0;x<w;x++){
			if(*p<128 && s1<0 && s2<0 && s3<0 && e<0)s1=x;
			if(*p>128 && s1>=0 && s2<0 && s3<0 && e<0)s2=x;
			if(*p<128 && s1>=0 && s2>0 && s3<0 && e<0)s3=x;
			if(*p>128 && s1>=0 && s2>0 && s3>0 && e<0)e=x;
			if(s1>=0 && s2>0 && s3>0 && e>0){
				l1=(double)s2-s1;
				l2=(double)s3-s2;
				l3=(double)e-s3;
				if(l1>wc && l2>wc && l3>wc){
					m=l1+l2+l3;
					l1/=m;
					l2/=m;
					l3/=m;
					l1-=mr;
					l2-=mr;
					l3-=mr;
					if(l1<0.0f)l1*=-1;
					if(l2<0.0f)l2*=-1;
					if(l3<0.0f)l3*=-1;
					if(l1<mc && l2<mc && l3<mc ){
						if(isCorner(bitmap,s2+(s3-s2)/2,y,&acr->corners[acr->count],differRange)!=0){
							if(acr->count<w-1)
								acr->count++;
						}
					}
				}
				s1=s3;
				s2=e;
				s3=e=-1;
			}
			p++;
		}
	}
	return acr;
}
//角点特征近似判别
int likeCorner(EssCorner * corner1,EssCorner * corner2,double differRange){
	if(corner1==NULL || corner2==NULL)
		return 0;
	int pc1,pc2;
	pc1=(corner1->rect.right-corner1->rect.left)*(corner1->rect.top-corner1->rect.bottom);
	pc2=(corner2->rect.right-corner2->rect.left)*(corner2->rect.top-corner2->rect.bottom);
	/*if(abs(pc1-pc2)>(pc2+pc1)*0.25)
		return 0;*/
	if(fabs((double)pc1/(pc1+pc2)-(double)pc2/(pc1+pc2))>differRange)
		return 0;
	double l1,l2,sum,er;
	er=0.5*differRange;
	l1=(double)(corner1->rect.right-corner1->rect.left);
	l2=(double)(corner2->rect.right-corner2->rect.left);
	sum=l1+l2;
	l1/=sum;
	l2/=sum;
	if(l1-0.5>er || l2-0.5>er)
		return 0;
	l1=(double)(corner1->rect.top-corner1->rect.bottom);
	l2=(double)(corner2->rect.top-corner2->rect.bottom);
	sum=l1+l2;
	l1/=sum;
	l2/=sum;
	if(l1-0.5>er || l2-0.5>er)
		return 0;
	return 1;
}
int swapCorner(EssCorner * corner1,EssCorner * corner2){
	if(corner1==NULL || corner2==NULL)
		return 0;
	EssCorner swp;
	memcpy(&swp,corner1,sizeof(EssCorner));
	memcpy(corner1,corner2,sizeof(EssCorner));
	memcpy(corner2,&swp,sizeof(EssCorner));
	return 1;
}

//角点特征判别
int isCorner(EssBitmap * bitmap,int cdtX,int cdtY,EssCorner * corner,double differRange){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	if(corner==NULL)
		return 0;
	int w,h,x,y;
	int xs1,xs2,xs3,xe, ys1,ys2,ys3,ye;
	double xl1,xl2,xl3,xm, yl1,yl2,yl3,ym;
	double mc,mr,wc;
	int l,t,r,b,ca,ma;
	w=bitmap->width;
	h=bitmap->height;
	mr=1.0/3.0;
	mc=mr*differRange;
	wc=5.0;
	ys1=ys2=ys3=ye=-1;
	unsigned char * p=NULL;
	for(y=cdtY;y<h;y++){
		p= bitmap->bits + bitmap->stride * y + cdtX;
		if(*p<128 && ys3<0 && ye<0)ys3=y;
		if(*p>128 && ys3>0 && ye<0)ye=y;
		if(ys3>0 && ye>0)
			break;
	}
	for(y=cdtY;y>=0;y--){
		p= bitmap->bits + bitmap->stride * y + cdtX;
		if(*p<128 && ys2<0 && ys1<0)ys2=y;
		if(*p>128 && ys2>0 && ys1<0)ys1=y;
		if(ys2>0 && ys1>=0)
			break;
	}
	if(ys1<0 || ys2<0 || ys3<0 || ye<0)
		return 0;
	yl1=(double)ys2-ys1;
	yl2=(double)ys3-ys2;
	yl3=(double)ye-ys3;
	y=-1;
	if(yl1>wc && yl2>wc && yl3>wc){
		ym=yl1+yl2+yl3;
		yl1/=ym;
		yl2/=ym;
		yl3/=ym;
		yl1-=mr;
		yl2-=mr;
		yl3-=mr;
		if(yl1<0.0f)yl1*=-1;
		if(yl2<0.0f)yl2*=-1;
		if(yl3<0.0f)yl3*=-1;
		if(yl1<mc && yl2<mc && yl3<mc )
			y=ys2+(ys3-ys2)/2;
	}
	if(y<0)
		return 0;
	t=ys1;b=ye;
	xs1=xs2=xs3=xe=-1;
	for(x=cdtX;x>=0;x--){
		p=bitmap->bits + bitmap->stride * y + x;
		if(*p<128 && xs2<0 && xs1<0)xs2=x;
		if(*p>128 && xs2>0 && xs1<0)xs1=x;
		if(xs2>0 && xs1>=0)
			break;
	}
	for(x=cdtX;x<w;x++){
		p=bitmap->bits + bitmap->stride * y + x;
		if(*p<128 && xs3<0 && xe<0)xs3=x;
		if(*p>128 && xs3>0 && xe<0)xe=x;
		if(xs2>0 && xe>0)
			break;
	}
	if(xs1<0 || xs2<0 || xs3<0 || xe<0)
		return 0;
	xl1=(double)xs2-xs1;
	xl2=(double)xs3-xs2;
	xl3=(double)xe-xs3;
	x=-1;
	if(xl1>wc && xl2>wc && xl3>wc){
		xm=xl1+xl2+xl3;
		xl1/=xm;
		xl2/=xm;
		xl3/=xm;
		xl1-=mr;
		xl2-=mr;
		xl3-=mr;
		if(xl1<0.0f)xl1*=-1;
		if(xl2<0.0f)xl2*=-1;
		if(xl3<0.0f)xl3*=-1;
		if(xl1<mc && xl2<mc && xl3<mc )
			x=xs2+(xs3-xs2)/2;
	}
	if(x<0)
		return 0;
	l=xs1;r=xe;
	corner->point.x=x;
	corner->point.y=y;

	mr=1.0/6.0;
	mc=mr*differRange;
	yl1=(double)ys2-ys1;
	yl2=(double)ys3-ys2;
	yl3=(double)ye-ys3;
	xl1=(double)xs2-xs1;
	xl2=(double)xs3-xs2;
	xl3=(double)xe-xs3;
	xm=xl1+xl2+xl3+yl1+yl2+yl3;
	xl1/=xm;xl2/=xm;xl3/=xm;
	yl1/=xm;yl2/=xm;yl3/=xm;
	xl1-=mr;xl2-=mr;xl3-=mr;
	yl1-=mr;yl2-=mr;yl3-=mr;
	if(xl1<0.0f)xl1*=-1;if(xl2<0.0f)xl2*=-1;if(xl3<0.0f)xl3*=-1;
	if(yl1<0.0f)yl1*=-1;if(yl2<0.0f)yl2*=-1;if(yl3<0.0f)yl3*=-1;
	if(xl1>mc || xl2>mc || xl3>mc  || yl1>mc || yl2>mc || yl3>mc )
		return 0;
	ca=0;
	for(y=t;y<=b;y++){//面积检查
		p=bitmap->bits + bitmap->stride * y + l;
		for(x=l;x<=r;x++){
			if(*p<128)
				ca++;
			p++;
		}
	}
	ma=(int)((r-l)*(b-t)*0.6982057777777778);
	if(ma<1)
		return 0;
	if(abs(ca-ma)>ma*differRange)
		return 0;
	xe=(r-l)/6;
	ye=(b-t)/6;
	ca=0;
	ma=ye*4;
	for(y=t+ye;y<=b-ye;y++){//左侧边检查
		p=bitmap->bits + bitmap->stride * y + l+xe;
		if(*p<128)
			ca++;
		p++;
	}
	if(abs(ca-ma)>ma*differRange)
		return 0;
	ca=0;
	for(y=t+ye;y<=b-ye;y++){//右侧边检查
		p=bitmap->bits + bitmap->stride * y + l+xe*5;
		if(*p<128)
			ca++;
		p++;
	}
	if(abs(ca-ma)>ma*differRange)
		return 0;
	ca=0;
	ma=xe*4;
	for(x=l+xe;x<=r-xe;x++){//顶部边检查
		p=bitmap->bits + bitmap->stride * (t+ye*5) + x;
		if(*p<128)
			ca++;
		p++;
	}
	if(abs(ca-ma)>ma*differRange)
		return 0;
	ca=0;
	for(x=l+xe;x<=r-xe;x++){//底部边检查
		p=bitmap->bits + bitmap->stride * (t+ye) + x;
		if(*p<128)
			ca++;
		p++;
	}
	/*******************外侧检查*/
	xe=(r-l)/10;
	ye=(b-t)/10;
	if(xe<1)xe=1;
	if(ye<1)ye=1;
	ca=0;
	ma=ye*2;
	x=l-xe;
	if(x<0)x=0;
	if(x>=w)x=w-1;
	for(y=t;y<=b;y++){//左侧边检查
		p=bitmap->bits + bitmap->stride * y + x;
		if(*p<128)
			ca++;
		p++;
	}
	if(ca>ma)
		return 0;
	ca=0;
	x=r+xe;
	if(x<0)x=0;
	if(x>=w)x=w-1;
	for(y=t;y<=b;y++){//右侧边检查
		p=bitmap->bits + bitmap->stride * y + x;
		if(*p<128)
			ca++;
		p++;
	}
	if(ca>ma)
		return 0;
	ca=0;
	y=b+ye;
	if(y<0)y=0;
	if(y>=h)y=h-1;
	ma=xe*2;
	for(x=l;x<=r;x++){//顶部边检查
		p=bitmap->bits + bitmap->stride * y + x;
		if(*p<128)
			ca++;
		p++;
	}
	if(ca>ma)
		return 0;
	ca=0;
	y=t-ye;
	if(y<0)y=0;
	if(y>=h)y=h-1;
	for(x=l;x<=r;x++){//底部边检查
		p=bitmap->bits + bitmap->stride * y + x;
		if(*p<128)
			ca++;
		p++;
	}
	if(ca>ma)
		return 0;
	corner->rect.left=l;
	corner->rect.right=r;
	corner->rect.top=b;
	corner->rect.bottom=t;
	xe=abs(l-r)+abs(b-t);
	ye=abs(abs(l-r)-abs(b-t));
	if((double)ye/xe>0.25)
		return 0;
	for(y=t;y<=b;y++){//擦除检测到的同心圆
		p=bitmap->bits + bitmap->stride * y + l;
		for(x=l;x<=r;x++){
			if(*p>128)
				*p=200;
			else
				*p=150;
			p++;
		}
	}
	for(y=corner->point.y-2;y<corner->point.y+2;y++){
		p=bitmap->bits + bitmap->stride * y + corner->point.x-2;
		for(x=corner->point.x-2;x<corner->point.x+2;x++){
			if(y>=0 && y<h && x>=0 && x<w)
				*p=130;
			p++;
		}
	}
	return 1;
}
//定位点
EssLines * createEssLines(int count){
	if(count<1)
		return 0;
	EssLines * lines=NULL;
	lines=(EssLines *)malloc(sizeof(EssLines));
	if(lines==NULL)
		return NULL;
	lines->lines=(EssLine *)malloc(sizeof(EssLine)*count);
	if(lines->lines==NULL){
		free(lines);
		return NULL;
	}
	lines->count=count;
	return lines;
}
void deleteEssLines(EssLines ** lines){
	if(lines==NULL)
		return;
	if(*lines==NULL)
		return;
	if((*lines)->lines !=NULL )
		free((*lines)->lines);
	free(*lines);
	*lines=NULL;
}
/**
 * 扫描定位点
 * bitmap要扫描的图像
 * start 扫描的起始坐标
 * end 扫描的终止坐标
 * clear 0表示不清除图像中检测到的定位点
 */
EssLines * getLines(EssBitmap * bitmap,EssPoint * start,EssPoint * end,int clear){
	if(isEssBitmap(bitmap)==NULL || start==NULL || end ==NULL)
		return 0;
	int x,y,tw,tc,ti,tx,ty,i,s,sel,ml;
	unsigned char * b;
	unsigned char * sp;
	unsigned char * tp;
	double l,sn,cs;
	EssLines * lines=createEssLines(bitmap->width > bitmap->height ? bitmap->width : bitmap->height );
	if(lines==NULL)
		return NULL;
	s=bitmap->stride ;
	b=bitmap->bits;
	tw=(int)((bitmap->width > bitmap->height ? bitmap->width : bitmap->height) * 0.0011);
	if(tw<1)tw=1;
	ti=tw+1;
	tc=ti*ti;
	l=sqrt(pow((double)(end->y - start->y),2) + pow((double)(end->x - start->x),2));
	ml=(int)l/100;
	sn=(double)(end->y - start->y)/l;
	cs=(double)(end->x - start->x)/l;
	EssPoint  p1,p2;
	int fb=0;
	p1.x=p2.x=-1;
	lines->count=0;
	for(i=0;i<(int)l;i++){
		y=start->y + (int)(sn*i);
		x=start->x + (int)(cs*i);
		if(x<tw || x>=bitmap->width-tw || y<tw || y >=bitmap->height-tw)
			break;
		sp=b + s * y + x;
		ti=0;
		for(ty=y-tw;ty<=y+tw;ty++){
			tp=b + s * ty + x-tw;
			if(ty>=0 && ty<bitmap->height){
				for(tx=x-tw;tx<=x+tw;tx++){
					if(tx<0 && tx>=bitmap->width)
						break;
					if(*tp<128)
						ti++;
					if(ti>=tc)
						break;
					tp++;
				}
			}
		}
		if(ti<tc){
			ti=0;
			if(fb==0)
				fb=1;
		}
		if(ti>0 && fb>0 && p1.x<0 && p2.x<0){
			p1.x=x;p1.y=y;
		}
		if(ti==0 && fb>0 && p1.x>0 && p2.x<0){
			p2.x=x;p2.y=y;
		}
		if(p1.x>0 && p2.x >0){
			sel=(int)sqrt(pow((double)(p2.x-p1.x),2) + pow((double)(p2.y-p1.y),2));
			if(sel>ml){
				memcpy(&lines->lines[lines->count].start ,&p1,sizeof(EssPoint));
				memcpy(&lines->lines[lines->count].end ,&p2,sizeof(EssPoint));
				lines->lines[lines->count].length=sel;
				lines->count++;
			}
			if(clear)
				clearNearPoints(bitmap,p1.x+(p2.x-p1.x)/2,p1.y+(p2.y-p1.y)/2,NULL);
			p1.x=p2.x=-1;
		}
	}
	int maxLength=0;
	int currentLength=0;
	if(lines->count>0){
		for(i=0;i<lines->count;i++){
			currentLength=lines->lines[i].length;
			if(currentLength>maxLength)
				maxLength=currentLength;
		}
	}
	lines->maxLength=maxLength;
	if(lines->count>0){
		for(i=0;i<lines->count;i++){
			l=sqrt(pow((double)(lines->lines[i].end.x-lines->lines[i].start.x),2)+pow((double)(lines->lines[i].end.y-lines->lines[i].start.y),2));
			sn=(double)(lines->lines[i].end.y-lines->lines[i].start.y)/l;
			cs=(double)(lines->lines[i].end.x-lines->lines[i].start.x)/l;
			l/=2;
			lines->lines[i].center.x=lines->lines[i].start.x+(int)(cs*l);
			lines->lines[i].center.y=lines->lines[i].start.y+(int)(sn*l);
		}
	}
	//drawPoint(bitmap,&p1);
	//drawPoint(bitmap,&p2);
	//saveEssBitmap(bitmap);
	return lines;
}
//清除输入图像指定坐标的黑块points不为NULL则存入所有黑点的坐标
int clearNearPoints(EssBitmap * bitmap,int x,int y,EssPoints * points){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	if(x<0 || y <0 || x>=bitmap->width || y>=bitmap->height)
		return 0;
	int tx,ty,block,tWhileIdx,tSize,tCount,rSize,rCount,s,w,h;
	EssPoint * tps;
	unsigned char * b;
	unsigned char * sp;
	unsigned char * tp;
	void * tmp;
	b=bitmap->bits;
	s=bitmap->stride;
	w=bitmap->width;
	h=bitmap->height;
	block=1024;
	tps=(EssPoint *)malloc(sizeof(EssPoint)*block);
	if(tps==NULL)
		return 0;
	if(points!=NULL){
		if(points->points!=NULL)
			free(points->points);
		points->points=(EssPoint *) malloc(sizeof(EssPoint)*block);
		if(points->points==NULL){
			free(tps);
			return 0;
		}
	}
	tWhileIdx=0;
	tps[tWhileIdx].x=x;
	tps[tWhileIdx].y=y;
	tCount=1;
	tSize=block;

	rCount=0;
	rSize=block;
	while(tWhileIdx<tCount){
		sp=b + s * tps[tWhileIdx].y + tps[tWhileIdx].x;
		//*sp=150;
		for(ty=tps[tWhileIdx].y-1; ty<=tps[tWhileIdx].y+1; ty++){
			tp=b + s * ty + tps[tWhileIdx].x-1;
			if(ty>=0 && ty<h){
				for(tx=tps[tWhileIdx].x-1; tx<=tps[tWhileIdx].x+1; tx++){
					if(tx<0 || tx>=w)
						break;
					if(*tp<128){
						if(points!=NULL){
							points->points[rCount].x=tps[tWhileIdx].x;
							points->points[rCount].y=tps[tWhileIdx].y;
							rCount++;
							if(rCount>=rSize){
								rSize+=block;
								tmp=realloc(points->points,sizeof(EssPoint)*rSize);
								//points->points=(EssPoint *)realloc(points->points,sizeof(EssPoint)*rSize);
								if(tmp==NULL){
									free(points->points);
									free(tps);
									return 0;
								}
								points->points=(EssPoint *)tmp;
							}
						}
						*tp=150;
						tps[tCount].x=tx;
						tps[tCount].y=ty;
						tCount++;
						if(tCount>=tSize){
							tSize+=block;
							tmp=realloc(tps,sizeof(EssPoint)*tSize);
							//tps=(EssPoint *)realloc(tps,sizeof(EssPoint)*tSize);
							if(tmp==NULL){
								free(tps);
								free(points->points);
								points->points=NULL;
								return 0;
							}
							tps=(EssPoint *)tmp;
						}
					}
					tp++;
				}
			}
		}
		tWhileIdx++;
	}
	free(tps);
	if(points!=NULL){
		if(rCount<1){
			if(points->points!=NULL){
				free(points->points);
				points->points=NULL;
			}
		}
		points->count=rCount;
		return rCount;
	}
	else
		return 1;
}
//获取答题卡的所有填涂串
int getEssStrings(EssContext * context,EssStrings * numberStrings,EssStrings * answerStrings){
	if(context==NULL)
		return 0;
	EssBitmap * bbitmap=context->binaryEssBitmap;
	EssInformation * information=(EssInformation*)context->information;
	if(isEssBitmap(bbitmap)==0 || information==NULL || numberStrings==NULL || answerStrings==NULL)
		return 0;
	EssCorners * corners=information->corners;
	if(corners==NULL)
		return 0;
	int x,y,w,h,s,size;
	w=bbitmap->width;
	h=bbitmap->height;
	s=bbitmap->stride;
	unsigned char * sb=bbitmap->bits;
	unsigned char * sp;
	int nl,al,nlr,alr,mps,nps;
	EssString string;
	nl=context->numberDigit;
	al=context->answerCount;
	nlr=nl/8;
	alr=al/3;
	answerStrings->count=0;
	numberStrings->count=0;
	if(answerStrings->strings!=NULL){
		free(answerStrings->strings);
		answerStrings->strings=NULL;
	}
	if(numberStrings->strings!=NULL){
		free(numberStrings->strings);
		numberStrings->strings=NULL;
	}
	nps=(int)((w>h?w:h)*0.25);
	mps=(int)((w>h?w:h)*15.625);
	EssPoint * lps,* rps;
	int lpi,rpi,lpc,rpc;
	double lpr;
	lpc=(int)sqrt(pow((double)(corners->topLeft.point.x-corners->lowerLeft.point.x),2) + pow((double)(corners->topLeft.point.y-corners->lowerLeft.point.y),2));
	rpc=(int)sqrt(pow((double)(corners->topRight.point.x-corners->lowerRight.point.x),2) + pow((double)(corners->topRight.point.y-corners->lowerRight.point.y),2));
	lpr=(double)lpc/rpc;
	lps=(EssPoint *)malloc(sizeof(EssPoint)*lpc);
	if(lps==NULL){
		return 0;
	}
	rps=(EssPoint *)malloc(sizeof(EssPoint)*rpc);
	if(rps==NULL){
		free(lps);
		return 0;
	}
	double lsn,lcs,rsn,rcs;
	lsn=(double)(corners->topLeft.point.y-corners->lowerLeft.point.y)/lpc;
	lcs=(double)(corners->topLeft.point.x-corners->lowerLeft.point.x)/lpc;
	rsn=(double)(corners->topRight.point.y-corners->lowerRight.point.y)/rpc;
	rcs=(double)(corners->topRight.point.x-corners->lowerRight.point.x)/rpc;
	for(lpi=0;lpi<lpc;lpi++){
		lps[lpi].x=corners->lowerLeft.point.x+(int)(lcs*lpi);
		lps[lpi].y=corners->lowerLeft.point.y+(int)(lsn*lpi);
	}
	for(rpi=0;rpi<rpc;rpi++){
		rps[rpi].x=corners->lowerRight.point.x+(int)(rcs*rpi);
		rps[rpi].y=corners->lowerRight.point.y+(int)(rsn*rpi);
	}
	double fsn,fcs,fl;
	//saveEssBitmap(bbitmap);
	int bc;
	void * tmp;
	size=sizeof(EssString);
	EssBitmap * mask=createEssBitmap(8,w,h,NULL);
	if(mask==NULL){
		free(lps);
		free(rps);
		return 0;
	}
	for(lpi=0;lpi<lpc;lpi++){
		rpi=(int)((double)lpi/lpr);
		fl=sqrt(pow((double)(rps[rpi].x-lps[lpi].x),2) + pow((double)(rps[rpi].y-lps[lpi].y),2));
		fsn=(double)(rps[rpi].y-lps[lpi].y)/fl;
		fcs=(double)(rps[rpi].x-lps[lpi].x)/fl;
		rpc=(int)fl;
		for(rpi=0;rpi<rpc;rpi++){
			x=lps[lpi].x+(int)(fcs*rpi);
			y=lps[lpi].y+(int)(fsn*rpi);
			sp=sb+s*y+x;
			if(*sp<128){
				bc=getEssString(context,mask,x,y,&string);
				if(bc==information->items){
					tmp=realloc(answerStrings->strings,size*(answerStrings->count+1));
					if(tmp==NULL){
						deleteEssBitmap(&mask);
						free(answerStrings->strings);
						free(lps);
						free(rps);
						return 0;
					}
					answerStrings->strings=(EssString *)tmp;
					if(answerStrings->strings!=NULL){
						memcpy(&answerStrings->strings[answerStrings->count],&string,size);
						answerStrings->count++;
					}
				}
				if(bc==10){
					tmp=realloc(numberStrings->strings,size*(numberStrings->count+1));
					if(tmp==NULL){
						deleteEssBitmap(&mask);
						free(answerStrings->strings);
						free(lps);
						free(rps);
						return 0;
					}
					numberStrings->strings=(EssString *)tmp;
					if(numberStrings->strings!=NULL){
						memcpy(&numberStrings->strings[numberStrings->count],&string,size);
						numberStrings->count++;
					}
				}
			}
			//if(numberStrings->count==14 || answerStrings->count==99)
				//saveEssBitmap(bbitmap);
			//bbitmap->bpp=8;
			sp++;
		}
	}
	//saveEssBitmap(bbitmap);
	deleteEssBitmap(&mask);
	free(lps);
	free(rps);
	if(!sortEssStrings(context,numberStrings,answerStrings)){
		if(numberStrings->strings!=NULL)free(numberStrings->strings);
		numberStrings->strings=NULL;
		if(answerStrings->strings!=NULL)free(answerStrings->strings);
		answerStrings->strings=NULL;
		return 0;
	}
	return 1;
}
/**获取一个填涂串的所有填涂点的坐标
* context上下文
* mask填涂串的掩膜图
* string输入填涂串
* */
int getEssStringPoints(EssContext * context,EssBitmap * mask,EssString * string){
	if(context==NULL || mask==NULL)
		return 0;
	double sn,cs,csn,ccs,len,minStep,maxStep,step;
	int sx,sy,ex,ey,cx,cy,vx,vy,tx,ty,bx,by,tf,bf,i,j,clen;
	int bpp=mask->bpp;
	int stride=mask->stride;
	int width=mask->width;
	int height=mask->height;
	int pad=stride-(width*bpp/8);
	int bytes=bpp/8;
	unsigned char *bits=mask->bits;
	unsigned char *pointer=bits;
	sx=string->start.x;
	sy=string->start.y;
	ex=string->end.x;
	ey=string->end.y;
	len=sqrt(pow((double)(ex-sx),2)+pow((double)(ey-sy),2));
	sn=(double)(ey-sy)/len;
	cs=(double)(ex-sx)/len;
	maxStep=len/string->bitCount;
	minStep=maxStep/2;
	if(context->outputEssBitmap!=NULL){
		int rt=(int)(minStep*0.5);
		if(rt<1)rt=1;
		drawSign(context->outputEssBitmap,&string->start,rt,0xffff00ff);
		drawSign(context->outputEssBitmap,&string->end,rt,0xffff00ff);
	}
	if(minStep<0)
		return 0;
	step=minStep*1.1;
	string->step=(int)(minStep*0.75);
	clen=(int)(maxStep*2);

	/*double xs,ys,b,l,r,t,fbx,fby,fcx,fcy,fstep;
	int pc,k;
	fstep=0.0;*/
	for(i=0;i<string->bitCount;i++){
		tx=bx=cx=(int)(0.5+step*cs+sx);
		ty=by=cy=(int)(0.5+step*sn+sy);
		tf=bf=0;
		for(j=0;j<clen;j++){
			vx=cx-(int)(0.5+sn*j);
			vy=cy+(int)(0.5+cs*j);
			pointer=bits+stride *vy+vx*bytes;
			if(*pointer>128){
				tx=vx;
				ty=vy;
				tf=1;
			}
		}
		for(j=0;j<clen;j++){
			vx=cx+(int)(0.5+sn*j);
			vy=cy-(int)(0.5+cs*j);
			pointer=bits+stride *vy+vx*bytes;
			if(*pointer>128){
				bx=vx;
				by=vy;
				bf=1;
			}
		}
		if(tf>0 && bf>0){
			len=sqrt(pow((double)(bx-tx),2)+pow((double)(by-ty),2));
			if(len!=0){
				csn=(double)(by-ty)/len;
				ccs=(double)(bx-tx)/len;
				len/=2;
				cx=(int)(0.5+ccs*len+tx);
				cy=(int)(0.5+csn*len+ty);
			}
		}
		string->points[i].x=cx;
		string->points[i].y=cy;
		/*

		xs=vx=cx;ys=vy=cy;
		pc=1;
		l=width;
		b=height;
		r=0.0;
		t=0.0;
		for(j=(int)fstep;j<=(int)(fstep+maxStep);j++){
			fbx=cs*j+sx+sn*maxStep;
			fby=sn*j+sy-cs*maxStep;
			for(k=0;k<=(int)(maxStep*2);k++){
				fcx=fbx-sn*k;
				fcy=fby+cs*k;
				cx=(int)fcx;
				cy=(int)fcy;
				pointer=bits+stride*cy+cx;
				if(*pointer>128){
					xs+=fcx;
					ys+=fcy;
					pc++;
					if(fcx<l)l=fcx;
					if(fcx>r)r=fcx;
					if(fcy<b)b=fcy;
					if(fcy>t)t=fcy;
				}
			}
		}
		fcx=(l+r)/2;
		fcy=(t+b)/2;
		xs+=fcx;
		ys+=fcy;
		pc++;
		xs/=pc;
		ys/=pc;
		string->points[i].x=(int)((fcx+xs+vx)/3);
		string->points[i].y=(int)((fcy+ys+vy)/3);
		fstep+=maxStep;
		*/
		step+=maxStep;
	}
	//saveEssBitmap(mask);
	return 1;
}
/**
 * 根据指定坐标检索一个填涂串
 * context上下文
 * mask填涂串的掩膜图
 * xy要检索的坐标
 * string输出填涂串
 */
int getEssString(EssContext * context,EssBitmap * mask,int x,int y,EssString * string){
	if(context==NULL || mask==NULL)
		return 0;
	EssBitmap * bbitmap=context->binaryEssBitmap;
	EssInformation * information=(EssInformation*)context->information;
	if(isEssBitmap(bbitmap)==0 || information==NULL || string==NULL)
		return 0;
	EssCorners * corners=information->corners;
	if(corners==NULL)
		return 0;
	if(x<0 || y<0)
		return 0;
	EssPoints aps;
	aps.count=0;
	aps.points=NULL;
	if(!clearNearPoints(bbitmap,x,y,&aps)){
		if(aps.points!=NULL)free(aps.points);
		return 0;
	}
	int minCount,maxCount;
	minCount=information->answerStringLenght/information->items;
	minCount*=minCount;
	maxCount=information->numberStringLength/8;
	maxCount*=information->numberStringLength;
	//saveEssBitmap(bbitmap);
	if(aps.count<minCount || aps.count>maxCount){
		if(aps.points!=NULL)free(aps.points);
		return 0;
	}
	int i,cx,cy,l,r,t,b,w,h;
	l=b=1000000;
	t=r=0;
	for(i=0;i<aps.count;i++){
		cx=aps.points[i].x;
		cy=aps.points[i].y;
		if(cx<l)l=cx;
		if(cx>r)r=cx;
		if(cy<b)b=cy;
		if(cy>t)t=cy;
	}
	w=r-l+1;
	h=t-b+1;
	int px,py,sx,sy,ex,ey;
	double len,maxlen;
	px=(l+r)/2;
	py=(b+t)/2;
	maxlen=0;
	for(i=0;i<aps.count;i++){
		cx=aps.points[i].x;
		cy=aps.points[i].y;
		len=sqrt(pow((double)(px-cx),2)+pow((double)(py-cy),2));
		if(len>maxlen){
			sx=cx;sy=cy;
			maxlen=len;
		}
	}
	maxlen=0;
	for(i=0;i<aps.count;i++){
		cx=aps.points[i].x;
		cy=aps.points[i].y;
		len=sqrt(pow((double)(sx-cx),2)+pow((double)(sy-cy),2));
		if(len>maxlen){
			ex=cx;ey=cy;
			maxlen=len;
		}
	}
	maxlen=0;
	for(i=0;i<aps.count;i++){
		cx=aps.points[i].x;
		cy=aps.points[i].y;
		len=sqrt(pow((double)(ex-cx),2)+pow((double)(ey-cy),2));
		if(len>maxlen){
			sx=cx;sy=cy;
			maxlen=len;
		}
	}
	len=sqrt(pow((double)(ex-sx),2)+pow((double)(ey-sy),2));
	int itemType=0;
	if(fabs(len-(double)information->answerStringLenght)<0.35*information->answerStringLenght)
		itemType=information->items;
	if(fabs(len-(double)information->numberStringLength)<0.125*information->numberStringLength)
		itemType=10;
	if(itemType==0){
		if(aps.points!=NULL)free(aps.points);
		return 0;
	}
	if(itemType==context->answerItems){
		cx=corners->lowerLeft.point.x;
		cy=corners->lowerLeft.point.y;
	}
	else{
		cx=corners->topRight.point.x;
		cy=corners->topRight.point.y;
	}
	len=sqrt(pow((double)(sx-cx),2)+pow((double)(sy-cy),2));
	maxlen=sqrt(pow((double)(ex-cx),2)+pow((double)(ey-cy),2));
	if(len>maxlen){
		cx=sx;cy=sy;
		sx=ex;sy=ey;
		ex=cx;ey=cy;
	}
	int bpp=mask->bpp;
	int stride=mask->stride;
	int width=mask->width;
	int height=mask->height;
	int bytes=bpp/8;
	unsigned char *bits=mask->bits;
	unsigned char *pointer=bits;
	for(i=0;i<aps.count;i++){
		cx=aps.points[i].x;
		cy=aps.points[i].y;
		pointer=bits+stride*cy+cx*bytes;
		*pointer=0xff;
	}
	string->start.x=sx;
	string->start.y=sy;
	string->end.x=ex;
	string->end.y=ey;
	string->bitCount=itemType;
	if(getEssStringPoints(context,mask,string)==0){
		if(aps.points!=NULL)free(aps.points);
		return 0;
	}
	for(i=0;i<aps.count;i++){
		cx=aps.points[i].x;
		cy=aps.points[i].y;
		pointer=bits+stride*cy+cx*bytes;
		*pointer=0;
	}
	if(aps.points!=NULL)free(aps.points);
	if(readEssString(context,string)!=0)
		return itemType;
	return 0;
}
//排列所有的填涂串
int sortEssStrings(EssContext * context,EssStrings * numberStrings,EssStrings * answerStrings){
	if(context==NULL)
		return 0;
	EssInformation * information=(EssInformation*)context->information;
	if(information==NULL)
		return 0;
	EssCorners * corners=information->corners;
	if(numberStrings==NULL || answerStrings==NULL || corners==NULL)
		return 0;
	EssString swp;
	int i,j,cx,cy,s;
	double il,jl,ml;
	s=sizeof(EssString);
	cx=corners->topRight.point.x;
	cy=corners->topRight.point.y;
	for(i=0;i<numberStrings->count-1;i++){
		il=sqrt(pow((double)(numberStrings->strings[i].start.x-cx),2) + pow((double)(numberStrings->strings[i].start.y-cy),2));
		for(j=i+1;j<numberStrings->count;j++){
			jl=sqrt(pow((double)(numberStrings->strings[j].start.x-cx),2) + pow((double)(numberStrings->strings[j].start.y-cy),2));
			if(jl>il){
				il=jl;
				memcpy(&swp,&numberStrings->strings[i],s);
				memcpy(&numberStrings->strings[i],&numberStrings->strings[j],s);
				memcpy(&numberStrings->strings[j],&swp,s);
			}
		}
	}
	//saveEssBitmap(context->binaryEssBitmap);
	if(numberStrings->count!=context->numberDigit+context->subjectDigit)
		return 0;

	int cols=information->rows;
	EssPoint * cp;
	int ged[1024];
	EssString *rows[1024]={NULL};
	EssString *sorted[1024]={NULL};
	int coli,rowi,ci,ati;
	ml=sqrt(pow((double)(corners->topLeft.point.x-corners->lowerLeft.point.x),2) + pow((double)(corners->topLeft.point.y-corners->lowerLeft.point.y),2))+
		sqrt(pow((double)(corners->topLeft.point.x-corners->topRight.point.x),2) + pow((double)(corners->topLeft.point.y-corners->topRight.point.y),2));
	ati=0;
	ci=0;
	for(i=0;i<1024;i++)
		ged[i]=1;
	for(rowi=0;rowi<information->columns;rowi++){
		cp=&corners->lowerLeft.point;
		for(coli=0;coli<cols;coli++){
			jl=ml;
			for(i=0;i<answerStrings->count;i++){
				if(ged[i]){
					il=sqrt(pow((double)(cp->x-answerStrings->strings[i].start.x),2) + pow((double)(cp->y-answerStrings->strings[i].start.y),2));
					if(il<jl){
						ci=i;
						jl=il;
					}
				}
			}
			rows[coli]=&answerStrings->strings[ci];
			ged[ci]=0;
			cp=&answerStrings->strings[ci].start;
		}
		for(coli=cols-1;coli>=0;coli--){
			sorted[ati]=rows[coli];
			ati++;
		}
	}
	if(answerStrings->count!=context->answerCount)
		return 0;

	if(ati!=answerStrings->count)
		return 0;
	EssString * string=(EssString *)malloc(s*answerStrings->count);
	if(string==NULL)
		return 0;
	for(i=0;i<answerStrings->count;i++){
		memcpy(&string[i],sorted[i],s);
	}
	free(answerStrings->strings);
	answerStrings->strings=string;
	return 1;
}
//读取所有填涂串
int readEssStrings(EssContext * context){
	if(context==NULL )
		return 0;
	EssStrings  numberStrings,answerStrings;
	numberStrings.strings=NULL;
	answerStrings.strings=NULL;
	if(!getEssStrings(context,&numberStrings,&answerStrings))
		return 0;
	int i,j,k,ret;
	unsigned char cen,minCen=100;
	ret=1;
	j=0;
	unsigned char * numberBuf=NULL;
	unsigned char * numberCen=NULL;
	unsigned char * subjectBuf=NULL;
	unsigned char * subjectCen=NULL;
	unsigned char * answerBuf=NULL;
	unsigned char * answerCen=NULL;
	numberBuf=(unsigned char *)malloc(context->numberDigit*10);
	if(numberBuf==NULL){
		context->lastErrorCode=outMemory;
		free(numberStrings.strings);
		numberStrings.strings=NULL;
		free(answerStrings.strings);
		answerStrings.strings=NULL;
		return 0;
	}
	numberCen=(unsigned char *)malloc(context->numberDigit*10);
	if(numberCen==NULL){
		free(numberBuf);
		context->lastErrorCode=outMemory;
		free(numberStrings.strings);
		numberStrings.strings=NULL;
		free(answerStrings.strings);
		answerStrings.strings=NULL;
		return 0;
	}
	answerBuf=(unsigned char *)malloc(context->answerCount*context->answerItems);
	if(answerBuf==NULL){
		free(numberBuf);
		free(numberCen);
		context->lastErrorCode=outMemory;
		free(numberStrings.strings);
		numberStrings.strings=NULL;
		free(answerStrings.strings);
		answerStrings.strings=NULL;
		return 0;
	}
	answerCen=(unsigned char *)malloc(context->answerCount*context->answerItems);
	if(answerCen==NULL){
		free(answerBuf);
		free(numberBuf);
		free(numberCen);
		context->lastErrorCode=outMemory;
		free(numberStrings.strings);
		numberStrings.strings=NULL;
		free(answerStrings.strings);
		answerStrings.strings=NULL;
		return 0;
	}
	j=0;
	for(i=0;i<context->numberDigit;i++){
		for(k=0;k<10;k++){
			numberBuf[j]=numberStrings.strings[i].bitBytes[k];
			cen=numberStrings.strings[i].certainty[k];
			numberCen[j]=cen;
			if(cen<minCen && numberStrings.strings[i].bitBytes[k]>0)minCen=cen;
			j++;
		}
	}
	if(context->subjectDigit>0){
		subjectBuf=(unsigned char *)malloc(context->subjectDigit*10);
		if (subjectBuf==NULL)
		{
			free(numberBuf);
			free(numberCen);
			free(answerBuf);
			free(answerCen);
			context->lastErrorCode=outMemory;
			free(numberStrings.strings);
			numberStrings.strings=NULL;
			free(answerStrings.strings);
			answerStrings.strings=NULL;
			return 0;
		}
		subjectCen=(unsigned char *)malloc(context->subjectDigit*10);
		if (subjectCen==NULL)
		{
			free(subjectBuf);
			free(numberBuf);
			free(numberCen);
			free(answerBuf);
			free(answerCen);
			context->lastErrorCode=outMemory;
			free(numberStrings.strings);
			numberStrings.strings=NULL;
			free(answerStrings.strings);
			answerStrings.strings=NULL;
			return 0;
		}
		j=0;
		for(i=context->numberDigit;i<numberStrings.count;i++){
			for(k=0;k<10;k++){
				subjectBuf[j]=numberStrings.strings[i].bitBytes[k];
				cen=numberStrings.strings[i].certainty[k];
				subjectCen[j]=cen;
				if(cen<minCen )minCen=cen;
				j++;
			}
		}
	}
	j=0;
	minCen=100;
	for(i=0;i<context->answerCount;i++){
		for (k=0;k<context->answerItems;k++)
		{
			answerBuf[j]=answerStrings.strings[i].bitBytes[k];
			cen=answerStrings.strings[i].certainty[k];
			answerCen[j]=cen;
			if(cen<minCen )minCen=cen;
			j++;
		}
	}
	free(numberStrings.strings);
	numberStrings.strings=NULL;
	free(answerStrings.strings);
	answerStrings.strings=NULL;
	context->numberBuffer=numberBuf;
	context->numberCertainties=numberCen;
	context->subjectBuffer=subjectBuf;
	context->subjectCertainties=subjectCen;
	context->answerBuffer=answerBuf;
	context->answerCertainties=answerCen;
	context->certainty=minCen;
	return 1;
}
//读取一个填涂串
int readEssString(EssContext * context,EssString * string){
	if(context==NULL  || string==NULL)
		return 0;
	EssBitmap * outBitmap=context->outputEssBitmap;
	double denstyThreshold=(double)context->denstyThreshold/100.0;
	int i,retv;
	for(i=0;i<string->bitCount;i++){
		retv=readPoint(context,&string->points[i],(int)string->step,denstyThreshold,&string->certainty[i]);
		if(retv>0)
			string->certainty[i]=100;
		if(retv>0)
			string->bitBytes[i]=0xff;
		else if(retv<0)
			string->bitBytes[i]=0;
		else
			return 0;
	}
	return 1;
}
/**获取四个定位点的交点坐标
 * width、height坐标的有效范围，不能超出图像的尺寸
 * line1Start, line1End, line2Start, line2End两条线段的顶点坐标
 * pointOut 输出交点坐标
 */
int getIntersectionPoint(int width,int height,EssPoint * line1Start,EssPoint * line1End,EssPoint * line2Start,EssPoint * line2End,EssPoint * pointOut){
	double A1,A2,B1,B2,C1,C2;
	A1=line1End->y-line1Start->y;
	B1=line1Start->x-line1End->x;
	C1=line1End->x*line1Start->y-line1Start->x*line1End->y;
	A2=line2End->y-line2Start->y;
	B2=line2Start->x-line2End->x;
	C2=line2End->x*line2Start->y-line2Start->x*line2End->y;
	/*A = Y2 - Y1
	B = X1 - X2
	C = X2*Y1 - X1*Y2*/
	double d = A2 * B1 - A1 * B2;
	if (d == 0)
		return 0;
	double x,y;
	x = (B2 * C1 - B1 * C2) / d;
	y = (A1 * C2 - A2 * C1) / d;

	if(x<0 || y<0 || x>=width || y>=height)
		return 0;
	pointOut->x=(int)x;
	pointOut->y=(int)y;

	return 1;
}
/**
 * 自信度范围转换
 * writed 填涂与否
 * centainty 自信度百分比
 */
unsigned char getCertaintyRange(unsigned char  centainty) {
	unsigned char cen = centainty;
	unsigned char ret=0;
	if(cen==100){//已填
		ret=4;
	}
	else{//未填
		cen+=20;//误差补偿
		ret=(unsigned char)(cen*0.04);//自信度分4等级
		if(ret>4)ret=4;
	}
	return ret;
}
/**
 * 读取一个填涂点
 * context上下文
 * point要读取的坐标
 * radius要读取的半径
 * denstyThreshold根据这个密度判别填涂与否
 * centainty读取后的自信度
 * 返回1表示已填、-1表示未填、0表示函数调用失败
 */
int readPoint(EssContext * context,EssPoint * point,int radius,double denstyThreshold,unsigned char * centainty){
	if(context==NULL || point==NULL || radius<1 || centainty==NULL)
		return 0;
	EssBitmap * bitmap=context->balanceBinaryEssBitmap;
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	EssBitmap * outBitmap=context->outputEssBitmap;
	int w=bitmap->width;
	int h=bitmap->height;
	int px=point->x;
	int py=point->y;
	int pxs=px-radius;
	int pxe=px+radius;
	int pys=py-radius;
	int pye=py+radius;
	if(px<0 || py<0 || pxs<0 || pys<0 || px>w || py>h || pxe>w || pye>h)
		return 0;
	EssBitmap * cbmp=getFillCircleEssBitmap(radius,255,0);
	//saveEssBitmap(cbmp);
	if(cbmp==NULL)
		return 0;
	int pw=cbmp->width;
	int ph=cbmp->height;
	int x,y;
	int bs=bitmap->stride;
	int cs=cbmp->stride;
	unsigned char * bb=bitmap->bits;
	unsigned char * cb=cbmp->bits;
	unsigned char * bp,*cp;
	int pc,bc;
	pc=bc=0;
	for(y=0;y<ph;y++){
		bp=bb+bs*(pys+y)+pxs;
		cp=cb+cs*y;
		for(x=0;x<pw;x++){
			if(*cp>0){
				if(*bp<128)
					pc++;
				bc++;
			}
			bp++;
			cp++;
		}
	}
	deleteEssBitmap(&cbmp);
	double r=(double)pc/bc;
	int cty=100;
	int ret=0;
	if(r>=denstyThreshold){
		cty=100;
		/*cty=(int)(((r-denstyThreshold)/(1.0-denstyThreshold))*100);
		if(cty<0)cty=0;
		if(cty>100)cty=100;*/
		*centainty=(unsigned char)cty;
		ret= 1;
	}
	else{
		r*=0.9;
		cty=(int)(((denstyThreshold-r)/denstyThreshold)*100);
		if(cty<0)cty=0;
		if(cty>100)cty=100;
		*centainty=(unsigned char)cty;
		ret= -1;
	}
	unsigned int color=0x7f000000;
	unsigned int red,green;
	if(outBitmap!=NULL){
		red=100-cty;
		red*=0xff;
		red/=100;
		if(red>0xff)
			red=0xff;
		red<<=16;
		green=cty;
		green*=0xff;
		green/=100;
		if(green>0xff)
			green=0xff;
		green<<=8;
		drawSign(outBitmap,point,radius,color | red | green);
	}
	return ret;
}
//读取所有填涂点
int readPoints(EssContext * context){
	if(context==NULL)
		return 0;
	if(context->version!=1)
		return 0;
	EssInformation * information=(EssInformation*)context->information;
	if(information==NULL)
		return 0;
	EssLocations * location=information->locations;
	if(location==NULL)
		return 0;
	EssBitmap * bitmap=context->balanceBinaryEssBitmap;
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	int width=bitmap->width;
	int height=bitmap->height;
	int i,j,k,l,m,radius,col,row,item,retv,cs;
	double rr=0.7;
	EssPoint point;
	unsigned char * number=NULL;
	unsigned char * numberCen=NULL;
	unsigned char * subject=NULL;
	unsigned char * subjectCen=NULL;
	unsigned char * answer=NULL;
	unsigned char * answerCen=NULL;
	unsigned char * cenPtr;
	unsigned char minCen=100;
	double dt=(double)context->denstyThreshold/100.0*0.6;
	i=j=0;
	for(k=0;k<location->verticalLower->count;k++){
		i+=location->verticalLower->lines[k].length;
		j++;
	}
	for(k=0;k<location->verticalTop->count;k++){
		i+=location->verticalTop->lines[k].length;
		j++;
	}
	radius=(int)(rr*(i/j));
	if (radius<1)
		radius=1;
	if(context->numberDigit>0){
		number=(unsigned char*)malloc(context->numberDigit*10);
		if(number==NULL){
			context->lastErrorCode=outMemory;
			return 0;
		}
		for(i=0;i<context->numberDigit*10;i++)
			number[i]=0;
		numberCen=(unsigned char*)malloc(context->numberDigit*10);
		if(number==NULL){
			free(number);
			context->lastErrorCode=outMemory;
			return 0;
		}
		for(i=0;i<context->numberDigit*10;i++)
			numberCen[i]=0;
		row=10;
		col=context->numberDigit;
		cenPtr=numberCen;
		k=0;
		for(i=0;i<col;i++){
			for(j=0;j<row;j++){
				if(getIntersectionPoint(width,height,&location->verticalTop->lines[j].center,&location->right->lines[j+1].center,&location->top->lines[i+7].center,&location->horizontal->lines[i+7].center,&point)==0){
					context->lastErrorCode=unknowError;
					free(number);
					free(numberCen);
					return 0;
				}
				retv=readPoint(context,&point,radius,dt,cenPtr);
				if(retv==0){
					context->lastErrorCode=unknowError;
					free(number);
					free(numberCen);
					return 0;
				}
				if(retv==1)
					number[k]=0xff;
				else
					number[k]=0;
				if(retv>0)*cenPtr=100;
				if(*cenPtr<minCen && retv==1)minCen=*cenPtr;
				cenPtr++;
				k++;
			}
		}
	}
	cs=7+context->numberDigit+1;
	if(context->subjectDigit>0){
		subject=(unsigned char*)malloc(context->subjectDigit*10);
		if(subject==NULL){
			context->lastErrorCode=outMemory;
			if(number!=NULL)
				free(number);
			if(numberCen!=NULL)
				free(numberCen);
			return 0;
		}
		for(i=0;i<context->subjectDigit*10;i++)
			subject[i]=0;
		subjectCen=(unsigned char*)malloc(context->subjectDigit*10);
		if(subjectCen==NULL){
			context->lastErrorCode=outMemory;
			free(subject);
			if(number!=NULL)
				free(number);
			if(numberCen!=NULL)
				free(numberCen);
			return 0;
		}
		for(i=0;i<context->subjectDigit*10;i++)
			subjectCen[i]=0;
		row=10;
		col=context->subjectDigit;
		cenPtr=subjectCen;
		k=0;
		for(i=0;i<col;i++){
			for(j=0;j<row;j++){
				if(getIntersectionPoint(width,height,&location->verticalTop->lines[j].center,&location->right->lines[j+1].center,&location->top->lines[i+cs].center,&location->horizontal->lines[i+cs].center,&point)==0){
					context->lastErrorCode=unknowError;
					free(subject);
					free(subjectCen);
					if(number!=NULL)
						free(number);
					if(numberCen!=NULL)
						free(numberCen);
					return 0;
				}
				retv=readPoint(context,&point,radius,dt,cenPtr);
				if(retv==0){
					context->lastErrorCode=unknowError;
					free(subject);
					free(subjectCen);
					if(number!=NULL)
						free(number);
					if(numberCen!=NULL)
						free(numberCen);
					return 0;
				}
				if(retv==1)
					subject[k]=0xff;
				else
					subject[k]=0;
				if(retv>0)
					*cenPtr=100;
				if(*cenPtr<minCen && retv==1)minCen=*cenPtr;
				cenPtr++;
				k++;
			}
		}
	}
	col=information->columns;
	row=information->rows;
	item=information->items;
	answer=(unsigned char *)malloc(context->answerCount*context->answerItems);
	if(answer==NULL){
		context->lastErrorCode=outMemory;
		if(number!=NULL)
			free(number);
		if(numberCen!=NULL)
			free(numberCen);
		if(subject!=NULL)
			free(subject);
		if(subjectCen!=NULL)
			free(subjectCen);
		return 0;
	}
	for(i=0;i<context->answerCount*context->answerItems;i++)
		answer[i]=0;
	answerCen=(unsigned char *)malloc(context->answerCount*item);
	if(answerCen==NULL){
		context->lastErrorCode=outMemory;
		free(answer);
		if(number!=NULL)
			free(number);
		if(numberCen!=NULL)
			free(numberCen);
		if(subject!=NULL)
			free(subject);
		if(subjectCen!=NULL)
			free(subjectCen);
		return 0;
	}
	for(i=0;i<context->answerCount*context->answerItems;i++)
		answerCen[i]=0;
	cenPtr=answerCen;
	cs=location->horizontal->count/2-1;
	EssLine * ll,*vl,*rl,*tl,*bl,*ml;
	ll=&location->left->lines[cs];
	vl=location->verticalLower->lines;
	rl=&location->right->lines[cs];
	tl=location->horizontal->lines;
	bl=location->bottom->lines;
	minCen=100;
	l=0;
	for(i=0;i<row;i++){
		m=i*item;
		for(j=0;j<col+1;j++){
			if(j<cs || j>cs){
				if(j<cs)
					ml=vl;
				if(j>cs)
					ml=rl;
				for(k=0;k<item;k++){
					if(getIntersectionPoint(width,height,&ll[k+m].center,&ml[k+m].center,&tl[j].center,&bl[j].center,&point)==0){
						context->lastErrorCode=unknowError;
						free(answer);
						free(answerCen);
						if(number!=NULL)
							free(number);
						if(numberCen!=NULL)
							free(numberCen);
						if(subject!=NULL)
							free(subject);
						if(subjectCen!=NULL)
							free(subjectCen);
						return 0;
					}
					retv=readPoint(context,&point,radius,dt,cenPtr);
					if(retv==0){
						context->lastErrorCode=unknowError;
						free(answer);
						free(answerCen);
						if(number!=NULL)
							free(number);
						if(numberCen!=NULL)
							free(numberCen);
						if(subject!=NULL)
							free(subject);
						if(subjectCen!=NULL)
							free(subjectCen);
						return 0;
					}
					if(retv==1)
						answer[l]=0xff;
					else
						answer[l]=0;
					if(retv>0)
						*cenPtr=100;
					if(*cenPtr<minCen )minCen=*cenPtr;
					cenPtr++;
					l++;
				}
			}
		}
	}
	context->numberBuffer=number;
	context->numberCertainties=numberCen;
	context->subjectBuffer=subject;
	context->subjectCertainties=subjectCen;
	context->answerBuffer=answer;
	context->answerCertainties=answerCen;
	context->certainty=minCen;
	return 1;
}
//*/**********************************绘制一个圆点********************************************************************************/


int drawSign(EssBitmap * bitmap,EssPoint * point,int radius,unsigned int colorARGB){
	if(isEssBitmap(bitmap)==NULL || point==NULL || radius<1)
		return 0;
	EssBitmap *sbmp=getFillCircleEssBitmap(radius,255,0);
	if(sbmp==NULL)
		return 0;
	int dBpp=bitmap->bpp;
	int dStride=bitmap->stride;
	int dWidth=bitmap->width;
	int dHeight=bitmap->height;
	int sStride=sbmp->stride;
	int sWidth=sbmp->width;
	int sHeight=sbmp->height;
	unsigned char *db,*dp,*sb,*sp;
	int x,y,cc,ci,c[4],cv,v;
	int xs,ys;
	double r,fr;
	xs=point->x-radius;
	ys=point->y-radius;
	if(xs<0 || xs+sWidth>dWidth || ys<0 || ys+sHeight>dHeight)
		return 0;
	c[3]=(colorARGB>>24)&0xff;//A
	c[2]=(colorARGB>>16)&0xff;//R
	c[1]=(colorARGB>>8)&0xff;//G
	c[0]=colorARGB&0xff;//B
	r=(double)c[3]/255.0;
	fr=1.0-r;
	db=bitmap->bits;
	sb=sbmp->bits;
	cc=dBpp/8;
	if(dBpp==32)cc--;
	if(dBpp==8){
		cv=(int)(r*(c[2]*299+c[1]*587+c[0]*114)/1000);
		for(y=0;y<sHeight;y++){
			dp=db+dStride*(y+ys)+xs;
			sp=sb+sStride*y;
			for(x=0;x<sWidth;x++){
				if(*sp>128){
					v=(int)(fr * *dp);
					v+=cv;
					if(v>0xff)v=0xff;
					*dp=(unsigned char)v;
				}
				dp++;
				sp++;
			}
		}
	}
	if(dBpp==24 || dBpp==32){
		c[2]=(int)(r*c[2]);
		c[1]=(int)(r*c[1]);
		c[0]=(int)(r*c[0]);
		for(y=0;y<sHeight;y++){
			dp=db+dStride*(y+ys)+xs*dBpp/8;
			sp=sb+sStride*y;
			for(x=0;x<sWidth;x++){
				for(ci=0;ci<cc;ci++){
					if(*sp>128){
						v=(int)(fr * *dp);
						v+=c[ci];
						if(v>0xff)v=0xff;
						*dp=(unsigned char)v;
					}
					dp++;
				}
				sp++;
				if(dBpp==32)
					dp++;
			}
		}
	}
	deleteEssBitmap(&sbmp);
	return 1;
}
