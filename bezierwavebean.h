#ifndef BEZIERWAVEBEAN_H
#define BEZIERWAVEBEAN_H

#include <QObject>
#include <QWidget>
#include <QPaintEvent>
#include <QList>
#include <QPainter>
#include <random>
#include <QTimer>
#include <QDebug>

class BezierWaveBean : public QObject
{
    Q_OBJECT
public:
    BezierWaveBean(QWidget *parent);

    void start();
    void resume();
    void pause();

    void set_offsety(int x);

    QPainterPath getPainterPath(QPainter &painter);

protected:
    int getRandomHeight();

signals:

public slots:
    void slotUpdateAims();
    void slotMovePoints();
    void slotSetOffset();

protected:
    QWidget* target;
    int _offsety; // 多层波浪y轴偏移
    int _max_offsety; // y轴上下偏移的最大值

    QTimer* update_timer;
    QTimer* move_timer;
    QTimer* offset_timer;
    QTimer* pause_timer;

    bool running;

    int count;
    int inter;
    int def_speed1;

    int speedy, offsety;
    int offsety_direct;
    int speedx, offsetx;

    QList<QPoint>keys;
    QList<QPoint>aim_keys;
};

#endif // BEZIERWAVEBEAN_H