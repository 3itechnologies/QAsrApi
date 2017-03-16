#include "QAsrApi.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

asr_api::QAsrApi::QAsrApi(QString str_host, QString asr_api_key)
{
  _address = str_host;
  _asr_api_key = asr_api_key;
}

bool asr_api::QAsrApi::task_send(QString path_to_file, QString name_model, QString priority)
{  
  QFile audio_file(path_to_file);
  if (!audio_file.open(QIODevice::ReadOnly))
  {
    qDebug() << "File not open. Path: " << path_to_file;
    return false;
  }
  QByteArray audio_data = audio_file.readAll();
  QString audio_base64 = audio_data.toBase64();

  QJsonObject json_contest
  {
    {_str_content, audio_base64}
  };

  QJsonObject json_obj
  {
    {_str_audio, json_contest},
    {_str_model_id, name_model},
    {_str_priority, priority}
  };
  
  qint64 maxWaitTime = 10000;
  QString task_address = _address + _str_url_task;
  QNetRequest request(_asr_api_key, true, task_address);
  request.set_data(QJsonDocument(json_obj).toJson());

  QNetRequestSender sender(maxWaitTime);
  QJsonDocument doc = QJsonDocument::fromJson(sender.post(request));

  QJsonObject req_json_obj = doc.object();
  if(doc.isEmpty())
  {
    _error.details = QString("failed to create json");
    _error.error = QString("bad response");
    _error.http_status = sender.error();
    return false;
  }

  if(sender.error() != QNetRequestSender::NoError)
  {
    _error.error = req_json_obj[_str_error].toString();
    _error.details = req_json_obj[_str_details].toString();
    _error.http_status = req_json_obj[_str_http_status].toInt();
    return false;
  }

  _task_timestamps.created_date = req_json_obj[_str_created_date].toString();
  _task_timestamps.file_id = req_json_obj[_str_file_id].toString();
  _task_timestamps.file_url = req_json_obj[_str_file_url].toString();
  _task_timestamps.id = req_json_obj[_str_id].toString();
  _task_timestamps.model_id = req_json_obj[_str_model_id].toString();
  _task_timestamps.priority = req_json_obj[_str_priority].toString();
  _task_timestamps.sample_rate = req_json_obj[_str_sample_rate].toInt();
  _task_timestamps.status = req_json_obj[_str_status].toString();
  _task_timestamps.type = req_json_obj[_str_type].toString();

  return true;
}

bool asr_api::QAsrApi::task_get_result(TWords& result)
{
  QString addr_with_id = _address + _str_url_task + "/" + _task_timestamps.id;
  QNetRequest request(_asr_api_key, false, addr_with_id);

  request.addParam(_str_output_format, _str_timestamps);
  request.addParam(_str_result, true);

  qint64 maxWaitTime = 10000;
  QNetRequestSender sender(maxWaitTime);

  QJsonDocument doc = QJsonDocument::fromJson(sender.get(request));
  QJsonObject req_json_obj = doc.object();

  if(doc.isEmpty())
  {
    _error.details = QString("failed to create json");
    _error.error = QString("bad response");
    _error.http_status = sender.error();
    return false;
  }

  if(sender.error() != QNetRequestSender::NoError)
  {
    _error.error = req_json_obj[_str_error].toString();
    _error.details = req_json_obj[_str_details].toString();
    _error.http_status = req_json_obj[_str_http_status].toInt();
    return false;
  }

  _task_timestamps.created_date = req_json_obj[_str_created_date].toString();
  _task_timestamps.file_id = req_json_obj[_str_file_id].toString();
  _task_timestamps.file_url = req_json_obj[_str_file_url].toString();
  _task_timestamps.id = req_json_obj[_str_id].toString();
  _task_timestamps.model_id = req_json_obj[_str_model_id].toString();
  _task_timestamps.priority = req_json_obj[_str_priority].toString();
  _task_timestamps.sample_rate = req_json_obj[_str_sample_rate].toInt();
  _task_timestamps.status = req_json_obj[_str_status].toString();
  _task_timestamps.type = req_json_obj[_str_type].toString();

  if(_task_timestamps.status == _str_status_wait)
  {
    _error.details = QString("waiting for the audio file processing");
    _error.error = QString("wait");
    _error.http_status = sender.error();
    return false;
  }

  if(_task_timestamps.status == _str_error)
  {
    _error.details = QString("error when processing the file");
    _error.error = QString("error_process_file");
    _error.http_status = sender.error();
    return false;
  }

  QJsonArray resultArray = req_json_obj[_str_result].toArray();
  foreach (const QJsonValue & value, resultArray)
  {
    QJsonObject obj = value.toObject();

    TWordItem cur_word_item;

    cur_word_item.word = obj[_str_word].toString();
    cur_word_item.startPos = static_cast<float>(obj[_str_start_time].toDouble());
    cur_word_item.endPos = static_cast<float>(obj[_str_end_time].toDouble());

    result.push_back(cur_word_item);
  }

  return true;
}

