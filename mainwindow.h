#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DTitlebar>
#include <DTabBar>
#include <DTabWidget>
#include <QHBoxLayout>
#include <QWindow>
#include <QProcess>
#include <QDebug>
#include <DMessageBox>
#include <DMessageManager>
#include <DWidgetUtil>
#include <QList>
DWIDGET_USE_NAMESPACE

class EventMonitor;
namespace Ui {
class MainWindow;
}

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void addPage();
    void delPage(int index);
    void closeALL();
    void updateTabWidth();
private:
    void closeEvent(QCloseEvent *e);
    int maxpage=-1;
    QList<QWindow *> win;
    QList<QWidget *> winw;
    //QWindow *win[20];
    //QWidget *winw[20];
    WId wid;
    DTabBar *tt=new DTabBar;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
