#ifndef HITOKOTOWIDGET_H
#define HITOKOTOWIDGET_H

#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class HitokotoWidget;
}
QT_END_NAMESPACE

class HitokotoWidget : public QWidget {
    Q_OBJECT
    Ui::HitokotoWidget* ui;
    QTimer* timer;

    std::string text;
    std::string creator;
    std::string from;
    std::string fromWho;

    QRect currentGeometry;
    QPoint mousePosition;
    bool inGeometryPressed;
    QSystemTrayIcon* tray;

    void refreshHitokotoFromNetwork();

protected:
    bool event(QEvent* event) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

public:
    void getHitokotoByNetwork();
    explicit HitokotoWidget(QWidget* parent = nullptr);
    void initTray();
    bool loadCache();
    void renderText();
    ~HitokotoWidget() override;
};

#endif // HITOKOTOWIDGET_H
