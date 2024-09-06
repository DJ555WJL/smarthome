#include "mainwindow.h"
#include <QDateTime>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(0, 0, 800, 480);

    init_led();
    init_pushbutton_function();
    init_serial_port();
    init_network();
    init_usbbluetooth();
}

MainWindow::~MainWindow()
{
}


/************** led *************/
void MainWindow::led_horizontalSliderValueChanged(int val)
{
    QString str = "亮度: " + QString::number(val);
    led_label_h->setText(str);

    /* to do */
}
void MainWindow::led_verticalSliderValueChanged(int val)
{
    QString str = "亮度: " + QString::number(val);
    led_label_v->setText(str);

    /* to do */
}
void MainWindow::init_led()
{
    led_horizontalSlider = new QSlider(Qt::Horizontal,this);
    led_verticalSlider = new QSlider(Qt::Vertical,this);
    led_label_h = new QLabel("亮度: 0",this);
    led_label_v = new QLabel("亮度: 0",this);
    led_groupbox = new QGroupBox(tr("LED"),this);
    led_vboxlayout = new QVBoxLayout();

    led_vboxlayout->addWidget(led_label_h);
    led_vboxlayout->addWidget(led_horizontalSlider);
    led_vboxlayout->addWidget(led_verticalSlider);
    led_vboxlayout->addWidget(led_label_v);

    led_groupbox->setLayout(led_vboxlayout);

    /* 设置滑条 值的大小 */
    led_horizontalSlider->setRange(0, 100);
    led_verticalSlider->setRange(0, 100);

    /* 设置区域大小，后续根据要求修改 */
    led_groupbox->setGeometry(10, 250, 140, 200);

    /* 信号槽 */
    connect(led_horizontalSlider,SIGNAL(valueChanged(int)),
            this,SLOT(led_horizontalSliderValueChanged(int)));
    connect(led_verticalSlider, SIGNAL(valueChanged(int)),
            this,SLOT(led_verticalSliderValueChanged(int)));
}

/****************** 按钮 ********************/
void MainWindow::pushButton_camera_Clicked()
{
//    this->close();
}

void MainWindow::pushButton_graph_Clicked()
{
    init_sensor_t_h();
}

void MainWindow::init_pushbutton_function()
{
    camera_pushbutton = new QPushButton("监控", this);
    t_h_pushbutton = new QPushButton("温湿度", this);

    /* 设置显示的位置与大小 */
    t_h_pushbutton->setGeometry(300, 200, 80, 40);
    camera_pushbutton->setGeometry(400, 200, 80, 40);

    /* 信号槽 */
    connect(t_h_pushbutton, SIGNAL(clicked()),this,
                SLOT(pushButton_graph_Clicked()));
    connect(camera_pushbutton, SIGNAL(clicked()),this,
                SLOT(pushButton_camera_Clicked()));
}

void MainWindow::init_sensor_t_h()
{
    t_h_maxsize = 51;                      //最大存储 maxsize -1 个 数据
    t_h_max_x = 5000;
    t_h_max_y = 40;

    t_h_splineseries = new QSplineSeries();//曲线，折线是QLineSeries
    t_h_chart = new QChart();              //图表
    t_h_chartview = new QChartView();      //图表视图
    t_h_axis_x = new QValueAxis();         //坐标轴
    t_h_axis_y = new QValueAxis();
    t_h_timer = new QTimer(this);

    /* legend 译图例类型 以绘图的颜色为区分，此处隐藏 */
    t_h_chart->legend()->hide();
    t_h_chart->setTitle("温湿度读取曲线");           //设置标题
    t_h_chart->addSeries(t_h_splineseries);       //添加曲线

    /* y轴 */
    t_h_axis_y->setLabelFormat("%i");             //显示格式
    t_h_axis_y->setTitleText("温度/.C");           //y轴标题位置
    t_h_chart->addAxis(t_h_axis_y, Qt::AlignLeft);//y轴标题位置 (设置坐标轴的方向)
    t_h_axis_y->setRange(0,t_h_max_y);            //设置y轴范围
    t_h_splineseries->attachAxis(t_h_axis_y);

    /* x轴 */
    t_h_axis_x->setLabelFormat("%i");
    t_h_axis_x->setTitleText("时间/ms");
    t_h_chart->addAxis(t_h_axis_x, Qt::AlignBottom);
    t_h_axis_x->setRange(0, t_h_max_x);
    t_h_splineseries->attachAxis(t_h_axis_x);

    /* 将图标的内容设置为图表视图上 */
    t_h_chartview->setChart(t_h_chart);
    /* 设置抗锯齿 */
    t_h_chartview->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(t_h_chartview);//中心部件

    /*
     * 注意：操作部分，后期更换具体数据传入
     */
#if 1
    t_h_timer->start(200);                         //定时200ms

    /* 槽函数 */
    connect(t_h_timer, SIGNAL(timeout()),this,SLOT(t_h_timeout()));

    qsrand(time(NULL));                           //设置随机种子，随机数数据化
#endif
}

void MainWindow::t_h_timeout()
{
    /* 产生随机 0~max_y 之间的数据 */
    t_h_receive_date(qrand() % t_h_max_y);
}

void MainWindow::t_h_receive_date(int value)
{
    t_h_data.append(value);

    while(t_h_data.size() > t_h_maxsize){
        /* 移除第一个数据 */
        t_h_data.removeFirst();
    }

    t_h_splineseries->clear();//清空
    /* 计算 x 轴上的点与点之间显示的间距 */
    int xspace = t_h_max_x / (t_h_maxsize -1);

    /* 添加点， xSpace * i 表示第 i 个点的 x 轴的位置 */
    for(int i = 0; i < t_h_data.size(); ++i){
        t_h_splineseries->append(xspace * i, t_h_data.at(i));
    }
}

