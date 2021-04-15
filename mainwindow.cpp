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

    connect(tt,&DTabBar::currentChanged,[=](int index){
        ui->tabWidget->setCurrentIndex(index);
        //这里延时50ms是因为切换页面和截图取色有时间差
        MainWindow::Sleep(50);
        MainWindow::setTitlebarColor();
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
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,[=](){
        //tt->setCurrentIndex(0);
    });

}
void MainWindow::setTitlebarColor()
{

   // qDebug()<<widlist.at(index);
    QScreen *screen = QGuiApplication::primaryScreen();
    QColor tcolor;
    tcolor=screen->grabWindow(this->winId()).toImage().pixel(10,60);

    QPalette palette = titlebar()->palette();
    palette.setColor(QPalette::Base,tcolor);

        //判断取到的颜色是深色还是浅色，保证titlebar上的控件始终保持可见
        if(tcolor.red()*0.299 + tcolor.green()*0.578 + tcolor.blue()*0.114 >= 192){ //浅色
            palette.setColor(QPalette::ButtonText,QColor("#414D68"));
            tt->setStyleSheet("color:#414D68;DTabBar::tab:disabled{width:0;color:transparent;}");
        }else{  //深色
            palette.setColor(QPalette::ButtonText,QColor("#FFFFFF"));
            tt->setStyleSheet("color:#5b657c;DTabBar::tab:disabled{width:0;color:transparent;}");
        }
        titlebar()->setPalette(palette);
        tt->setPalette(palette);
}

void MainWindow::Sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//快捷键这部分有点问题，后面再修复
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
    QDesktopWidget *screenResolution = QApplication::desktop();
    int center_y = screenResolution->height()/2 - wpos.at(index).y()/2;
    int center_x = screenResolution->width()/2 - wpos.at(index).x()/2;
    QScreen *screen = qApp->primaryScreen();
    int dpiVal = screen->logicalDotsPerInch();
    qDebug()<<dpiVal;
    float i;
    switch (dpiVal) {
        case 96:
            i=1;
            break;
        case 120:
            i=1.25;
            break;
        case 144:
            i=1.5;
            break;
        case 192:
            i=2;
            break;
    }
    win[index]->setGeometry(center_y,center_x,wpos.at(index).x()/i,wpos.at(index).y()/i);
    win[index]->setParent(NULL);
    winw[index]->setParent(NULL);
    win[index]->show();
    widlist.removeAt(index);
    win.removeAt(index);
    winw.removeAt(index);
    wpos.removeAt(index);
    tt->setTabEnabled(index, false);
    tt->removeTab(index);
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
    int b=output.indexOf("Width: ");
    int c=output.indexOf(" Height: ");
    int d=output.indexOf(" Depth: ");
    winid=output.mid(i,9);
    winname=output.mid(i+11,a-i+9);
    QString w=output.mid(b+7,c-b-7);
    QString h=output.mid(c+9,d-c-9);
    //目前是按照窗口名来做黑名单，由于dde桌面的窗口名是中文的“桌面”，所以导致在系统语言为英文的情况下对桌面的黑名单失效
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
    WId wid=WId(winid.toInt(NULL,16));
    widlist<<wid;

    win<<QWindow::fromWinId(wid);
    wpos<<QPoint(w.toInt(),h.toInt());
    winw<<QWidget::createWindowContainer(win.at(index),NULL,Qt::FramelessWindowHint);

    ui->tabWidget->insertTab(index,winw[index],winname);
    ui->tabWidget->setCurrentIndex(index);
    tt->setCurrentIndex(index);
    Sleep(50);
    MainWindow::setTitlebarColor();
    maxpage=maxpage+1;
    return ;
}

void MainWindow::closeALL()
{
    while (maxpage>=0) {
        //qDebug()<<win[maxpage];
        delPage(maxpage);
    }
}
//退出程序时自动脱离所有窗口
void MainWindow::closeEvent(QCloseEvent *e)
{
    MainWindow::closeALL();
    //e->ignore();
    QApplication* app;
    app->exit(0);
 }
