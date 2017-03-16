#include "QNetRequestSender.h"

#include <QTimer>
#include <QEventLoop>
#include <QNetworkReply>
#include <QThread>
#include <QLoggingCategory>
#include <iostream>

QNetRequestSender::QNetRequestSender(qint64 maxWaitTime, QObject *parent)  : QObject(parent)
{
  setMaxWaitTime(maxWaitTime);
  _error = NoError;
}

QNetRequestSender::~QNetRequestSender()
{

}

void QNetRequestSender::setProxy(const QNetworkProxy& proxy)
{
  _proxy = proxy;
}

QByteArray QNetRequestSender::get(QNetRequest& request)
{
  return sendRequest(request, true);
}

QByteArray QNetRequestSender::post(QNetRequest& request)
{
  return sendRequest(request, false);
}

QByteArray QNetRequestSender::http_delete(QNetRequest& request)
{
  return sendDeleteRequest(request);
}

QByteArray QNetRequestSender::getWhileSuccess(QNetRequest& request, int maxCount /*= 2*/)
{
  return sendWhileSuccess(request, maxCount, true);
}

QByteArray QNetRequestSender::postWhileSuccess(QNetRequest& request, int maxCount /*= 2*/)
{
  return sendWhileSuccess(request, maxCount, false);
}

void QNetRequestSender::setMaxWaitTime(qint64 max)
{
  _maxWaitTime = max;
}

qint64 QNetRequestSender::maxWaitTime() const
{
  return _maxWaitTime;
}

QNetRequestSender::RequestError QNetRequestSender::error() const
{
  return _error;
}

QByteArray QNetRequestSender::sendRequest(QNetRequest& request, bool getRequest /*= true*/)
{
  QTimer timer;
  timer.setInterval(_maxWaitTime);
  timer.setSingleShot(true);

  QEventLoop loop;
  QSharedPointer<QNetworkAccessManager> manager(new QNetworkAccessManager);
  manager->setProxy(_proxy);

  QNetworkReply* reply = getRequest ? manager->get(request.request(true)) :
                                      manager->post(request.request(false), request.data());

  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);

  timer.start();
  loop.exec();

  QByteArray data;

  if (reply->isFinished() && reply->error() == QNetworkReply::NoError)
  {
    data = reply->readAll();
    _error = QNetRequestSender::NoError;
  }
  else
  {
    _error = QNetRequestSender::TimeoutError;
  }

  reply->deleteLater();

  return data;
}

QByteArray QNetRequestSender::sendDeleteRequest(QNetRequest &request)
{
  QTimer timer;
  timer.setInterval(_maxWaitTime);
  timer.setSingleShot(true);

  QEventLoop loop;
  QSharedPointer<QNetworkAccessManager> manager(new QNetworkAccessManager);
  manager->setProxy(_proxy);

  QNetworkReply* reply = manager->deleteResource(request.request(true));

  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);

  timer.start();
  loop.exec();

  QByteArray data;

  if (reply->isFinished() && reply->error() == QNetworkReply::NoError)
  {
    data = reply->readAll();
    _error = QNetRequestSender::NoError;
  }
  else
  {
    _error = QNetRequestSender::TimeoutError;
  }
  reply->deleteLater();

  return data;
}

QByteArray QNetRequestSender::sendWhileSuccess(QNetRequest& request, int maxCount /*= 2*/, bool getRequest /*= true*/)
{
  if (maxCount < 0)
    throw QString(__LINE__ + " " __FILE__);

  int c = 0;
  QByteArray answer;

  while (c < maxCount)
  {
    c++;
    answer = getRequest ? get(request) : post(request);

    if (error() == NoError)
      break;

    qDebug() << "Ошибка при отправке запроса. Код ошибки - " << error() << ". Повторная отправка запроса через 2 секунды\n";
    QThread::currentThread()->msleep(2000);
  }

  return answer;
}