/******************* serial port ***********************/
void MainWindow::init_serial_port()
{
    serial_connect = new QPushButton("connect",this);
    serial_refresh = new QPushButton("refresh",this);
    serial_port = new QSerialPort(this);
    serial_gridlayout = new QGridLayout();
    serial_vboxlayout = new QVBoxLayout();
    serial_funcwidget = new QWidget(this);
    serial_mainwidget = new QWidget(this);

    /* QList 链表，字符串类型 */
    QList <QString> list1;
    list1<<"串口号"<<"波特率";

    for(int i = 0; i < 2; i++){
        serial_label_number[i] = new QLabel(list1[i]);
        serial_label_number[i]->setMinimumSize(20, 15);

        /* 自动调整大小 */
        serial_label_number[i]->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Expanding);
        serial_gridlayout->addWidget(serial_label_number[i], 0, i);//添加到坐标(0，i)

        serial_combobox[i] = new QComboBox();
        serial_combobox[i]->setMinimumSize(20, 15);

        /* 自动调整大小 */
        serial_combobox[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        serial_gridlayout->addWidget(serial_combobox[i],1,i);//添加到坐标(1，0)
    }

    serial_gridlayout->addWidget(serial_connect,0,2);
    serial_gridlayout->addWidget(serial_refresh,1,2);

    /* 布局 */
    serial_funcwidget->setLayout(serial_gridlayout);
    serial_vboxlayout->addWidget(serial_funcwidget);
    serial_mainwidget->setLayout(serial_vboxlayout);   

    /* QList 链表，字符串类型 */
    QList <QString> list2;
    list2<<"1200"<<"2400"<<"4800"<<"9600"
        <<"19200"<<"38400"<<"57600"
        <<"115200"<<"230400"<<"460800"
      <<"921600";
    for (int i = 0; i < 11; i++) {
        serial_combobox[1]->addItem(list2[i]);
    }
    serial_combobox[1]->setCurrentIndex(7);

    /* 设置区域大小，后续根据要求修改 */
    serial_mainwidget->setGeometry(0,0,300,80);

}

/******************* newwork ***********************/
void MainWindow::init_network()
{
    net_get = new QPushButton("获得本机信息",this);
    net_clean = new QPushButton("清空",this);
    net_vwidget = new QWidget(this);
    net_vboxlayout = new QVBoxLayout();
    net_textbrowser = new QTextBrowser();
    net_timer = new QTimer();

    /* 添加到垂直布局 */
    net_vboxlayout->addWidget(net_get);
    net_vwidget->setLayout(net_vboxlayout);

    net_vboxlayout->addWidget(net_textbrowser);
    net_vwidget->setLayout(net_vboxlayout);

    net_vboxlayout->addWidget(net_clean);
    net_vwidget->setLayout(net_vboxlayout);

    /* 设置区域大小，后续根据要求修改 */
    net_vwidget->setGeometry(600,10,200,450);

    /* 信号槽 */
    connect(net_get,SIGNAL(clicked()),
                this,SLOT(network_timerstart()));
    connect(net_clean,SIGNAL(clicked()),
                this,SLOT(network_clearhostinfo()));
    connect(net_timer,SIGNAL(timeout()),
                this,SLOT(network_timeout()));

}

void MainWindow::network_timerstart()
{
    net_textbrowser->clear();

    net_timer->start(1000);//定时1s
}

void MainWindow::network_timeout()
{
    network_showhostinfo();

    net_timer->stop();//停止定时
}

QString MainWindow::network_gethostinfo()
{
    /* 获取主机名称 */
    QString str = "主机名称：" + QHostInfo::localHostName() + "\n";

    /* 获取所有网络接口 */
    QList<QNetworkInterface> list
            = QNetworkInterface::allInterfaces();

    /* 遍历list */
    foreach(QNetworkInterface interface, list){
        str+="网络设置：" + interface.name() + "\n";
        str+="MAC地址：" + interface.hardwareAddress() + "\n";

        QList<QNetworkAddressEntry> entryList
                = interface.addressEntries();

        /* 遍历entryList */
        foreach(QNetworkAddressEntry entry,entryList){
            /* 过滤IPV6地址，只留下IPV4 */
            if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol){
                str+="IP 地址：" + entry.ip().toString() + "\n";
                str+="子网掩码：" + entry.netmask().toString() + "\n";
                str+="广播地址：" + entry.broadcast().toString() + "\n";
            }
        }
    }

    return str;
}

void MainWindow::network_showhostinfo()
{
    net_textbrowser->insertPlainText(network_gethostinfo());
}

void MainWindow::network_clearhostinfo()
{
    if(!net_textbrowser->toPlainText().isEmpty())
        net_textbrowser->clear();
}

/******************* usb bluetooth ***********************/
void MainWindow::init_usbbluetooth()
{
    usbbt_vboxlayout = new QVBoxLayout();
    usbbt_groupbox = new QGroupBox(tr("usb bluttooth"),this);

    /* QList 链表，字符串类型 */
    QList <QString> list;
    list<<"开始扫描"<<"停止扫描"<<"连接"<<"断开";

    for(int i = 0; i < 4; i++)
    {
        usbbt_pushbutton[i] = new QPushButton();
        usbbt_pushbutton[i]->setText(list[i]);
        usbbt_vboxlayout->addWidget(usbbt_pushbutton[i]);
    }

    /* vboxLayout布局设置为groupBox布局 */
    usbbt_groupbox->setLayout(usbbt_vboxlayout);

    /* 设置区域大小，后续根据要求修改 */
    usbbt_groupbox->setGeometry(300,250,160,200);
}
