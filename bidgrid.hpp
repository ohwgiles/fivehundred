#ifndef BIDGRID_HPP
#define BIDGRID_HPP

#include <QWidget>

class Bid;

class BidGrid : public QWidget
{
    Q_OBJECT
public:
    explicit BidGrid(QWidget *parent = 0);
    void setWinningBid(const Bid& bid);
    Bid selectedBid();

protected:
    void paintEvent(QPaintEvent *);
    void drawRect(QPainter& painter, uint i, uint j);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    int mousePosToIndex(QMouseEvent* ev);
    enum { POS_UNSET = -2, POS_NONE = -1 };
    int coordToIndex(uint i, uint j) { return 6*j + i; }
    int m_mouse_square;
    int m_selected_square;
    int m_disabled_threshold;
    QPixmap m_grid_image;
    static const uint X_OFFSET = 61;
    static const uint Y_OFFSET = 48;
    static const uint X_SIZE = 60;
    static const uint Y_SIZE = 40;

signals:
    void bidSelected();

public slots:

};

#endif // BIDGRID_HPP
