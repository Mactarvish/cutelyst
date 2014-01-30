#include "users.h"

#include <QDebug>

Users::Users()
{

}

void Users::list(Context *ctx, const QString &name, const QString &age, Controller::Args, Controller::Local)
{
    qDebug() << Q_FUNC_INFO << name << age;
//    qDebug() << "Cookies" << c->req()->cookies();

    QNetworkCookie cookie;
    cookie.setName("bar");
    cookie.setValue("lolololo");
    cookie.setPath("/");
    ctx->response()->addCookie(cookie);
}

void Users::Begin(Context *ctx)
{
    qDebug() << "*** Users::Begin()";
}

bool Users::Auto(Context *ctx)
{
    qDebug() << "*** Users::Auto()";
    return true;
}

void Users::index(Context *ctx, Controller::Path, Controller::Args)
{

}

void Users::home(Context *ctx, Controller::Args, Controller::Path)
{

}

//void Users::End(Context *ctx)
//{
//    qDebug() << "*** Users::End()";
//}
