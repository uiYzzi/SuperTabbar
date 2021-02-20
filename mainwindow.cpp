#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tt->setStyleSheet("DTabBar::tab:disabled{width:0;color:transparent;}");
    titlebar()->setCustomWidget(tt);
    titlebar()->setMenuVisible(false);
    tt->setTabsClosable(true);
    ui->tabWidget->tabBar()->hide();
    DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxInformation),"亲，请点击想要添加的窗口");
    connect(tt,&DTabBar::currentChanged,[=](int index){
        ui->tabWidget->setCurrentIndex(index);
        //qDebug()<<index;
    });
    connect(tt,&DTabBar::tabAddRequested,[=](){
        MainWindow::addPage();
    });
    connect(tt,&DTabBar::tabBarDoubleClicked,[=](int index){
            MainWindow::delPage(index);
    });
    connect(tt,&DTabBar::tabCloseRequested,[=](int index){
            MainWindow::delPage(index);
    });
}
void MainWindow::delPage(int index)
{
    win[index]->setParent(NULL);
    winw[index]->setParent(NULL);
    tt->setTabEnabled(index, false);
    //delete  winw[index];
    tt->removeTab(index);
    for(int i=0;i<=maxpage;i++)
    {
        win[index]=win[index+i];
        winw[index]=winw[index+i];
    }
    //ui->tabWidget->removeTab(index);
    maxpage--;
}
void MainWindow::addPage()
{
    QString winid,winname,output;
    QProcess *cmd=new QProcess;
    cmd->start("xwininfo");
    cmd->waitForFinished();
    output=cmd->readAllStandardOutput();
    output=output.simplified();
    int i=output.indexOf("Window id: ")+11;
    int a=output.indexOf("\" Absolute")-20;
    winid=output.mid(i,9);
    winname=output.mid(i+11,a-i+9);

    if(winid.toInt(NULL,16)==MainWindow::winId())
    {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),"QAQ 干嘛点我！");
        return;
    }else if(winname=="桌面")
    {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),"QAQ 干嘛点桌面！");
        return;
    }else if(winname=="DDE Dock")
    {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),"QAQ 干嘛点DOCK！");
        return;
    }

    int index=tt->addTab(winname);;
    //qDebug()<<index;
    wid=WId(winid.toInt(NULL,16));
    win[index] = QWindow::fromWinId(wid);
    winw[index]= QWidget::createWindowContainer(win[index],NULL,Qt::FramelessWindowHint);;
    ui->tabWidget->insertTab(index,winw[index],winname);
    ui->tabWidget->setCurrentIndex(index);
    tt->setCurrentIndex(index);
    maxpage=maxpage+1;
    return ;
}
void MainWindow::closeALL()
{
    /*for (int i=0;i<=maxpage;i++) {
        win[i]->setParent(NULL);
        winw[i]->setParent(NULL);
        qDebug()<<i;
    }*/
    while (maxpage>=0) {
        win[maxpage]->setParent(NULL);
        winw[maxpage]->setParent(NULL);
        tt->setTabEnabled(maxpage,false);
        tt->removeTab(maxpage);
        maxpage--;
    }
}
void MainWindow::closeEvent(QCloseEvent *e)
{
    MainWindow::closeALL();
    //e->ignore();
    QApplication* app;
    app->exit(0);
 }
