#include "QNetRequest.h"


QNetRequest::QNetRequest(QString api_key, bool post_flag, QString address)
{
  setAddress(address);
  _req_api_key = api_key;
  _post_flag = post_flag;
}

QString QNetRequest::address() const
{
  return _address;
}

void QNetRequest::setAddress(QString address)
{
  for (QPair<QString, QString> value : QUrlQuery(QUrl(address)).queryItems())
              addParam(value.first, value.second);
          _address = address;
}

void QNetRequest::addParam(QString name, QVariant value)
{
  _params[name] = value.toString();
}

bool QNetRequest::removeParam(QString name)
{
  if (false == _params.contains(name))
              return false;
          _params.remove(name);
          return true;
}

QStringList QNetRequest::paramsNames() const
{
  return _params.keys();
}

QMap<QString, QString> QNetRequest::params() const
{
  return _params;
}

QUrl QNetRequest::url(bool forGetRequest /*= true*/) const
{
    QUrl url(address());
    if (forGetRequest)
        url.setQuery(data());
    return url;
}


QNetworkRequest QNetRequest::request(bool withParams) const
{
  QNetworkRequest r;
  QSslConfiguration config = QSslConfiguration::defaultConfiguration();

  r.setSslConfiguration(config);
  r.setUrl(url(withParams));

  qDebug() << "fullUrl: " << r.url().toString();

  r.setRawHeader("content-type", "application/json");
  r.setRawHeader("api_key", _req_api_key.toUtf8());

  return r;
}

QByteArray QNetRequest::data() const
{
  QByteArray byteArrayData;

  if(_post_flag)
  {
    byteArrayData = _byte_array_data;
  }
  else
  {
    auto b = _params.begin();
    auto e = _params.end();

    while (b != e)
    {
        byteArrayData.append(b.key());
        byteArrayData.append('=');
        byteArrayData.append(b.value());
        byteArrayData.append('&');

        b++;
    }
    byteArrayData.chop(1);
  }
  return byteArrayData;
}

void QNetRequest::set_data(QByteArray json_data)
{
  _byte_array_data = json_data;
}
