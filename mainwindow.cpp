  #include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    listSendTimer = new QTimer();
    qDebug() << connect(listSendTimer,SIGNAL(timeout()),this,SLOT(on_timeout_listSendTimer()));
    u2c = new USB2CAN_driver();

    ui->label_3->setVisible(false);
    //u2c->setReceiveDataType(1);
    //connect(u2c,SIGNAL(dataReceived(QByteArray)),this,SLOT(on_dataReceived(QByteArray)));
    u2c->setReceiveDataType(2);
    connect(u2c,SIGNAL(dataReceived(QString)),this,SLOT(on_dataReceived(QString)));
}

MainWindow::~MainWindow(){
    delete ui;
}
/////
void MainWindow::on_dataReceived(QString data){
    ui->RX_textEdit->append(data);


    if(u2c->correctInit == true){
        if(ui->label_3->isVisible() == true){
            ui->label_3->setVisible(false);
        }
        else{
            ui->label_3->setVisible(true);
        }
    }
    else{
        ui->label_3->setVisible(false);
    }
}
/////
void MainWindow::on_dataReceived(QByteArray data){
    //ui->RX_textEdit->append(QString::fromLatin1(data));
    QString HtS;// = QByteArray::fromHex(data.at(4));
    QByteArray temp;
    //HtS = QByteArray::fromHex(data.at(1));
    uchar c;
    for(int i=0;i<data.size();i++){
        c = data.at(i);
        HtS.append(QString("%1").arg(c,2,16,QChar('0')));
    }
    ui->RX_textEdit->append(HtS);
    ui->RX_textEdit->append("\n");
    //ui->RX_textEdit->append(QString(data));
    HtS.clear();

    /*
    if(u2c->correctInit == true){
        ui->label_3->setVisible(true);
        ui->label_3->setStyleSheet("font-weight: strike; color: green");
    }
    else{
        //ui->label_3->setStyleSheet("font-weight: bold; color: red");
        ui->label_3->setVisible(false);
    }
    */

    if(u2c->correctInit == true){
        if(ui->label_3->isVisible() == true){
            ui->label_3->setVisible(false);
        }
        else{
            ui->label_3->setVisible(true);
        }
    }
    else{
        ui->label_3->setVisible(false);
    }
}

bool MainWindow::event(QEvent *event){
    if(event->type() == QEvent::Resize){
        ui->tabWidget->resize(ui->centralwidget->size());
        return true;
    }
    else if(event->type() == QEvent::Close){
        u2c->close();

    }
    return QMainWindow::event(event);
}

//connect disconnect
void MainWindow::on_pushButton_released(){
    connectButton = !connectButton;
    if(connectButton == true){
        ui->pushButton->setText("Connected");
        ui->RX_textEdit->append(QString::number(ui->lineEdit_baudrate->text().toInt()));

        u2c->connectToPort(ui->lineEdit->text(),ui->lineEdit_baudrate->text().toInt());           //COM5
        if(u2c->port_USB2CAN->isOpen()){
            qDebug() << "port is open? "<< u2c->port_USB2CAN->isOpen() << "Port name:" << ui->lineEdit->text();
        }
    }
    else{                                                   //connectButton == false
        u2c->port_USB2CAN->close();
        qDebug() << "Close port" << !u2c->port_USB2CAN->isOpen();
        ui->pushButton->setText("Disconnected");
    }

    if(u2c->port_USB2CAN->isOpen() == true){
        ui->pushButton->setText("conn");
        qDebug() << "port is open? "<< u2c->port_USB2CAN->isOpen() << "Port name:" << ui->lineEdit->text();
    }
    else{
        ui->pushButton->setText("disconn");
    }
}

void MainWindow::on_pushButton_SendBtn_clicked()
{
    QString txt = ui->TX_textEdit->toPlainText();
    if((txt.isEmpty() == 0)&&(u2c->port_USB2CAN->isOpen())){
        if(txt.at(0) == '#'){
            menu_sendCommands(txt.sliced(1),false);
        }
        else{
            //qDebug() << "length: " << u2c->SendHex(QByteArray::fromHex(txt.toLocal8Bit()));
            ui->RX_textEdit->setText("\n WRONG CMD- use #\n");
            //u2c->SendHex(QByteArray::fromHex(outputTxt.front().toLocal8Bit()));
        }
    }
    else if (!u2c->port_USB2CAN->isOpen()){
       ui->RX_textEdit->setText("ERROR: Device is not connected !\n");
    }
}


