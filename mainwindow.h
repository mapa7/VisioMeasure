#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include "dialoghelpdbread.h"
#include "dialoghelpdbwrite.h"
#include "plcsiemens.h"
#include "detectionsystem.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTimer *clockTimer;

    PlcSiemens *s71200;
    int plcErrCode=0;

    DetectionSystem *detectionSystem;

    DialogHelpDBRead *dialogHelpDBRead;
    DialogHelpDBWrite *dialogHelpDBWrite;

    const QString buttonEnabledStyleSheet="background-color: #00ff00;";
    const QString buttonDisabledStyleSheet="background-color: #eff0f1;";

    const int timeBacklight=200;

    const int dialogHelpPosX=100,dialogHelpPosY=100, dialogHelpW=600,dialogHelpH=660;

    QString qSettingsFileName;

    int erosion=0;
    int dilation=0;
    double dimFactorXY=0.0;
    double dimDistZ=0.0;
    double dimFactorZ=0.0;
    int colorHMin=0;
    int colorSMin=0;
    int colorVMin=0;
    int colorHMax=0;
    int colorSMax=0;
    int colorVMax=0;

    QString ipAdress="0.0.0.0";
    int rack=0;
    int slot=0;
    int dbReadNb=0;
    int dbWriteNb=0;

    void keyPressEvent(QKeyEvent *keyEvent);

    void InternalSignalConn();
    void ExternalSignalConn();
    void SetIpAdressValidator();
    void DisableWidgets();
    void EnableWidgets();
    void ReadSettings();
    void BlockSignals();
    void UnblockSignals();

    void LoadParamToUIValues();
    QString GetIpAdress();

signals:
    void plcDataExchange_start(QString ip,int r, int s,int dbR,int dbW);
    void plcDataExchange_stop();

private slots:
    void AppExit();
    void SaveSettings();
    void on_pushButtonHelpDBRead_clicked();
    void on_pushButtonHelpDBWrite_clicked();

    void on_pushButtonStart_clicked();
    void on_pushButtonExit_clicked();
    void on_pushButtonStop_clicked();

    void on_horizontalSliderHMin_valueChanged(int value);
    void on_horizontalSliderSMin_valueChanged(int value);
    void on_horizontalSliderVMin_valueChanged(int value);
    void on_horizontalSliderHMax_valueChanged(int value);
    void on_horizontalSliderSMax_valueChanged(int value);
    void on_horizontalSliderVMax_valueChanged(int value);

    void on_spinBoxHMin_valueChanged(int arg1);
    void on_spinBoxSMin_valueChanged(int arg1);
    void on_spinBoxVMin_valueChanged(int arg1);
    void on_spinBoxHMax_valueChanged(int arg1);
    void on_spinBoxSMax_valueChanged(int arg1);
    void on_spinBoxVMax_valueChanged(int arg1);
    void on_pushButtonGetColorFromCam_clicked();


public slots:
    void UpdateCameraFrame(QImage img);
    void UpdateProcessFrame(QImage img);
    void SetDetectHsvValues(int hMin, int hMax, int sMin, int sMax, int vMin, int vMax);

};

#endif // MAINWINDOW_H
