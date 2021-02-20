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
DWIDGET_USE_NAMESPACE
namespace Ui {
class MainWindow;
}

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void addPage();
    void delPage(int index);
    void closeALL();
private:
    void closeEvent(QCloseEvent *e);
    int maxpage=-1;

    QWindow *win[20];
    QWidget *winw[20];
    WId wid;
    DTabBar *tt=new DTabBar;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
