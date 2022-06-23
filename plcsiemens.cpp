#include "plcsiemens.h"
#include <QDebug>

PlcSiemens::PlcSiemens(QObject *parent) : QObject(parent)
{
    s7Client=new TS7Client();

    readData=new QVector <byte>(sizeReadData);
    writeData=new QVector <byte>(sizeWriteData);

    cycleTimer=new QTimer(this);
    liveBitTimer=new QTimer(this);
    cycleTimer->setInterval(cycleTime);
    liveBitTimer->setInterval(liveBitChangeTime);
    connect(liveBitTimer,&QTimer::timeout,this,&PlcSiemens::ChangeStateLiveBit);
    connect(cycleTimer,&QTimer::timeout,this,&PlcSiemens::CyclicalDataExchange);

}

PlcSiemens::~PlcSiemens()
{
    delete s7Client;
}


void PlcSiemens::Start(QString ipAdress,int rack,int slot,int dbReadNb,int dbWriteNb)
{  
    this->ipAdress=ipAdress;
    this->rack=rack;
    this->slot=slot;
    this->dbReadNb=dbReadNb;
    this->dbWriteNb=dbWriteNb;

    //Live bit QTimer
    liveBitTimer->start();
    cycleTimer->start();



}

void PlcSiemens::Stop()
{
liveBitTimer->stop();
cycleTimer->stop();
DisconnectPlc();
}

void PlcSiemens::ConnectToPlc()
{
    if(s7Client->Connected())
        return;

    try
    {
        connErrCode=s7Client->ConnectTo(ipAdress.toUtf8(),rack,slot);
        if(connErrCode==0)
        {
            qDebug()<<"connected with PLC with adress: "+ipAdress;
        }

    }
    catch (std::string exception)
    {

        qDebug()<<"error with connection "<<QString::fromStdString(exception);
    }
}

void PlcSiemens::DisconnectPlc()
{
    if(s7Client->Connected())
    s7Client->Disconnect();
}

void PlcSiemens::ChangeStateLiveBit()
{
    liveBit=!liveBit;
    this->boolToPlc[0]=liveBit;
}


void PlcSiemens::SetBool1ToPLC(bool state)
{
    this->boolToPlc[1]=state;
}

void PlcSiemens::SetBool2ToPLC(bool state)
{
    this->boolToPlc[2]=state;
}
void PlcSiemens::SetBool3ToPLC(bool state)
{
    this->boolToPlc[3]=state;
}
void PlcSiemens::SetFloat0ToPLC(float value)
{
    this->floatToPlc[0]=value;
}
void PlcSiemens::SetFloat1ToPLC(float value)
{
    this->floatToPlc[1]=value;
}

void PlcSiemens::SendDataToPlc()
{
    if(s7Client->Connected())
    {
        try
        {
             dbWriteErrCode=s7Client->DBWrite(dbWriteNb,0,sizeWriteData,writeData->data());
        }
        catch (std::string exception)
        {
            qDebug()<<"error with send data"+QString::fromStdString(exception);
        }

    }
    else
        ConnectToPlc();
}

void PlcSiemens::GetDataFromPlc()
{
    if(s7Client->Connected())
    {
        try
        {
          dbReadErrCode=s7Client->DBRead(dbReadNb,0,sizeReadData,readData->data());
        }
        catch (std::string exception)
        {
            qDebug()<<"error with read data "+QString::fromStdString(exception);
        }
    }
}

void PlcSiemens::CyclicalDataExchange()
{
    cycleTimer->stop();
    SetDataInWriteBuffer();

    SendDataToPlc();
    GetDataFromPlc();

    if(dbReadErrCode==0)
    {
    GetDataFromReadBuffer();
    }
     cycleTimer->start();
}

int PlcSiemens::GetIntFromDInt(QVector <byte> buffer,int pos)
{
    if(buffer.size()<pos+3)
    return 0;
    else
    return int(buffer[pos]<<24|buffer[pos+1]<<16|buffer[pos+2]<<8|buffer[pos+3]);
}

float PlcSiemens::GetFloatFromReal(QVector <byte> buffer,int pos)
{
    float value=0.0;

    if(buffer.size()>=pos+3)
  {
       byte b[] = {buffer.at(pos+3), buffer.at(pos+2), buffer.at(pos+1), buffer.at(pos)};
       memcpy(&value, &b, sizeof(value));
  }

    return value;
}

bool PlcSiemens::GetBitStateFromByte(QVector <byte> buffer,int bytePos, int bitNb)
{
    if(buffer.size()<bytePos)
    return false;
    else
    return (buffer[bytePos])&(0x01<<bitNb);
}

void PlcSiemens::SetDIntFromInt( QVector<byte> &buffer, int pos, int value)
{
    if(buffer.size()>=pos+4)
    {
        buffer.replace(pos,byte(value>>24));
        buffer.replace(pos+1,byte(value>>16));
        buffer.replace(pos+2,byte(value>>8));
        buffer.replace(pos+3,byte(value));
    }

}

void PlcSiemens::SetRealFromFloat(QVector<byte> &buffer, int pos, float value)
{
    if(buffer.size()>=pos+4)
    {
      byte temp[4];
      memcpy(temp,&value,4);

      buffer.replace(pos,temp[3]);
      buffer.replace(pos+1,temp[2]);
      buffer.replace(pos+2,temp[1]);
      buffer.replace(pos+3,temp[0]);
    }
}

void PlcSiemens::SetBitInByte(QVector<byte> &buffer, int bytePos, int bitNb, bool bitState)
{
    if(buffer.size()>bytePos)
    {
        byte temp=buffer.at(bytePos);

        if(bitState)
        {
            temp=temp|(byte(0x01<<bitNb));
        }
        else
        {
            temp=temp&~(byte(0x01<<bitNb));
        }

        buffer.replace(bytePos,temp);
    }
}

void PlcSiemens::ResetBitInByte(QVector <byte> &buffer,int bytePos,int bitNb)
{
    if(buffer.size()>bytePos)
    {
        byte temp=buffer.at(bytePos)&~(byte(0x01<<bitNb));
        buffer.replace(bytePos,temp);
    }
}

void PlcSiemens::GetBitArrayFromByte(QVector <byte> &buffer,int bytePos,bool (&bitArray)[8])
{
    byte tempByte=buffer.at(bytePos);
    byte mask=0x01;

    for(int i=0;i<8;i++)
    {
        if(tempByte&(mask<<i))
            bitArray[i]=true;
        else
            bitArray[i]=false;
    }
}

void PlcSiemens::SetBitArrayInByte(QVector <byte> &buffer,int bytePos,bool bitArray[])
{
    byte tempByte=0x00;
    for(int i=0;i<8;i++)
    {
        if(bitArray[i])
        {
             tempByte=tempByte|(byte(0x01<<i));
        }
    }
    buffer.replace(bytePos,tempByte);
}

void PlcSiemens::SetDataInWriteBuffer()
{
    SetBitArrayInByte(*writeData,0,boolToPlc);

    for(int i=0;i<10;i++)
    {
        SetRealFromFloat(*writeData,2+(4*i),floatToPlc[i]);
    }
  emit writeDataWindowUpdate(boolToPlc,floatToPlc);
}

void PlcSiemens::GetDataFromReadBuffer()
{
    GetBitArrayFromByte(*readData,0,boolFromPlc);
    for(int i=0;i<10;i++)
    {
    floatFromPlc[i]=GetFloatFromReal(*readData,2+(4*i));
    }
    emit readDataWindowUpdate(boolFromPlc,floatFromPlc);
    //emit dataToVS_transfer(boolFromPlc[0],floatFromPlc[0]);
}

