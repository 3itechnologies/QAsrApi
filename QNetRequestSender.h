#ifndef QNETREQUESTSENDER_H
#define QNETREQUESTSENDER_H

#include "QNetRequest.h"

#include <QObject>
#include <QNetworkProxy>

class QNetRequestSender : public QObject
{
  Q_OBJECT
public:
  explicit QNetRequestSender(qint64 maxWaitTime = 35000, QObject *parent = 0);

  enum RequestError { NoError, TimeoutError};

  ~QNetRequestSender();

  void setProxy(const QNetworkProxy& proxy);

  QByteArray get(QNetRequest& request);
  QByteArray post(QNetRequest& request);
  QByteArray http_delete(QNetRequest& request);
  QByteArray getWhileSuccess(QNetRequest& request, int maxCount = 2);
  QByteArray postWhileSuccess(QNetRequest& request, int maxCount = 2);

  void setMaxWaitTime(qint64 max);

  qint64 maxWaitTime() const;
  RequestError error() const;

private:
  QByteArray sendRequest(QNetRequest& request, bool getRequest = true);
  QByteArray sendDeleteRequest(QNetRequest& request);
  QByteArray sendWhileSuccess(QNetRequest& request, int maxCount = 2, bool getRequest = true);

private:
  qint64 _maxWaitTime;
  RequestError _error;
  QNetworkProxy _proxy;
};

#endif // QNETREQUESTSENDER_H
