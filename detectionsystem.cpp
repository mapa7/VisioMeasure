#include "detectionsystem.h"
#include <QDebug>

using namespace std;
using namespace cv;

DetectionSystem::DetectionSystem(QObject *parent) : QObject(parent)
{
captureTimer=new QTimer(this);
captureTimer->setInterval(captureInterval);
connect(captureTimer,&QTimer::timeout,this,&DetectionSystem::CapFrameCamera);

processFrameTimer=new QTimer(this);
processFrameTimer->setInterval(processFrameInterval);
connect(processFrameTimer,&QTimer::timeout,this,&DetectionSystem::ProcessFrameCamera);

}

DetectionSystem::~DetectionSystem()
{

}

void DetectionSystem::Start()
{
cap.open(0);
cap.set(CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
cap.set(CAP_PROP_FRAME_WIDTH,FRAME_HEIGHT);
if(!cap.isOpened())
{
qDebug()<<"Problem z kamerą";
emit stateToPlc_ready(false);
}
else
{
captureTimer->start();
processFrameTimer->start();
}
}

void DetectionSystem::Stop()
{
captureTimer->stop();
processFrameTimer->stop();
cap.release();
}


void DetectionSystem::CapFrameCamera()
{
cap.read(cvframeOrginal);
//cvframeOrginal=imread("./test2.jpg");

 if(cvframeOrginal.empty())
 {
     emit stateToPlc_ready(false);
     return;
 }

emit stateToPlc_ready(true);
rotate(cvframeOrginal,cvframeOrginal,ROTATE_180);
cvtColor(cvframeOrginal, cvframeOrginal, COLOR_BGR2RGB);
cvframeOrgDrawed=cvframeOrginal.clone();
DrawingOnImage();
emit cameraFrame_captured(ConvMatToQImage(cvframeOrgDrawed,QImage::Format_RGB888));
PlcCommHandling();
}

void DetectionSystem::ProcessFrameCamera()
{
  if(cvframeOrginal.empty())
      return;

 cvframeOrgSeparated=cvframeOrginal.clone();
 cvtColor(cvframeOrgSeparated,cvframeProcess,COLOR_BGR2HSV);
 //imshow("hsv",cvframeProcess);

 //Color threshold
 //----------------------------
 inRange(cvframeProcess,Scalar(colorHMin,colorSMin,colorVMin),Scalar(colorHMax,colorSMax,colorVMax),cvframeProcess);
 imshow("range",cvframeProcess);

 //Gausian blur
 //----------------------------
 GaussianBlur(cvframeProcess, cvframeProcess, Size(9,9), 1.5);
 imshow("gausian",cvframeProcess);

 //Morfological operations
 //----------------------------
 if(erosion>0)
 {
 erosionMat = getStructuringElement(MORPH_RECT, Size(erosion, erosion));
 erode(cvframeProcess,cvframeProcess,erosionMat);
 }
imshow("erode",cvframeProcess);
 if(dilation>0)
 {
 dilationMat = getStructuringElement(MORPH_RECT, Size(dilation, dilation));
 dilate(cvframeProcess,cvframeProcess,dilationMat);
 }
 imshow("dilate",cvframeProcess);

 //Canny edge detector
 //----------------------------
 Canny(cvframeProcess,cvframeProcess,100,200,3);
imshow("canny",cvframeProcess);

 emit cameraFrame_processed(ConvMatToQImage(cvframeProcess,QImage::Format_Indexed8));


 if(FindItemCoordAndSize(cvframeProcess,itemCoord,itemSizePix))
 {
 float factor=dimFactorXY*(dimFactorZ*itemHeight*0.01);
 itemSizeReal=CalcRealSize(itemSizePix.width,itemSizePix.height,factor);
 blockPlcReq=false;
 }
 else
 {
     ResetItemCoord(itemCoord);
     itemSizePix=Point2f(0.0,0.0);
     itemSizeReal=Size2f(0.0,0.0);
 }

}

void DetectionSystem::ResetItemCoord(cv::Point2f (&points)[4])
{
    for(int i=0;i<4;i++)
    {
        points[i]=Point2f(0.0,0.0);
    }
}

bool DetectionSystem::FindItemCoordAndSize(cv::Mat &img,cv::Point2f (&points)[4],cv::Size2f &size)
{
    if(img.empty())
        return false;

    vector<vector<Point>> contours;
    RotatedRect boundRect;

    //find contour on frame camera
    findContours(img,contours,RETR_LIST,CHAIN_APPROX_SIMPLE);

    if(contours.size()<1)
        return false;

    double minContArea=contourArea(contours[0]);
    int contInd=0;

    //find contour number with max area
    for(size_t i=0;i<contours.size();i++)
    {

       double area=contourArea(contours[i]);
       if(minContArea>area)
       {
            minContArea=area;
            contInd=i;
        }
    }

    boundRect=minAreaRect(contours[contInd]);

    boundRect.points(points);
    size=boundRect.size;

    return true;
}

Size2f DetectionSystem::CalcRealSize(float wPix,float hPix,float factor)
{
  float f=0.01;

  if(f<factor)
      f=factor;

  float realW=wPix*f;
  float realH=hPix*f;

 return Size2f(realW,realH);
}

void DetectionSystem::DrawItemSize(cv::Mat &img, float wPix,float wMm, float hPix, float hMm)
{

    if(img.empty())
        return;

    if(wPix<10.0 || hPix<10.0 || wMm<0.0 || hMm<0.0)
        return;

    QString hP=QString::number(hPix,'f',2);
    QString hM=QString::number(hMm,'f',2);

    QString wP=QString::number(wPix,'f',2);
    QString wM=QString::number(wMm,'f',2);

    String hString="Dl: "+hP.toStdString()+" px = "+hM.toStdString()+" mm";
    String wString="Sz: "+wP.toStdString()+" px = "+wM.toStdString()+" mm";

    Point textHPoint=Point(0,444);
    Point textWPoint=Point(0,470);

    putText(img,hString,textHPoint,FONT_HERSHEY_COMPLEX,0.7,Scalar(0,255,0),2);
    putText(img,wString,textWPoint,FONT_HERSHEY_COMPLEX,0.7,Scalar(0,255,0),2);

}

void DetectionSystem::DrawItemHeight(cv::Mat &img, float height)
{

    if(img.empty())
        return;

    if(height<0.001)
        return;

    QString h=QString::number(height,'f',1);

    String string="Gr: "+h.toStdString()+" mm";

    Point textPoint=Point(0,20);
    putText(img,string,textPoint,FONT_HERSHEY_COMPLEX,0.7,Scalar(0,0,255),2);
}

QImage DetectionSystem::ConvMatToQImage(cv::Mat img, QImage::Format format)
{
    QImage qimg((uchar*)img.data, img.cols, img.rows, img.step, format);
    return qimg;
}

void DetectionSystem::DrawingOnImage()
{
    if(detItemColor)
    DrawItemColorDetectArea();

    DrawItemHeight(cvframeOrgDrawed,itemHeight);

    //qDebug()<itemPlcItemHeight;

    DrawItemSize(cvframeOrgDrawed,itemSizePix.width,itemSizeReal.width,itemSizePix.height,itemSizeReal.height);
    DrawItemBound(cvframeOrgDrawed,itemCoord);

}

void DetectionSystem::DetectItemColor()
{
  if(cvframeOrginal.empty())
        return;

  detItemColor=true;

  QTimer::singleShot(1000,this,[=]()
  {
      detItemColor=false;
  });

  int hMin = 179,hMax = 0,sMin = 255,sMax = 0,vMin = 255,vMax=0;

  Mat matRoiOrg=cvframeOrginal(Range(areaDetectMinY,areaDetectMaxY),Range(areaDetectMinX,areaDetectMaxX));
  Mat matRoiHsv;
  cvtColor(matRoiOrg,matRoiHsv,COLOR_BGR2HSV);

  for(int i=0;i<matRoiHsv.rows;i++)
  {
      for(int j=0;j<matRoiHsv.cols;j++)
      {
          Vec3b hsv=matRoiHsv.at<Vec3b>(i,j);
          hMin=qMin(hMin,(int)hsv[0]);
          hMax=qMax(hMax,(int)hsv[0]);
          sMin=qMin(sMin,(int)hsv[1]);
          sMax=qMax(sMax,(int)hsv[1]);
          vMin=qMin(vMin,(int)hsv[2]);
          vMax=qMax(vMax,(int)hsv[2]);
      }
  }


  emit hsvValues_detected(hMin,hMax,sMin,sMax,vMin,vMax);
}

void DetectionSystem::DrawItemColorDetectArea()
{
    Scalar areaColor=Scalar(0,255,0);
    line(cvframeOrgDrawed,Point(areaDetectMinX,areaDetectMinY),Point(areaDetectMaxX,areaDetectMinY),areaColor,2);
    line(cvframeOrgDrawed,Point(areaDetectMaxX,areaDetectMinY),Point(areaDetectMaxX,areaDetectMaxY),areaColor,2);
    line(cvframeOrgDrawed,Point(areaDetectMinX,areaDetectMaxY),Point(areaDetectMaxX,areaDetectMaxY),areaColor,2);
    line(cvframeOrgDrawed,Point(areaDetectMinX,areaDetectMinY),Point(areaDetectMinX,areaDetectMaxY),areaColor,2);
}

void DetectionSystem::DrawItemBound(cv::Mat &img, cv::Point2f *coord)
{
    if(img.empty())
          return;

    if(coord[0].x<0.1 && coord[0].y<0.1 &&
       coord[1].x<0.1 && coord[1].y<0.1 &&
       coord[2].x<0.1 && coord[2].y<0.1 &&
       coord[3].x<0.1 && coord[3].y<0.1)
        return;

    line(img,coord[0],coord[1],Scalar(0,255,0),3);
    line(img,coord[0],coord[3],Scalar(0,255,0),3);

}

void DetectionSystem::SetErosion(int erosion)
{
    this->erosion=erosion;
}

void DetectionSystem::SetDilation(int dilation)
{
    this->dilation=dilation;
}

void DetectionSystem::SetDimFactorXY(double factor)
{
    this->dimFactorXY=factor;
}

void DetectionSystem::SetDimFactorZ(double factor)
{
    this->dimFactorZ=factor;
}

void DetectionSystem::SetItemHeight(double height)
{
    this->itemHeight=height;
}

void DetectionSystem::SetHMaxVal(int hMax)
{
    this->colorHMax=hMax;
}

void DetectionSystem::SetHMinVal(int hMin)
{
    this->colorHMin=hMin;
}

void DetectionSystem::SetSMaxVal(int sMax)
{
    this->colorSMax=sMax;
}

void DetectionSystem::SetSMinVal(int sMin)
{
    this->colorSMin=sMin;
}

void DetectionSystem::SetVMaxVal(int vMax)
{
    this->colorVMax=vMax;
}

void DetectionSystem::SetVMinVal(int vMin)
{
    this->colorVMin=vMin;
}

void DetectionSystem::SetDataFromPlc(bool req,float itemH)
{
    this->fromPlcNewParamReq=req;
    this->fromPlcItemHeight=itemH;

    SetItemHeight(this->fromPlcItemHeight);
}


void DetectionSystem::PlcCommHandling()
{   
    if(DetectChangeValue(fromPlcItemHeight,fromPlcItemHeightHelpMem))
        blockPlcReq=true;

    if(!blockPlcReq)
        PlcItemSizeReqHandling();

}

void DetectionSystem::PlcItemSizeReqHandling()
{
    if(DetectPositiveEdge(fromPlcNewParamReq,fromPlcNewParamReqHelpFlag))
    {
        if(itemSizeReal.width>1.0 && itemSizeReal.height>1.0)
        {
            emit dataToPlc_lenght(itemSizeReal.width);
            emit dataToPlc_width(itemSizeReal.height);
            emit stateToPlc_dataSend(true);
        }
        else
            emit stateToPlc_err(true);
    }

    if(fromPlcNewParamReq==false)
    {
         emit stateToPlc_err(false);
         emit dataToPlc_lenght(0.0);
         emit dataToPlc_width(0.0);
         emit stateToPlc_dataSend(false);
    }
}

bool DetectionSystem::DetectPositiveEdge(bool state, bool &helpFlag)
{
    bool retVal=false;

    if(state && !helpFlag)
    {
        helpFlag=true;

        retVal=true;
    }
    else if (!state)
    {
        helpFlag=false;
    }

    return retVal;
}

bool DetectionSystem::DetectChangeValue(float value, float &valMem)
{
    bool retVal=false;

    if(value!=valMem)
    {
        retVal=true;
    }

    valMem=value;
    return retVal;
}
