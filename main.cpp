#include <QCoreApplication>

#include "QAsrApi.h"
#include <QDebug>
#include <QString>
#include <QSslSocket>
#include <QThread>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false"); // no show warning about SSL

  QString path_to_file = "../wav/output.wav";
  QString host_api = "https://api.3i-analytics.ru/asr/v1";
  QString key = "1111-2222-3333-4444-5555";

  qDebug() << "host:          " << host_api;
  qDebug() << "key:           " << key;
  qDebug() << "path to file:  " << path_to_file;

  asr_api::QAsrApi asr_api(host_api, key);

  QStringList list_with_models = asr_api.get_model_id_list();
  if(list_with_models.isEmpty())
  {
    qDebug() << "List with models empty";
    return a.exec();
  }

  qDebug() << list_with_models;

  bool isSend = asr_api.task_send(  path_to_file,
                                    list_with_models[1],
                                    asr_api::_str_prior_high);
  if(!isSend)
  {
    auto send_error = asr_api.get_error();
    qDebug() << send_error.http_status;
    qDebug() << send_error.error;
    qDebug() << send_error.details;
    return a.exec();
  }

  asr_api::TWords _result_timestamps;

  while(!asr_api.task_get_result(_result_timestamps))
  {
    auto send_error = asr_api.get_error();
    qDebug() << send_error.http_status;
    qDebug() << send_error.error;
    qDebug() << send_error.details;
    if(send_error.error == asr_api::_str_error_process_file)
    {
      break;
    }
    QThread::sleep(4);
  }

  for(auto val: _result_timestamps)
  {
    qDebug() << val.word;
  }

  auto list_asr_tasks = asr_api.get_task_list(false,
                                              asr_api::_str_text,
                                              0,
                                              10,
                                              asr_api::_str_status_ready);

  for (asr_api::asr_task cur_task: list_asr_tasks)
  {
    qDebug() << cur_task.id;
  }

  qDebug() << "Id task: " << asr_api.get_task_id();
  qDebug() << "Delete task: " << asr_api.task_delete(asr_api.get_task_id());

  list_asr_tasks = asr_api.get_task_list(false,
                                         asr_api::_str_text,
                                         0,
                                         10,
                                         asr_api::_str_status_ready);

  for (asr_api::asr_task cur_task: list_asr_tasks)
  {
    qDebug() << cur_task.id;
  }

  return a.exec();
}
