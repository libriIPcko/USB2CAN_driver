#include "usb2can_driver.h"
#include "USB2CAN_define.h"
#include "QSerialPort"

#include "QSerialPort"
#include "QObject"
#include "QDebug"

//#include <QSignalSpy>



USB2CAN_driver::USB2CAN_driver()
{
    port_USB2CAN = new QSerialPort();
    ListTimer = new QTimer();
    initListTimer = new QTimer();
    deinitListTimer = new QTimer();
    //qDebug() << "Connection of readyRead to sw interrupt:" <<
    connect(port_USB2CAN,SIGNAL(readyRead()),this,SLOT(read_USB2CAN()));
}

/*
USB2CAN_driver::~USB2CAN_driver(){
    QObject::disconnect(USB2CAN_driver::,SIGNAL(readyRead()),USB2CAN_driver::,SLOT(QByteArray read_USB2CAN()));
}
*/

int USB2CAN_driver::connectToPort(QString portName){
    port_USB2CAN->setPortName(portName);
    port_USB2CAN->setBaudRate(Baud9600,AllDirections);
    tim = new QTimer;
    return port_USB2CAN->open(QIODevice::ReadWrite);
}
int USB2CAN_driver::connectToPort(QString portName,int BaudRate){
    port_USB2CAN->setPortName(portName);
    port_USB2CAN->setBaudRate(BaudRate,AllDirections);
    tim = new QTimer;
    return port_USB2CAN->open(QIODevice::ReadWrite);
}

/*
void USB2CAN_driver::timEvent(){
    qDebug() << "Tim" << tim_counter++;
    if(tim_counter >= 5){
        tim_counter = 0;
        tim->stop();
    }
}
*/

int USB2CAN_driver::disconnectedFromPort(){
    port_USB2CAN->close();
    if(port_USB2CAN->isOpen()){
        return 1;
    }
    else{
        qDebug() << "------------------Port is diconected-----------------";
        return 0;
    }
}


void USB2CAN_driver::USB_LoopBack(){
}
void USB2CAN_driver::Boot_Mode(){
}
void USB2CAN_driver::Config_Mode(){
}
void USB2CAN_driver::Normal_Mode(){
}
void USB2CAN_driver::LoopBack_Mode(){
}

QByteArray USB2CAN_driver::Get_Mode(){
    while(!port_USB2CAN->waitForBytesWritten(300)){
        port_USB2CAN->write(getMode);
    }
    return USB2CAN_driver::readAll();
}

void USB2CAN_driver::WriteReg(QByteArray regAdress, QByteArray value){
    QByteArray sync = "0f";
    QByteArray out = {QByteArray::fromHex(sync) +QByteArray::fromHex("12") + regAdress + QByteArray::fromHex(QByteArray::number(value.size())) + value};
    SendHex(out);
    /*
    while(!port_USB2CAN->waitForBytesWritten(300)){
        port_USB2CAN->write(out);

    }
    */
    /*
    int length = regAdress.length() + value->length();
    QByteArray len;
    len.setNum(length);
    QByteArray sendVal[] = { writeReg, len, regAdress, *value };
    QByteArray sendData;
    sendData.fromRawData(*sendVal,sizeof (sendVal));
    while(!port_USB2CAN->waitForBytesWritten(300)){
        port_USB2CAN->write(sendData);

    }
    */
}

int USB2CAN_driver::SendString(QString data){
    //sendData.fromRawData(*sendVal,sizeof (sendVal));
    long length;
    while(!port_USB2CAN->waitForBytesWritten(300)){
        length = port_USB2CAN->write(data.toUtf8(),data.size());
    }
    return length;
}
int USB2CAN_driver::SendHex(QByteArray data){
    long length;
    while(!port_USB2CAN->waitForBytesWritten(300)){
        length = port_USB2CAN->write(data);
    }
    return length;
}
QByteArray USB2CAN_driver::WriteCMD(QByteArray CMD_name, QByteArray value){
}
QByteArray USB2CAN_driver::ReadReg(QByteArray regAdress){
}

int USB2CAN_driver::init(){
    //Init CMD by timer period
        activeInit = true;
        temporary_init_Counter = 0;
        connect(initListTimer,SIGNAL(timeout()),this,SLOT(initSend()));
        initSend();
        initListTimer->setTimerType(Qt::PreciseTimer);
        initListTimer->start(initTimerDelay);

}
void USB2CAN_driver::deinit(){
    //connect deinitialization Timer
    connect(deinitListTimer,SIGNAL(timeout()),this,SLOT(deinitSend()));
    //start deinitialization Timer
    deinitListTimer->start(400);
}

