#ifndef QNETREQUEST_H
#define QNETREQUEST_H

#include <QString>
#include <QVariant>
#include <QString>

#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

class QNetRequest
{
public:
  QNetRequest(QString api_key, bool post_flag, QString address = QString());
  QString address() const;
  void setAddress(QString address);

  void addParam(QString name, QVariant value);
  bool removeParam(QString name);

  QStringList paramsNames() const;
  QMap<QString, QString> params() const;

  QUrl url(bool withParams = true) const;
  QNetworkRequest request(bool withParams) const;
  QByteArray data() const;
  void set_data(QByteArray json_data);
private:
  QString _address;
  QMap<QString, QString> _params;
  QString _req_api_key;
  bool _post_flag;
  QByteArray _byte_array_data;
};

struct QAnswerError
{
  QString error;
  QString details;
  QString http_status;
};

#endif // QNETREQUEST_H
