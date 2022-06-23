#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QDebug>
#include <QRegExpValidator>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //settings file
    qSettingsFileName=QApplication::applicationDirPath()+"/settings.ini";//app settings file path
    //plc and ds instances
    s71200=new PlcSiemens(this);
    detectionSystem=new DetectionSystem(this);

    //extra windows
    dialogHelpDBWrite=new DialogHelpDBWrite();
    dialogHelpDBRead=new DialogHelpDBRead();

    SetIpAdressValidator();
    ReadSettings();  
    InternalSignalConn();
    ExternalSignalConn();
    LoadParamToUIValues();
    detectionSystem->Start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent)
{
   if(keyEvent->key()==Qt::Key_Escape)
   {
    MainWindow::showNormal();
   }
}



void MainWindow::InternalSignalConn()
{
    connect(ui->pushButtonSaveSettings,&QPushButton::clicked,this,&MainWindow::SaveSettings);
}

void MainWindow::ExternalSignalConn()
{
    //Vision System => PLC
    connect(detectionSystem,&DetectionSystem::stateToPlc_ready,s71200,&PlcSiemens::SetBool1ToPLC);
    connect(detectionSystem,&DetectionSystem::stateToPlc_dataSend,s71200,&PlcSiemens::SetBool2ToPLC);
    connect(detectionSystem,&DetectionSystem::stateToPlc_err,s71200,&PlcSiemens::SetBool3ToPLC);
    connect(detectionSystem,&DetectionSystem::dataToPlc_width,s71200,&PlcSiemens::SetFloat0ToPLC);
    connect(detectionSystem,&DetectionSystem::dataToPlc_lenght,s71200,&PlcSiemens::SetFloat1ToPLC);

    //PLC => Vision System
    connect(s71200,&PlcSiemens::dataToVS_transfer,detectionSystem,&DetectionSystem::SetDataFromPlc);

    //PLC => GUI
    connect(s71200,&PlcSiemens::readDataWindowUpdate,dialogHelpDBRead,&DialogHelpDBRead::UpdateDispValues);
    connect(s71200,&PlcSiemens::writeDataWindowUpdate,dialogHelpDBWrite,&DialogHelpDBWrite::UpdateDispValues);

    //Vision System => GUI
    connect(detectionSystem,&DetectionSystem::cameraFrame_captured,this,&MainWindow::UpdateCameraFrame);
    connect(detectionSystem,&DetectionSystem::cameraFrame_processed,this,&MainWindow::UpdateProcessFrame);
    connect(detectionSystem,&DetectionSystem::hsvValues_detected,this,&MainWindow::SetDetectHsvValues);

    //GUI => Vision System
    connect(ui->spinBoxErosion,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetErosion);
    connect(ui->spinBoxDilation,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetDilation);
    connect(ui->doubleSpinBoxFactorXY,QOverload<double>::of(&QDoubleSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetDimFactorXY);
    connect(ui->doubleSpinBoxFactorZ,QOverload<double>::of(&QDoubleSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetDimFactorZ);
    connect(ui->doubleSpinBoxCamDist,QOverload<double>::of(&QDoubleSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetItemHeight);

    connect(ui->spinBoxHMax,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetHMaxVal);
    connect(ui->spinBoxHMin,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetHMinVal);
    connect(ui->spinBoxSMax,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetSMaxVal);
    connect(ui->spinBoxSMin,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetSMinVal);
    connect(ui->spinBoxVMax,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetVMaxVal);
    connect(ui->spinBoxVMin,QOverload<int>::of(&QSpinBox::valueChanged),detectionSystem,&DetectionSystem::SetVMinVal);
    connect(ui->pushButtonGetColorFromCam,&QPushButton::clicked,detectionSystem,&DetectionSystem::DetectItemColor);

    //GUI => PLC
    connect(this,&MainWindow::plcDataExchange_start,s71200,&PlcSiemens::Start);
    connect(this,&MainWindow::plcDataExchange_stop,s71200,&PlcSiemens::Stop);
}


void MainWindow::AppExit()
{
exit(0);
}


QString MainWindow::GetIpAdress()
{
   return ui->lineEditIpAdress3oct->text()+"."+ui->lineEditIpAdress2oct->text()+
      "."+ui->lineEditIpAdress1oct->text()+"."+ui->lineEditIpAdress0oct->text();
}



void MainWindow::DisableWidgets()
{
    //PLC
    ui->lineEditIpAdress0oct->setEnabled(false);
    ui->lineEditIpAdress1oct->setEnabled(false);
    ui->lineEditIpAdress2oct->setEnabled(false);
    ui->lineEditIpAdress3oct->setEnabled(false);
    ui->spinBoxRack->setEnabled(false);
    ui->spinBoxSlot->setEnabled(false);
    ui->spinBoxDBRead->setEnabled(false);
    ui->spinBoxDBWrite->setEnabled(false);
    //Camera settings
    ui->spinBoxErosion->setEnabled(false);
    ui->spinBoxDilation->setEnabled(false);
    ui->doubleSpinBoxFactorXY->setEnabled(false);
    ui->doubleSpinBoxFactorZ->setEnabled(false);
    ui->doubleSpinBoxCamDist->setEnabled(false);
    //Main
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonSaveSettings->setEnabled(false);
    ui->pushButtonExit->setEnabled(false);
    //Color calibration
    ui->horizontalSliderHMax->setEnabled(false);
    ui->horizontalSliderSMax->setEnabled(false);
    ui->horizontalSliderVMax->setEnabled(false);
    ui->horizontalSliderHMin->setEnabled(false);
    ui->horizontalSliderSMin->setEnabled(false);
    ui->horizontalSliderVMin->setEnabled(false);
    ui->spinBoxHMin->setEnabled(false);
    ui->spinBoxSMin->setEnabled(false);
    ui->spinBoxVMin->setEnabled(false);
    ui->spinBoxHMax->setEnabled(false);
    ui->spinBoxSMax->setEnabled(false);
    ui->spinBoxVMax->setEnabled(false);
    ui->pushButtonGetColorFromCam->setEnabled(false);
}

void MainWindow::EnableWidgets()
{
//PLC
ui->lineEditIpAdress0oct->setEnabled(true);
ui->lineEditIpAdress1oct->setEnabled(true);
ui->lineEditIpAdress2oct->setEnabled(true);
ui->lineEditIpAdress3oct->setEnabled(true);
ui->spinBoxRack->setEnabled(true);
ui->spinBoxSlot->setEnabled(true);
ui->spinBoxDBRead->setEnabled(true);
ui->spinBoxDBWrite->setEnabled(true);
//Camera settings
ui->spinBoxErosion->setEnabled(true);
ui->spinBoxDilation->setEnabled(true);
ui->doubleSpinBoxFactorXY->setEnabled(true);
ui->doubleSpinBoxFactorZ->setEnabled(true);
ui->doubleSpinBoxCamDist->setEnabled(true);
//Main
ui->pushButtonStart->setEnabled(true);
ui->pushButtonStop->setEnabled(false);
ui->pushButtonSaveSettings->setEnabled(true);
ui->pushButtonExit->setEnabled(true);
//Color calibration
ui->horizontalSliderHMax->setEnabled(true);
ui->horizontalSliderSMax->setEnabled(true);
ui->horizontalSliderVMax->setEnabled(true);
ui->horizontalSliderHMin->setEnabled(true);
ui->horizontalSliderSMin->setEnabled(true);
ui->horizontalSliderVMin->setEnabled(true);
ui->spinBoxHMin->setEnabled(true);
ui->spinBoxSMin->setEnabled(true);
ui->spinBoxVMin->setEnabled(true);
ui->spinBoxHMax->setEnabled(true);
ui->spinBoxSMax->setEnabled(true);
ui->spinBoxVMax->setEnabled(true);
ui->pushButtonGetColorFromCam->setEnabled(true);

}

void MainWindow::BlockSignals()
{
ui->doubleSpinBoxCamDist->blockSignals(true);
}

void MainWindow::UnblockSignals()
{
ui->doubleSpinBoxCamDist->blockSignals(false);
emit ui->doubleSpinBoxCamDist->valueChanged(ui->doubleSpinBoxCamDist->value());
}

void MainWindow::SetIpAdressValidator()
{

    QRegularExpression ipOctRegEx("(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
    QValidator *ipOctValidator=new QRegularExpressionValidator(ipOctRegEx,this);
    ui->lineEditIpAdress0oct->setValidator(ipOctValidator);
    ui->lineEditIpAdress1oct->setValidator(ipOctValidator);
    ui->lineEditIpAdress2oct->setValidator(ipOctValidator);
    ui->lineEditIpAdress3oct->setValidator(ipOctValidator);
}

void MainWindow::SaveSettings()
{
ui->pushButtonSaveSettings->setStyleSheet(buttonEnabledStyleSheet);

QTimer::singleShot(timeBacklight,this,[=]()
{
    ui->pushButtonSaveSettings->setStyleSheet(buttonDisabledStyleSheet);
});

QSettings settings(qSettingsFileName,QSettings::NativeFormat);
ipAdress=GetIpAdress();
rack=ui->spinBoxRack->value();
slot=ui->spinBoxSlot->value();
dbReadNb=ui->spinBoxDBRead->value();
dbWriteNb=ui->spinBoxDBWrite->value();

erosion=ui->spinBoxErosion->value();
dilation=ui->spinBoxDilation->value();
dimFactorXY=ui->doubleSpinBoxFactorXY->value();
dimDistZ=ui->doubleSpinBoxCamDist->value();
dimFactorZ=ui->doubleSpinBoxFactorXY->value();

colorHMax=ui->spinBoxHMax->value();
colorHMin=ui->spinBoxHMin->value();
colorSMax=ui->spinBoxSMax->value();
colorSMin=ui->spinBoxSMin->value();
colorVMax=ui->spinBoxVMax->value();
colorVMin=ui->spinBoxVMin->value();

//PLC connection settings
settings.beginGroup("PLC_settings");
settings.setValue("ipAdress",ipAdress);
settings.setValue("rack",rack);
settings.setValue("slot",slot);
settings.setValue("dbReadNb",dbReadNb);
settings.setValue("dbWriteNb",dbWriteNb);
settings.endGroup();
//Camera settings
settings.beginGroup("Camera_settings");
settings.setValue("erosion",erosion);
settings.setValue("dilation",dilation);
settings.setValue("dimFactorXY",dimFactorXY);
settings.setValue("dimDistZ",dimDistZ);
settings.setValue("dimFactorZ",dimFactorZ);
settings.endGroup();
//Color calibration settings
settings.beginGroup("Color_calib");
settings.setValue("colorHMin",colorHMin);
settings.setValue("colorHMax",colorHMax);
settings.setValue("colorSMin",colorSMin);
settings.setValue("colorSMax",colorSMax);
settings.setValue("colorVMin",colorVMin);
settings.setValue("colorVMax",colorVMax);
}

void MainWindow::ReadSettings()
{
QSettings settings(qSettingsFileName,QSettings::NativeFormat);
//PLC connection settings
ipAdress=settings.value("PLC_settings/ipAdress",ipAdress).toString();
rack=settings.value("PLC_settings/rack",rack).toInt();
slot=settings.value("PLC_settings/slot",slot).toInt();
dbReadNb=settings.value("PLC_settings/dbReadNb",dbReadNb).toInt();
dbWriteNb=settings.value("PLC_settings/dbWriteNb",dbWriteNb).toInt();
//Camera settings
erosion=settings.value("Camera_settings/erosion",erosion).toInt();
dilation=settings.value("Camera_settings/dilation",dilation).toInt();
dimFactorXY=settings.value("Camera_settings/dimFactorXY",dimFactorXY).toDouble();
dimDistZ=settings.value("Camera_settings/dimDistZ",dimDistZ).toDouble();
dimFactorZ=settings.value("Camera_settings/dimFactorZ",dimFactorZ).toDouble();
//Color calibration settings
colorHMin=settings.value("Color_calib/colorHMin",colorHMin).toInt();
colorHMax=settings.value("Color_calib/colorHMax",colorHMax).toInt();
colorSMin=settings.value("Color_calib/colorSMin",colorSMin).toInt();
colorSMax=settings.value("Color_calib/colorSMax",colorSMax).toInt();
colorVMin=settings.value("Color_calib/colorVMin",colorVMin).toInt();
colorVMax=settings.value("Color_calib/colorVMax",colorVMax).toInt();

}

void MainWindow::LoadParamToUIValues()
{
QStringList ipAdressOctets=ipAdress.split(".");
ui->lineEditIpAdress3oct->setText(ipAdressOctets.at(0));
ui->lineEditIpAdress2oct->setText(ipAdressOctets.at(1));
ui->lineEditIpAdress1oct->setText(ipAdressOctets.at(2));
ui->lineEditIpAdress0oct->setText(ipAdressOctets.at(3));

ui->spinBoxRack->setValue(rack);
ui->spinBoxSlot->setValue(slot);
ui->spinBoxDBRead->setValue(dbReadNb);
ui->spinBoxDBWrite->setValue(dbWriteNb);

ui->spinBoxErosion->setValue(erosion);
ui->spinBoxDilation->setValue(dilation);
ui->doubleSpinBoxFactorXY->setValue(dimFactorXY);
ui->doubleSpinBoxCamDist->setValue(dimDistZ);
ui->doubleSpinBoxFactorZ->setValue(dimFactorZ);

ui->spinBoxHMin->setValue(colorHMin);
ui->spinBoxSMin->setValue(colorSMin);
ui->spinBoxVMin->setValue(colorVMin);
ui->spinBoxHMax->setValue(colorHMax);
ui->spinBoxSMax->setValue(colorSMax);
ui->spinBoxVMax->setValue(colorVMax);

}


void MainWindow::SetDetectHsvValues(int hMin, int hMax, int sMin, int sMax, int vMin, int vMax)
{
ui->spinBoxHMin->setValue(hMin);
ui->spinBoxHMax->setValue(hMax);
ui->spinBoxSMin->setValue(sMin);
ui->spinBoxSMax->setValue(sMax);
ui->spinBoxVMin->setValue(vMin);
ui->spinBoxVMax->setValue(vMax);
}


void MainWindow::UpdateCameraFrame(QImage img)
{
ui->orgImage->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::UpdateProcessFrame(QImage img)
{
ui->procImage->setPixmap(QPixmap::fromImage(img));
}


void MainWindow::on_pushButtonStart_clicked()
{
    ui->pushButtonStart->setStyleSheet(buttonEnabledStyleSheet);
    DisableWidgets();
    BlockSignals();

    //start communication with plc
    QString ip=GetIpAdress();
    int r=ui->spinBoxRack->value();
    int s=ui->spinBoxSlot->value();
    int dbR=ui->spinBoxDBRead->value();
    int dbW=ui->spinBoxDBWrite->value();
    emit plcDataExchange_start(ip,r,s,dbR,dbW);

}

void MainWindow::on_pushButtonStop_clicked()
{
    ui->pushButtonStop->setStyleSheet(buttonEnabledStyleSheet);
    QTimer::singleShot(timeBacklight,this,[=]()
    {
        ui->pushButtonStop->setStyleSheet(buttonDisabledStyleSheet);
    });
    ui->pushButtonStart->setStyleSheet(buttonDisabledStyleSheet);

    emit plcDataExchange_stop();
    EnableWidgets();
    UnblockSignals();
}


void MainWindow::on_pushButtonExit_clicked()
{
   ui->pushButtonExit->setStyleSheet(buttonEnabledStyleSheet);

   QTimer::singleShot(timeBacklight,this,[=]()
   {
       ui->pushButtonExit->setStyleSheet(buttonDisabledStyleSheet);
   });

   QTimer::singleShot(1000,this,&MainWindow::AppExit);
}

void MainWindow::on_horizontalSliderHMin_valueChanged(int value)
{
    ui->spinBoxHMin->setValue(value);
}

void MainWindow::on_horizontalSliderSMin_valueChanged(int value)
{
    ui->spinBoxSMin->setValue(value);
}

void MainWindow::on_horizontalSliderVMin_valueChanged(int value)
{
    ui->spinBoxVMin->setValue(value);
}

void MainWindow::on_horizontalSliderHMax_valueChanged(int value)
{
   ui->spinBoxHMax->setValue(value);
}

void MainWindow::on_horizontalSliderSMax_valueChanged(int value)
{
    ui->spinBoxSMax->setValue(value);
}

void MainWindow::on_horizontalSliderVMax_valueChanged(int value)
{
    ui->spinBoxVMax->setValue(value);
}

void MainWindow::on_pushButtonHelpDBRead_clicked()
{
    ui->pushButtonHelpDBRead->setStyleSheet(buttonEnabledStyleSheet);
    QTimer::singleShot(timeBacklight,this,[=]()
    {
        ui->pushButtonHelpDBRead->setStyleSheet(buttonDisabledStyleSheet);
    });
    dialogHelpDBRead->setGeometry(dialogHelpPosX,dialogHelpPosY,dialogHelpW,dialogHelpH);
    dialogHelpDBRead->show();
}

void MainWindow::on_pushButtonHelpDBWrite_clicked()
{
    ui->pushButtonHelpDBWrite->setStyleSheet(buttonEnabledStyleSheet);
    QTimer::singleShot(timeBacklight,this,[=]()
    {
        ui->pushButtonHelpDBWrite->setStyleSheet(buttonDisabledStyleSheet);
    });
    dialogHelpDBWrite->setGeometry(dialogHelpPosX,dialogHelpPosY,dialogHelpW,dialogHelpH);
    dialogHelpDBWrite->show();
}


void MainWindow::on_spinBoxHMin_valueChanged(int arg1)
{
    ui->horizontalSliderHMin->setValue(arg1);
}

void MainWindow::on_spinBoxSMin_valueChanged(int arg1)
{
    ui->horizontalSliderSMin->setValue(arg1);
}

void MainWindow::on_spinBoxVMin_valueChanged(int arg1)
{
    ui->horizontalSliderVMin->setValue(arg1);
}

void MainWindow::on_spinBoxHMax_valueChanged(int arg1)
{
    ui->horizontalSliderHMax->setValue(arg1);
}

void MainWindow::on_spinBoxSMax_valueChanged(int arg1)
{
    ui->horizontalSliderSMax->setValue(arg1);
}

void MainWindow::on_spinBoxVMax_valueChanged(int arg1)
{
    ui->horizontalSliderVMax->setValue(arg1);
}

void MainWindow::on_pushButtonGetColorFromCam_clicked()
{
    ui->pushButtonGetColorFromCam->setStyleSheet(buttonEnabledStyleSheet);

    QTimer::singleShot(timeBacklight,this,[=]()
    {
        ui->pushButtonGetColorFromCam->setStyleSheet(buttonDisabledStyleSheet);
    });
}




