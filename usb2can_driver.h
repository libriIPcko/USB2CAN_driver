#ifndef USB2CAN_DRIVER_H
#define USB2CAN_DRIVER_H

#include "QSerialPort"
#include "QTimer"

#include "QObject"

//#include "QSignalSpy"


class USB2CAN_driver : public QSerialPort
{
    Q_OBJECT;

public:
    USB2CAN_driver();
    //virtual ~USB2CAN_driver();

    QSerialPort *port_USB2CAN;
    QTimer *tim_interrupt_1 = new QTimer();
    QTimer *ListTimer = new QTimer();
    QTimer *initListTimer = new QTimer();
    QTimer *deinitListTimer = new QTimer();
    int initTimerDelay = 500;
    QTimer *tim;
    int tim_counter = 0;

    int temporary_init_Counter = 0;
    int deinit_Counter = 0;
    int init();
    void deinit();
    int init_test();
    bool activeInit = false;
    bool active_init_test = false;
    void USB_LoopBack();
    void Boot_Mode();
    void Config_Mode();
    void Normal_Mode();
    void LoopBack_Mode();
    QByteArray Get_Mode();

    void WriteReg(QByteArray regAdress, QByteArray value);
    QByteArray WriteCMD(QByteArray CMD_name, QByteArray value);
    QByteArray ReadReg(QByteArray regAdress);
    int SendString(QString data);
    int SendHex(QByteArray data);

    int writeCANmsg(QString msg);
    void writeCANmsg(QByteArray msg);

    //QString portName;
    int connectToPort(QString portName);
    int connectToPort(QString portName,int BaudRate);
    int disconnectedFromPort();

    bool correctInit = false;
    int msgCounter = 0;

    //@ 1 - QByteArray
    //@ 2 - QString
    int receiveDataType = 1;
    //@ 1 - QByteArray
    //@ 2 - QString
    int setReceiveDataType(int type);
    //@ 1 - QByteArray
    //@ 2 - QString
    int whatReceiveDataType();


public: signals:
    friend void QIODevice::readyRead();
    void timeout();
    void dataReceived(QByteArray);          //signal with QByteArray output
    void dataReceived(QString);             //signal with QString output


private slots:
    //For reading from driver, you have to implemented separately to own
    //QObject::connect(port_USB2CAN,SIGNAL(readyRead()),ObjWhereIsOwnMethod,SLOT(QByteArray ownMethod()));

    QByteArray read_USB2CAN();
    bool initSend();
    void initSend_1();
    bool deinitSend();
};

#endif // USB2CAN_DRIVER_H
