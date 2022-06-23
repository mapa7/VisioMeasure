#ifndef PLCSIEMENS_H
#define PLCSIEMENS_H

#include <QObject>
#include <QString>
#include "snap7.h"
#include <QTimer>


class PlcSiemens:public QObject
{
    Q_OBJECT
public:
    explicit PlcSiemens(QObject *parent = nullptr);
    ~PlcSiemens();

    bool boolToPlc[8]={false};
    float floatToPlc[10]={0.0};

    bool boolFromPlc[8]={false};
    float floatFromPlc[10]={0.0};

public slots:
    void Start(QString ipAdress,int rack,int slot,int dbReadNb,int dbWriteNb);
    void Stop();
    void SetBool1ToPLC(bool state);
    void SetBool2ToPLC(bool state);
    void SetBool3ToPLC(bool state);
    void SetFloat0ToPLC(float value);
    void SetFloat1ToPLC(float value);

signals:
void readDataWindowUpdate(bool boolFromPlc[],float floatFromPlc[]);
void writeDataWindowUpdate(bool boolToPlc[],float floatToPlc[]);
//void readFromPlcDone();
void dataToVS_transfer(bool bool0, float float0);


private:
    TS7Client *s7Client;
    QTimer *cycleTimer;
    QTimer *liveBitTimer;

    int errCode=0;
    int dbWriteErrCode=0;
    int dbReadErrCode=0;
    int connErrCode=0;

    int rack=0;
    int slot=0;
    QString ipAdress="0.0.0.0";
    int dbReadNb=0;
    int dbWriteNb=0;

    QVector <byte> *readData;
    const int sizeReadData=42;

    QVector <byte> *writeData;
    const int sizeWriteData=42;

   const int cycleTime=400;
   const int liveBitChangeTime=1000;

   void ConnectToPlc();
   void DisconnectPlc();

    //live bit
    bool liveBit=false;

    //buffer exchange
    void SendDataToPlc();
    void GetDataFromPlc();

    //conversion bit array <=> byte
    void SetBitArrayInByte(QVector <byte> &buffer,int bytePos,bool bitArray[]);
    void GetBitArrayFromByte(QVector <byte> &buffer,int bytePos,bool (&bitArray)[8]);
    //conversion methods from S7 data types
    int GetIntFromDInt(QVector <byte> buffer,int pos);
    float GetFloatFromReal(QVector <byte> buffer,int pos);
    bool GetBitStateFromByte(QVector <byte> buffer,int byteNb, int bitNb);
    //conversion methods to S7 data types
    void SetDIntFromInt(QVector <byte> &buffer,int pos,int value);
    void SetRealFromFloat(QVector <byte> &buffer,int pos,float value);
    void SetBitInByte(QVector <byte> &buffer,int bytePos,int bitNb, bool bitState);
    void ResetBitInByte(QVector <byte> &buffer,int bytePos,int bitNb);
    // get and set data in exchange buffer
    void SetDataInWriteBuffer();
    void GetDataFromReadBuffer();

private slots:
    void CyclicalDataExchange();
    void ChangeStateLiveBit();
};

#endif // PLCSIEMENS_H