int USB2CAN_driver::init_test(){
    //TestCode
        active_init_test = true;
        temporary_init_Counter = 0;
        connect(initListTimer,SIGNAL(timeout()),this,SLOT(initSend_1()));
        initListTimer->setTimerType(Qt::PreciseTimer);
        initListTimer->start(initTimerDelay);
}
int USB2CAN_driver::setReceiveDataType(int type){
    receiveDataType = type;
}
int USB2CAN_driver::whatReceiveDataType(){
    return receiveDataType;
}

QByteArray USB2CAN_driver::read_USB2CAN(){
    //qDebug() <<"From driver RX" << USB2CAN_driver::readAll();
    QByteArray temporary = port_USB2CAN->readAll();
    msgCounter++;
    //Filtering of the continuously cmd,
    if(temporary.compare("\017\t\002\002\002") == 0){
        //qDebug()<< "filtered\n";
        msgCounter = 0;
        correctInit = true;
    }

    //All other readed datas
    else{
        //Add re-formating CAN message
        if(receiveDataType == 2){
            //Add QString rewrited from QByteArray
            QString out;
            QString recMsg[2];
            uchar c;
            for(int i=0;i<temporary.size();i++){
                c = temporary.at(i);
                out.append(QString("%1").arg(c,2,16,QChar('0')));
            }
            if(out.contains("0f410d88",Qt::CaseInsensitive)){   //Extend CAN message
                recMsg[0] = out.sliced(8,4);
                recMsg[1] = out.sliced(12,out.length()-12);
                out.clear();
                out.append(recMsg[0]);
                out.append(recMsg[1]);
                emit dataReceived(out);
            }
            else if(out.contains("0f410b08",Qt::CaseInsensitive)){  //Standard CAN message
                recMsg[0] = out.sliced(8,4);
                recMsg[1] = out.sliced(12,out.length()-12);
                qDebug() << "OK";
                out.clear();
                out.append("ID: ");
                out.append(recMsg[0]);
                out.append("\tDATA: ");
                out.append(recMsg[1]);
                emit dataReceived(out);
            }
            else{
                emit dataReceived(out); //different message than CAN receive message
            }


        }
        else{
            emit dataReceived(temporary);
        }


        //for init sub-rutine
        //Correct
        if(activeInit == true){
            initListTimer->stop();
            initSend();
            if(activeInit == true){
                initListTimer->setTimerType(Qt::PreciseTimer);
                initListTimer->start(initTimerDelay);
            }
        }
        //for init sub-rutine
        //New initialize subrutine
        if(active_init_test == true){
            initListTimer->stop();
            initSend_1();
            if(active_init_test == true){
                initListTimer->setTimerType(Qt::PreciseTimer);
                initListTimer->start(initTimerDelay);
            }
        }
        if(msgCounter%3 == 0){//after every 3 CAN RX message without OK string set corectInit to false
            correctInit = false;
        }
    }
    return 0;
}

int USB2CAN_driver::writeCANmsg(QString msg){
    QStringList inDat = msg.split("/");
    if(inDat.length() < 2){
        //Giver return = 999;
        return 999; // BAD syntax
    }

    bool ok = false;
    unsigned char prefixRawDat[] = {0x0f,0x40,0x0B}; //This (0x08) is length of data YOU MUST CALCULATE FROM DATA LENGTH
    unsigned char suffixRawDat[] = {0xD7,0x39};
    QByteArray prefixDat = QByteArray::fromRawData((char*)prefixRawDat,3);
    QByteArray suffixDat = QByteArray::fromRawData((char*)suffixRawDat,2);
    QByteArray outDat;

    QByteArray CAN_DATA = QByteArray::fromHex(inDat.at(2).toLocal8Bit());
    //Emergency if. If data set of CAN data is odd
    /*
    if((inDat.at(2).size()%2 != 0)&&(inDat.at(2) != 1)){
        return 4;   //The size of CAN data are odd
    }
    */
    //QByteArray length = (int*)CAN_DATA.length();
    QByteArray length = QByteArray::fromHex(QString("%1").arg(CAN_DATA.length(), 8, 16, QLatin1Char( '0' )).toLocal8Bit());
    length.remove(0,3);
    //outDat.append()
    //qDebug() << "length of data: " << length.size() << length.at(length.size()-1);// << QString(QByteArray::fromHex(length.at(length.length())));

    outDat.append(prefixDat);
    //outDat.append(length.at(length.length()-1));
    outDat.append(length);

    int CAN_ID_dec = inDat.at(1).toUInt(&ok,16);
    //recalculate CAN_ID
    CAN_ID_dec = (CAN_ID_dec)/0.03125;

    QByteArray CAN_ID = QByteArray::fromHex(QString("%1").arg(CAN_ID_dec, 8, 16, QLatin1Char( '0' )).toLocal8Bit());
    CAN_ID.remove(0,2);
    outDat.append(CAN_ID);


    outDat.append(CAN_DATA);
    outDat.append(suffixDat);
    //qDebug() <<CAN_ID_dec << QString(QByteArray::fromHex(outDat));
    SendHex(outDat);
    return 0;
}

