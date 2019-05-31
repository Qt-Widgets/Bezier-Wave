#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srand(time(nullptr));

    count = 14; // 点的总数
    inter = geometry().width()/(count-4);
    def_speed1 = 20;

    speedy = def_speed1; // y移动速度
    offsety = 0; // y累计偏移
    offsety_direct = -1; // y短期偏移方向，-1上 和 1下
    speedx = 4; // x移动速度
    offsetx = 0; // x累计偏移

    // 初始化一个值
    for (int i = 0; i < count; i++)
        aim_keys.append(QPoint(inter*(i-2), getRandomHeight()));
    for (int i = 0; i < aim_keys.size(); i++)
//        keys.append(aim_keys.at(i));
        keys.append(QPoint(aim_keys.at(i).x(), this->geometry().height()));
    slotUpdatePositions();

    // 更新目标点的时钟
    update_timer = new QTimer(this);
    update_timer->setInterval(500);
    update_timer->start();
    connect(update_timer, SIGNAL(timeout()), this, SLOT(slotUpdatePositions())); // 每隔一段时间更新一下位置

    // 移动波浪的时钟
    move_timer = new QTimer(this);
    move_timer->setInterval(50);
    move_timer->start();
    connect(move_timer, SIGNAL(timeout()), this, SLOT(slotMovePositions()));

    // y偏移波浪的时钟
    offset_timer = new QTimer(this);
    offset_timer->setInterval(3000);
    offset_timer->start();
    connect(offset_timer, SIGNAL(timeout()), this, SLOT(slotSetOffset()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    // 每次绘图，更新一次偏移量
    if (offsety+offsety_direct > -geometry().height()/4 && offsety+offsety_direct < geometry().height()/4)
        offsety += offsety_direct;
    for (int i = 0; i < keys.length(); i++)
    {
        keys[i].setX(keys[i].x()+speedx);
        aim_keys[i].setX(keys[i].x());
    }

    offsetx += speedx;
    if (offsetx >= inter)
    {
        QPoint p1(keys[0].x()-inter*2, getRandomHeight());
        QPoint p2(keys[0].x()-inter, getRandomHeight());
        keys.insert(0, p2);
        keys.insert(0, p1);
        aim_keys.insert(0, QPoint(p2));
        aim_keys.insert(0, QPoint(p1));

        offsetx -= inter*2;

        if (keys.length() > count+2)
        {
            keys.removeLast();
            keys.removeLast();
            aim_keys.removeLast();
            aim_keys.removeLast();
        }
    }

    // 从关键点生成绘图点
    QList<QPoint>pots;
    for (int i = 0; i < keys.length(); i++)
    {
        if (i & 1) // 奇数
        {
            int x = (keys.at(i-1).x()+keys.at(i).x())/2;
            int y = (keys.at(i-1).y()+keys.at(i).y())/2;
            pots.append(QPoint(x, y));
            pots.append(keys.at(i));
        }
        else // 偶数
            pots.append(keys.at(i));
    }

    // x/y方向的偏移
    for (int i = 0; i < pots.length(); i++)
    {
        pots[i].setY(pots[i].y()+offsety);
    }

    // 开始画图，设置painter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);

    // 画进行判断的点
    /*for (int i = 0; i < pots.length(); i++)
    {
        QPoint p = pots.at(i);
        painter.drawEllipse(QRect(p.x()-2, p.y()-2, 4, 4));
    }*/

    // 开始画图
    QPainterPath bezier;
    bezier.moveTo(0, geometry().height());
    bezier.lineTo(pots.at(1));
    //painter.drawLine(pots.at(0), pots.at(1));
    for (int i = 2; i+2 < pots.count(); i+=3)
    {
        // 画切线线条
        //painter.drawLine(pots.at(i-1), pots.at(i));
        //painter.drawLine(pots.at(i+1), pots.at(i+2));

        // 画三阶贝塞尔曲线
        bezier.cubicTo(pots.at(i), pots.at(i+1), pots.at(i+2));
    }
    bezier.lineTo(geometry().bottomRight());
    painter.fillPath(bezier, QBrush(Qt::red));
//    painter.setPen(Qt::red);
    //painter.drawPath(bezier);
    return QMainWindow::paintEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    /*inter = geometry().width() / (count - 4);
    // 调整所有点的横坐标
    for (int i = 0; i < keys.length(); i++)
    {
        int x = inter*(i-2);
        keys[i].setX(x);
        aim_keys[i].setX(x);
    }*/

    return QMainWindow::resizeEvent(e);
}

int MainWindow::getRandomHeight()
{
    return rand() % (geometry().height()/2) + geometry().height()/2;
}

void MainWindow::slotUpdatePositions()
{
    if (speedy == def_speed1) speedy = 1;
    // 生成随机的目标关键点
    for (int i = 0; i < aim_keys.length(); i++)
    {
        aim_keys[i].setY(getRandomHeight());
    }

    /*QString str = "更新目标 ";
    for (int i = 0; i < aim_keys.size(); i++)
        str += " " + QString::number(aim_keys.at(i).y());
    qDebug() << str;*/
}

void MainWindow::slotMovePositions()
{
    // 慢慢移动当前关键点到目标关键点
    for (int i = 0; i < keys.length(); i++)
    {
        QPoint& cur = keys[i];
        QPoint aim = aim_keys[i];
        int del = aim.y()-cur.y();
        if (del > 0)
        {
            if (speedy < del)
                cur.setY(cur.y()+speedy);
            else
                cur.setY(cur.y()+del);
        }
        else if (del < 0)
        {
            if (speedy < -del)
                cur.setY(cur.y()-speedy);
            else
                cur.setY(cur.y()+del);
        }
        else
            continue;
    }
    // 更新当前界面
    this->update();

    /*QString str = "移动点集 ";
    for (int i = 0; i < keys.size(); i++)
        str += " " + QString::number(keys.at(i).y());
    qDebug() << str;*/
}

void MainWindow::slotSetOffset()
{
    // 每隔一段时间，稍微修改波浪的上下位置
    if (rand() & 1)
        offsety_direct = 1;
    else
        offsety_direct = -1;
    //qDebug() << "y : offset_direct" << offsety_direct << "    " << offsety;
}