void MainWindow::on_pushButton_ListSendBtn_released(){
    if(u2c->port_USB2CAN->isOpen()){
        QString inputTXT = ui->TX_textEdit->toPlainText();
        QString outputTxt_string;
        //find the CR and LF
        for(int i=0; i < inputTXT.length();i++){
            if(inputTXT.at(i) == '\n'){
               //outputTxt.at(pDeque).push_back(inputTXT.at(i));
               //pDeque++;
                outputTxt_string.append(inputTXT.at(i));
                outputTxt.push_back(outputTxt_string);
                outputTxt_string.clear();
            }
            else{
                outputTxt_string.append(inputTXT.at(i));
            }
        }
        //RUN timer or Thread
        //listSendTimer->setInterval(QString::number(ui->lineEdit_lineEdit_timdelaylist->text()));
        //QString value = ui->lineEdit_lineEdit_timdelaylist->text();
        listSendTimer->setInterval(ui->lineEdit_lineEdit_timdelaylist->text().toInt());
        listSendTimer->start();
    }
    else{
        ui->RX_textEdit->append("The port is not opened");
    }
}
/*
void MainWindow::..() {
    QString inputTXT = ui->TX_textEdit->toPlainText();
    QString temp;
    QStringList outputList;
    //read from GUI
    for(int i=0; i < inputTXT.length();i++){
        if(inputTXT.at(i) == '\n'){
            outputTxt_string.append(inputTXT.at(i));
            outputTxt.push_back(outputTxt_string);
            outputTxt_string.clear();
        }
        else{
            outputTxt_string.append(temp.at(i));
        }
    }
    //clear syntax
    //put the list to output

}
*/
void MainWindow::on_timeout_listSendTimer(){
    if(outputTxt.size() == 0){
        listSendTimer->stop();
    }
    else{
        //u2c->SendHex(QByteArray::fromHex(outputTxt.front().toLocal8Bit()));
        //menu_sendCommands(outputTxt.front().toLocal8Bit().sliced(1));
        menu_sendCommands(outputTxt.front().toLocal8Bit().sliced(1),true);
        outputTxt.pop_front();
    }

}

void MainWindow::menu_sendCommands(QString cmd,bool list){
    //Commands:
    try {
        if(cmd.sliced(0,3).compare("msg") == 0 && list == false){
            qDebug() << "msg cmd" << cmd << cmd.sliced(3) << QByteArray::fromHex(cmd.sliced(3).toLocal8Bit());
            //hex to char
            //QByteArray test = cmd.sliced(3).toLatin1();
            //u2c->SendHex(QByteArray::fromHex(cmd.sliced(3).toLocal8Bit())); //this argument put to the u2c->writeCANmsg()
            u2c->writeCANmsg(cmd.sliced(3));
        }
        else if(cmd.sliced(0,3).compare("msg") == 0 && list == true){
            qDebug() << "Start list init" << cmd;
            /*
            QStringList cmdlist;
            for(int i=0;i<=(int)outputTxt.size();i++){
                cmdlist.push_back(outputTxt.at(i));
            }
            u2c->listCANmsg(cmdlist,300);
            */
            u2c->writeCANmsg(cmd,3);
        }
        else if(cmd.compare("init") == 0){
            qDebug() << "Start init" << cmd;
            u2c->init();
        }
        else if(cmd.compare("initTest") == 0){
            qDebug() << "Start init" << cmd;
            u2c->init_test();
        }
        else if(cmd.compare("deinit") == 0){
            qDebug() << "Start init" << cmd;
            u2c->deinit();
        }
        else if(cmd.compare("boot") == 0){
            qDebug() << "Set boot mode" << cmd;
            u2c->Boot_Mode();
        }
        else if(cmd.compare("config") == 0){
            qDebug() << "Set config mode" << cmd;
            u2c->Config_Mode();
        }
        else if(cmd.sliced(0,1).compare("w") == 0){
            if(cmd.size() >= 4){
                //qDebug() << QByteArray::fromHex(cmd.sliced(4).toUtf8());
                //u2c->WriteReg(QByteArray::fromHex(cmd.sliced(1,2).toUtf8()),QByteArray::fromHex(cmd.sliced(4).toUtf8()));
                u2c->WriteReg(QByteArray::fromHex(cmd.sliced(1,2).toUtf8()),QByteArray::fromHex(cmd.sliced(3).toUtf8()));
            }
            else{
                //CMD is short
                //QColor errorCol = QColor::red();
                ui->TX_textEdit->append("ERROR /cmd is short/");
            }
        }
        else{
            qDebug() << "default cmd" << cmd << QByteArray::fromHex(cmd.toLocal8Bit());
            //hex to char
            QByteArray test = cmd.toLatin1();
            u2c->SendHex(QByteArray::fromHex(cmd.toLocal8Bit()));
        }
    }catch (const std::exception& e) {
        qDebug() << e.what();
    }
}
/*
//pushButton_TX_clean
void MainWindow::on_pushButton_3_released()
{
    ui->TX_textEdit->clear();
}
*/