void USB2CAN_driver::writeCANmsg(QByteArray msg){
}
bool USB2CAN_driver::deinitSend(){
    if(deinit_Counter == 0){
        while(!port_USB2CAN->waitForBytesWritten(200)){
            bool status;
            status = port_USB2CAN->write(Config,3);
            qDebug() << "TX:" << QString::fromLocal8Bit(Config) << "Status" << status << "Config" << deinit_Counter;
        }
    }
    else if(deinit_Counter == 1){
        while(!port_USB2CAN->waitForBytesWritten(200)){
            bool status;
            status = port_USB2CAN->write(ResetMod,3);
            qDebug() << "TX:" << QString::fromLocal8Bit(ResetMod) << "Status" << status << "ResetMod" << deinit_Counter;
        }
        //deinitListTimer->disconnect(timeout(),deinitSend());
        qDebug() << "disconnected?" << deinitListTimer->disconnect();
        deinit_Counter = 0;
        return 0;
    }
    else{
        return 1;
    }
    deinit_Counter++;
}

bool USB2CAN_driver::initSend(){
     bool stop = false;
    qDebug() << "case" << temporary_init_Counter << "time: " << initListTimer->remainingTime();
    initListTimer->stop();
    int waitForBytesWritten = 200;
    int status;
    switch (temporary_init_Counter){
        case 0:                     //1-Set to Config Mode [0x02]
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                status = port_USB2CAN->write(Config,3);
                qDebug() << "TX:" << QString::fromLocal8Bit(Config) << "Status" << status << "Config" << temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
                /*
                //USB2CAN_driver::write(Config,qstrlen(Config));
                bool ok = false;
                int cycle = 0;
                while(ok == true){
                    status = port_USB2CAN->write(Config,3);
                    while(!port_USB2CAN->waitForReadyRead(5)){
                        if((strcmp(port_USB2CAN->readAll(),"FF"))==1){
                                ok = true;
                        }
                        if(cycle >= 4){
                            ok = true;
                        }
                        qDebug() << cycle++;
                    }
                }
            }
            */
        break;
        case 1:                    //2-Set Reset Mode [0x00]on value 0x01   (by WriteReg[x12])
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(ResetMod);
                //USB2CAN_driver::write(ResetMod,qstrlen(ResetMod));
                status = port_USB2CAN->write(ResetMod,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(ResetMod) << "Status" << status << "ResetMod"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 2:                    //3-Set Clock divider [0x1F] on value 0xC0 (by WriteReg[x12])
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(ClockDivData);
                //USB2CAN_driver::write(ClockDivData,qstrlen(ClockDivData));
                status = port_USB2CAN->write(ClockDivData,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(ClockDivData) << "Status" << status << "ClockDivData"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 3:                    //4.1-Set message filter; without filtration: (by WriteReg[x12])
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(AccCode);
                //USB2CAN_driver::write(AccCode,qstrlen(AccCode));
                status = port_USB2CAN->write(AccCode,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(AccCode) << "Status" << status << "AccCode"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 4:                    //4.2 set-> Acceptance Mask [0x05] on 0xff
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(AccMask);
                //USB2CAN_driver::write(AccMask,qstrlen(AccMask));
                status = port_USB2CAN->write(AccMask,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(AccMask) << "Status" << status << "AccMask"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 5:                   //5-Set OutputControl[0x08] on 0xDA (by WriteReg[x12])
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(OutCtrl);
                //USB2CAN_driver::write(OutCtrl),qstrlen(OutCtrl);
                status = port_USB2CAN->write(OutCtrl,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(OutCtrl) << "Status" << status << "OutCtrl"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 6:                  //7. -Set Interrupt enable[]  on 0x03  (by WriteReg[x12])
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(IE);
                //USB2CAN_driver::write(IE,qstrlen(IE));
                status = port_USB2CAN->write(IE,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(IE) << "Status" << status << "IE"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 7:                 //6.1 - Bus Timing 0
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(BT0);
                //USB2CAN_driver::write(BT0,qstrlen(BT0));
                status = port_USB2CAN->write(BT0,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(BT0) << "Status" << status << "BT0"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 8:                 //6.2 - Bus Timing 1
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(BT1);
                //USB2CAN_driver::write(BT1,qstrlen(BT1));
                status = port_USB2CAN->write(BT1,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(BT1) << "Status" << status << "BT1"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 9:                 //8.1 -Set Transmit Critical Limit and Transmit Ready limit by cmd COMMAND TCL
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(CTL_Code);
                //USB2CAN_driver::write(CTL_Code,qstrlen(CTL_Code));
                status = port_USB2CAN->write(CTL_Code,4);
                qDebug() << "TX:" << QString::fromLocal8Bit(CTL_Code) << "Status" << status << "CTL_Code"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 10:               //8.2 Set-> TRL
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                //USB2CAN_driver::write(TRL_Code);
                //USB2CAN_driver::write(TRL_Code,qstrlen(TRL_Code));
                status = port_USB2CAN->write(TRL_Code,4);
                qDebug() << "TX:" << QString::fromLocal8Bit(TRL_Code) << "Status" << status << "TRL_Code"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 11:              //9-Set Normal Mode
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                status = port_USB2CAN->write(NormalMode,3);
                qDebug() << "TX:" << QString::fromLocal8Bit(ModRegDat) << "Status" << status << "NormalMode"<< temporary_init_Counter;
            }
            initListTimer->start(initTimerDelay);
        break;
        case 12:              //10-Set Mode register [0x00], the value depends on Message Filter   (by WriteReg[x12])
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                status = port_USB2CAN->write(ModRegDat,5);
                qDebug() << "TX:" << QString::fromLocal8Bit(NormalMode) << "Status" << status << "ModRegDat - final"<< temporary_init_Counter;

            }
            initListTimer->start(initTimerDelay);
        break;
        case 13:              //End of Initialize sub-routine
            stop = true;
            temporary_init_Counter = 0;
            activeInit = false;
            initListTimer->stop();
            while(!port_USB2CAN->waitForBytesWritten(waitForBytesWritten)){
                status = port_USB2CAN->write(NormalMode,3);
                qDebug() << "TX:" << QString::fromLocal8Bit(ModRegDat) << "Status" << status << "NormalMode"<< temporary_init_Counter;
            }
            qDebug() << QObject::disconnect(initListTimer);
            qDebug() << "initTimer is active? " << initListTimer->isActive();
            //return true;
        break;
        default:
            qDebug() << "ERROR init !! case" << temporary_init_Counter;
            return false;
        break;
    }

    if(stop == true){        
        initListTimer->stop();        
        QObject::disconnect(initListTimer);
        //temporary_init_Counter = 0;
        return true;
    }
    else{
        temporary_init_Counter++;
        return false;
    }



}

void USB2CAN_driver::initSend_1(){
   bool stop = false;
   qDebug() << "case" << temporary_init_Counter << "time: " << initListTimer->remainingTime();
   initListTimer->stop();
   int waitForBytesWritten = 200;
   int status;
   QByteArray reg,dat;
   bool ok;
   //reg = QByteArray::fromHex(initReg.toInt(&ok,16));
   reg = QByteArray::fromHex(initReg.toUtf8());
   dat = QByteArray::fromHex(initData.toUtf8());
   char a = reg.at(temporary_init_Counter);
   char b = dat.at(temporary_init_Counter);
   WriteReg(QByteArray::fromRawData(&a,1),QByteArray::fromRawData(&b,1));
   if(temporary_init_Counter<11){
       temporary_init_Counter++;
       initListTimer->start();
   }
   else{
       temporary_init_Counter = 0;
       active_init_test = false;
       initListTimer->stop();
       initListTimer->disconnect();
   }
}
