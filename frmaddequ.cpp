#include "frmaddequ.h"
#include "ui_frmaddequ.h"
#include "iconhelper.h"
#include "myhelper.h"
frmAddEqu::frmAddEqu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmAddEqu)
{
    ui->setupUi(this);

    InitStyle();
    connect(ui->btnAdd,SIGNAL(clicked(bool)),this,SLOT(btnAddEqument()));
    connect(ui->btnCancel,SIGNAL(clicked(bool)),this,SLOT(btnCancel()));
}

frmAddEqu::~frmAddEqu()
{
    delete ui;
}
void frmAddEqu::InitStyle()
{
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    //IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    //IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);

}

bool frmAddEqu::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void frmAddEqu::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void frmAddEqu::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void frmAddEqu::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void frmAddEqu::on_btnMenu_Close_clicked()
{
    this->close();
}

void frmAddEqu::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
    }
    max = !max;
}

void frmAddEqu::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}
void frmAddEqu::btnAddEqument()
{
    emit AddEqument(ui->leIp->text());
    this->close();
}
void frmAddEqu::btnCancel()
{
    this->close();
}
