#ifndef DETECTIONSYSTEM_H
#define DETECTIONSYSTEM_H

#include <QObject>
#include <QTimer>
#include <QImage>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <sstream>

class DetectionSystem : public QObject
{
    Q_OBJECT
public:
    explicit DetectionSystem(QObject *parent = nullptr);
    ~DetectionSystem();

   void Start();
   void Stop();


signals:
//GUI signals
void cameraFrame_captured(QImage frame);
void cameraFrame_processed(QImage frame);
void hsvValues_detected(int hMin,int hMax,int sMin,int sMax,int vMin,int vMax);
//PLC signals
void stateToPlc_ready(bool ready);
void stateToPlc_dataSend(bool dataSend);
void stateToPlc_err(bool err);
void dataToPlc_lenght(float lenght);
void dataToPlc_width(float width);

public slots:
//for GUI slots
void SetErosion(int erosion);
void SetDilation(int dilation);
void SetDimFactorXY(double factor);
void SetDimFactorZ(double factor);
void SetItemHeight(double height);
void SetHMaxVal(int hMax);
void SetHMinVal(int hMin);
void SetSMaxVal(int sMax);
void SetSMinVal(int sMin);
void SetVMaxVal(int vMax);
void SetVMinVal(int vMin);
void DetectItemColor();
//for PLC slots
void SetDataFromPlc(bool req,float itemH);

private:
const int FRAME_WIDTH=640;
const int FRAME_HEIGHT=480;
const int captureInterval=100;
const int processFrameInterval=1000;
const int getItemColorTime=1000;

QTimer *captureTimer;
QTimer *processFrameTimer;
QTimer *getItemColorTimer;

int areaDetectMinX=FRAME_WIDTH/2-20;
int areaDetectMaxX=FRAME_WIDTH/2+20;
int areaDetectMinY=FRAME_HEIGHT/2-20;
int areaDetectMaxY=FRAME_HEIGHT/2+20;

int erosion=0;
int dilation=0;
double dimFactorXY=0.0;
double dimFactorZ=0.0;
double itemHeight=0.0;
int colorHMin=0;
int colorSMin=0;
int colorVMin=0;
int colorHMax=0;
int colorSMax=0;
int colorVMax=0;

//data to PLC
bool toPlcVisionSysReady=false;
bool toPlcNewDimSend=false;
bool toPlcVisionSysErr=false;
float toPlcItemWidth=0.00;
float toPlcItemLenght=0.00;
//data from PLC
bool fromPlcNewParamReq=false;
float fromPlcItemHeight=0.01;

//signals help flags
//-------------------------
bool fromPlcNewParamReqHelpFlag=false;
float fromPlcItemHeightHelpMem=0.01;

cv::VideoCapture cap;

cv::Mat cvframeOrginal;
cv::Mat cvframeOrgSeparated;
cv::Mat cvframeOrgDrawed;
cv::Mat cvframeProcess;
cv::Mat erosionMat;
cv::Mat dilationMat;

cv::Size2f itemSizePix=cv::Size2f(0.0,0.0);
cv::Size2f itemSizeReal=cv::Size2f(0.0,0.0);
cv::Point2f itemCoord[4];

bool detItemColor=false;
bool blockPlcReq=true;

void CapFrameCamera();
void ProcessFrameCamera();
bool FindItemCoordAndSize(cv::Mat &img,cv::Point2f (&points)[4],cv::Size2f &size);
void DrawingOnImage();
void DrawItemColorDetectArea();
void DrawItemSize(cv::Mat &img, float wPix,float wMm, float hPix, float hMm);
void DrawItemHeight(cv::Mat &img, float height);
void DrawItemBound(cv::Mat &img,cv::Point2f coord[4]);

void PlcCommHandling();
void PlcItemSizeReqHandling();

cv::Size2f CalcRealSize(float wPix,float hPix,float factor);
void ResetItemCoord(cv::Point2f (&points)[4]);
QImage ConvMatToQImage(cv::Mat img,QImage::Format format);
bool DetectPositiveEdge(bool state, bool &helpFlag);
bool DetectChangeValue(float value, float &valMem);

};

#endif // DETECTIONSYSTEM_H