void MainWindow::on_pushButton_RX_clean_released(){
    ui->RX_textEdit->clear();
}


void MainWindow::on_pushButton_TX_clean_released(){
    ui->TX_textEdit->clear();
}


void MainWindow::on_pushButton_showDev_released()
{
    infoCOM();
}
void MainWindow::infoCOM(){
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        if(info.serialNumber().isEmpty()){
            /*
            QString s = QObject::tr("Port: ") + info.portName()
            + QObject::tr("\tSerial number: ") + info.serialNumber() + "\n"
            + QObject::tr("\tProduct Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
            + QObject::tr("\tVendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n";
            */

                QString s = QObject::tr("Port: ") + info.portName()
                + QObject::tr("\tLocation: ") + info.systemLocation() + "\n"
                + QObject::tr("\tDescription: ") + info.description() + "\n"
                + QObject::tr("\tManufacturer: ") + info.manufacturer() + "\n"
                + QObject::tr("\tSerial number: ") + info.serialNumber()
                + QObject::tr("\tVendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                + QObject::tr("\tProduct Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n";
            ui->TX_textEdit->append(s);
        }
    else{
            /*
            QString s = QObject::tr("Port: ") + info.portName()
                    + QObject::tr("\tSerial number: ") + info.serialNumber() + "\n"
                    + QObject::tr("\tProduct Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
                    + QObject::tr("\tVendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n";
            */
        //    QString s = QObject::tr("Port: ") + info.portName() + "\n";
        //+ QObject::tr("Location: ") + info.systemLocation() + "\n"
        //+ QObject::tr("Description: ") + info.description() + "\n"
        //+ QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
        //+ QObject::tr("Serial number: ") + info.serialNumber();
        //+ QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
        //+ QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n";
       // + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

            QString s = QObject::tr("Port: ") + info.portName()
            + QObject::tr("\tLocation: ") + info.systemLocation() + "\n"
            + QObject::tr("\tDescription: ") + info.description() + "\n"
            + QObject::tr("\tManufacturer: ") + info.manufacturer() + "\n"
            + QObject::tr("\tSerial number: ") + info.serialNumber()
            + QObject::tr("\tVendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
            + QObject::tr("\tProduct Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n";
        ui->TX_textEdit->append(s);
        }
    }
}

void MainWindow::on_radioButton_toggled(bool checked)
{
    //Motor activation, send
    if(checked == true){
        ui->dial_position->setVisible(checked);
        //Activation of motor
        QString ID_PPmode = "701";
        QString ID_ = "601";
        QString start_1 = "0100";
        QString switch_OFF_2 = "2B40600000000000";
        QString ready_mode_3 = "2B40600007";
        QString switch_ON_4 =  "2B4060000F";
        QString operation_mode_5 = "2B4060000F";

        QString activation_array[5] = {ID_PPmode+"/"+start_1,
                                       ID_+"/"+switch_OFF_2,
                                       ID_+"/"+ready_mode_3,
                                       ID_+"/"+switch_ON_4,
                                       ID_+"/"+operation_mode_5};

        //ui->dial_position.
        for(int i = 0;i<=4;i++){
            qDebug() << activation_array[i];
            u2c->writeCANmsg(activation_array[i]);  //writeCANmsg list send add to the driver class
        }
    }
    else{
        ui->dial_position->setVisible(checked);
    }
}


void MainWindow::on_dial_position_valueChanged(int value){

    QString prepareForStart = "2B4060001F000000";
    QString activationOfStart = "2B4060000F000000";
    QString ID_ = "601";
    QString set_position = "237A6000";
    //QString set_position = "237A6000AABB0000";


    ui->lcdNumber->display(value);
    dial_position = value;
    //QByteArray val = QByteArray::fromHex(QString::number(dial_position));
    QString hexvalue = QString("%1").arg(dial_position, 8, 16, QLatin1Char( '0' ));
    std::reverse_copy(hexvalue.constBegin(),hexvalue.constEnd(),hexvalue.begin());
    hexvalue.remove(6,hexvalue.size());
    hexvalue.append("00");
    set_position.append(hexvalue);
    ui->RX_textEdit->setText(hexvalue);
    QString Mov[5] = {ID_+"/"+set_position,
                     ID_+"/"+prepareForStart,
                     ID_+"/"+activationOfStart};
    //QString val = QString::number(dial_position);
}


void MainWindow::on_pushButton_2_released()
{
    u2c->Get_Mode();
}

