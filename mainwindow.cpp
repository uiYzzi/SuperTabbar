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
    updateTabWidth();
}
void QWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
       {
        if (event->key() == Qt::Key_Left)//单键触发
        {

        }else if (event->key() == Qt::Key_Right)//单键触发
        {

        }
    }
}
void MainWindow::keyPressEvent(QKeyEvent *event)

{
    if (event->modifiers() & Qt::ControlModifier)
       {
        if (event->key() == Qt::Key_Left)//单键触发
        {
            int index=tt->currentIndex();
            if(index!=0)
            {
                index=index-1;
                tt->setCurrentIndex(index);
                ui->tabWidget->setCurrentIndex(index);
            }
        }else if (event->key() == Qt::Key_Right)//单键触发
        {
            int index=tt->currentIndex();
            if(index!=maxpage)
            {
                index=index+1;
                tt->setCurrentIndex(index);
                ui->tabWidget->setCurrentIndex(index);
            }
        }
    }
}
void MainWindow::updateTabWidth()
{
    if(maxpage==-1)
    {
        return;
    }
    int tabWidth=(tt->width()-130)/tt->count();
    tt->hide();
    for(int i=0;i<tt->count();i++){
        if(tabWidth<40){
            tabWidth=40;
        }else if(tabWidth>220){
            tabWidth=220;
        }
        tt->setTabMinimumSize(i,QSize(tabWidth,36));
        tt->setTabMaximumSize(i,QSize(tabWidth,37));
    }
    tt->hide();
    //下边这部分是因为Dtabbar中的按钮在标签尺寸变化后不会跟随移动,只有在新建或者删除标签时才会移动.
    int tmp=tt->addTab("");
    tt->setTabMinimumSize(tmp,QSize(0,36));
    tt->setTabMaximumSize(tmp,QSize(0,37));
    tt->removeTab(tmp);
    tt->show();
}
void MainWindow::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event)
    updateTabWidth();
}
void MainWindow::delPage(int index)
{
    win[index]->setParent(NULL);
    winw[index]->setParent(NULL);
    tt->setTabEnabled(index, false);
    //delete  winw[index];
    tt->setTabEnabled(index,false);
    tt->removeTab(index);
    win.removeAt(index);
    winw.removeAt(index);
    //for(int i=index;i<=maxpage;i++)
    //{
    //    win[index]=win[index+i];
    //    winw[index]=winw[index+i];
    //}
    //ui->tabWidget->removeTab(index);
    maxpage=maxpage-1;
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
    win<<QWindow::fromWinId(wid);
    winw<<QWidget::createWindowContainer(win.at(index),NULL,Qt::FramelessWindowHint);;

    //win[index] = QWindow::fromWinId(wid);
    //winw[index]= QWidget::createWindowContainer(win[index],NULL,Qt::FramelessWindowHint);;
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
        qDebug()<<win[maxpage];
        delPage(maxpage);
    }
}
void MainWindow::closeEvent(QCloseEvent *e)
{
    MainWindow::closeALL();
    //e->ignore();
    QApplication* app;
    app->exit(0);
 }
