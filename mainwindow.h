#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//引入类
#include <QDebug>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QChartView>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QTimer>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextBrowser>
#include <QTextEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QComboBox>


/* 图标实验  必须添加命名空间 */
QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /* 申明变量 */
    /* 按钮 */
    QPushButton *camera_pushbutton;
    QPushButton *t_h_pushbutton;

    /* LED */
    QSlider *led_horizontalSlider;
    QSlider *led_verticalSlider;
    QLabel *led_label_h;
    QLabel *led_label_v;
    QGroupBox *led_groupBox;
    QVBoxLayout *led_vBoxLayout;

    /*  温湿度图表  */
    int t_h_maxsize;                 //数据最大接收值
    int t_h_max_x;                   //x轴最大值
    int t_h_max_y;                   //y轴最大值
    QValueAxis *t_h_axis_x;          //x轴
    QValueAxis *t_h_axis_y;          //y轴
    QList<int> t_h_data;

    QSplineSeries *t_h_splineseries; //曲线
    QChart *t_h_chart;               //图表
    QChartView *t_h_chartview;       //图表视图
    QTimer *t_h_timer;

    /* 串口 */
    QPushButton *serial_connect;
    QPushButton *serial_refresh;
    QSerialPort *serial_port;
    QComboBox *serial_combobox[2];
    QVBoxLayout *serial_vboxlayout; //垂直分布
    QGridLayout *serial_gridlayout; //网络布局

    QWidget *serial_funcwidget;
    QWidget *serial_mainwidget;
    QLabel *serial_label_number[2];

    /* 网络 */
    QPushButton *net_get;
    QPushButton *net_clean;  //清除浏览框内容按键

    QTextBrowser *net_textbrowser;      //文本浏览框
    QWidget *net_vwidget;               //垂直widget容器
    QVBoxLayout *net_vboxlayout;        //垂直布局

    QTimer *net_timer;                  //定时器


    /* 初始化函数 */
    void init_led();
    void init_pushbutton_function();
    void init_sensor_t_h();
    void init_serial_port();
    void init_network();

    QString network_gethostinfo();//获取本机的网络的信息，返回类型是QString


private slots:
    /* 槽函数  */
    void led_horizontalSliderValueChanged(int);
    void led_verticalSliderValueChanged(int);


    void pushButton_graph_Clicked();
    void pushButton_camera_Clicked();

    /* 图表 */
    void t_h_timeout();
    void t_h_receive_date(int value);

    /* 网络 */
    void network_timeout();
    void network_showhostinfo();
    void network_timerstart();
    void network_clearhostinfo();
};
#endif // MAINWINDOW_H