bool asr_api::QAsrApi::task_delete(QString task_id)
{
  QString addr_with_id = _address + _str_url_task + "/" + task_id;
  QNetRequest request(_asr_api_key, false, addr_with_id);

  qint64 maxWaitTime = 10000;
  QNetRequestSender sender(maxWaitTime);

  QJsonDocument doc = QJsonDocument::fromJson(sender.http_delete(request));
  QJsonObject req_json_obj = doc.object();

  if(sender.error() != QNetRequestSender::NoError)
  {
    _error.details = QString("Server could not complete this operation");
    _error.error = QString("ID not found or invalid");
    _error.http_status = 404;
    return false;
  }
  return true;
}

QList<asr_api::asr_task> asr_api::QAsrApi::get_task_list(bool whith_result,
                                                         QString output_format,
                                                         int offset,
                                                         int limit,
                                                         QString status)
{
  QString addr_with_id = _address + _str_url_task;
  QNetRequest request(_asr_api_key, false, addr_with_id);

  request.addParam(_str_result, whith_result);
  request.addParam(_str_output_format, output_format);
  request.addParam(_str_offset, offset);
  request.addParam(_str_limit, limit);
  request.addParam(_str_status, status);

  qint64 maxWaitTime = 10000;
  QNetRequestSender sender(maxWaitTime);

  QJsonDocument doc = QJsonDocument::fromJson(sender.get(request));
  QJsonObject req_json_obj = doc.object();

  if(doc.isEmpty())
  {
    _error.details = QString("failed to create json");
    _error.error = QString("bad response");
    _error.http_status = sender.error();
    return QList<asr_api::asr_task>();
  }

  if(sender.error() != QNetRequestSender::NoError)
  {
    _error.error = req_json_obj[_str_error].toString();
    _error.details = req_json_obj[_str_details].toString();
    _error.http_status = req_json_obj[_str_http_status].toInt();
    return QList<asr_api::asr_task>();
  }

  QList<asr_api::asr_task> result_list;
  QJsonArray resultArray = req_json_obj[_str_task].toArray();
  foreach (const QJsonValue & value, resultArray)
  {
    QJsonObject obj = value.toObject();
    asr_api::asr_task cur_task;
    cur_task.created_date = obj[_str_created_date].toString();
    cur_task.file_id = obj[_str_file_id].toString();
    cur_task.file_url = obj[_str_file_url].toString();
    cur_task.id = obj[_str_id].toString();
    cur_task.model_id = obj[_str_model_id].toString();
    cur_task.priority = obj[_str_priority].toString();
    cur_task.sample_rate = obj[_str_sample_rate].toInt();
    cur_task.status = obj[_str_status].toString();
    cur_task.type = obj[_str_type].toString();

    result_list.push_back(cur_task);
  }
  return result_list;
}

QString asr_api::QAsrApi::get_task_id()
{
  return  _task_timestamps.id;
}

QStringList asr_api::QAsrApi::get_model_id_list()
{
  QString models_address = _address + _str_url_models;
  QNetRequest request(_asr_api_key, false, models_address);

  qint64 maxWaitTime = 10000;
  QNetRequestSender sender(maxWaitTime);

  QJsonDocument doc = QJsonDocument::fromJson(sender.get(request));
  QJsonObject req_json_obj = doc.object();

  if(doc.isEmpty())
  {
    _error.details = QString("failed to create json");
    _error.error = QString("bad response");
    _error.http_status = sender.error();
    return QStringList();
  }

  if(sender.error() != QNetRequestSender::NoError)
  {
    _error.error = req_json_obj[_str_error].toString();
    _error.details = req_json_obj[_str_details].toString();
    _error.http_status = req_json_obj[_str_http_status].toInt();
    return QStringList();
  }

  QJsonArray resultArray = req_json_obj[_str_models].toArray();

  foreach (const QJsonValue & value, resultArray)
  {
    QJsonObject obj = value.toObject();

    asr_model cur_model;

    cur_model.language = obj[_str_lang].toString();
    cur_model.model_id = obj[_str_model_id].toString();
    cur_model.sample_rate = obj[_str_sample_rate].toInt();
    cur_model.topic = obj[_str_topic].toString();
    cur_model.type = obj[_str_type].toString();

    _details_model.insert(cur_model.model_id, cur_model);
  }

  QStringList list_models = _details_model.keys();
  return list_models;
}

asr_api::asr_model asr_api::QAsrApi::get_model_details(QString name_model)
{
  return _details_model.take(name_model);
}

asr_api::asr_error asr_api::QAsrApi::get_error()
{
  return _error;
}
