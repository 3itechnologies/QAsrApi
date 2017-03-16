#ifndef QASRAPI_H
#define QASRAPI_H

#include <QMap>

#include "QAsrApiDefine.h"
#include "QNetRequestSender.h"

namespace  asr_api
{
class QAsrApi
{
public:
  QAsrApi(QString str_host, QString asr_api_key);

  //sending a file for recognition
  //path_to_file - path to file in the file system
  //name_model - selected model for recognition
  //priority - priority of file recognition
  bool task_send(QString path_to_file, QString name_model, QString priority);

  //getting the result from the server
  bool task_get_result(TWords& result);

  //deleting data from the server
  bool task_delete(QString task_id);

  //getting a list of tasks/files stored on the server
  QList<asr_task> get_task_list(bool whith_result,
                                QString output_format,
                                int offset,
                                int limit,
                                QString status);

  //getting the number of the last sent task
  QString get_task_id();

  //getting a list with possible models
  QStringList get_model_id_list();

  //getting the description of the model
  asr_model get_model_details(QString id_model);

  //getting the last error
  asr_error get_error();

private:
  //format processing result
  asr_task _task_timestamps;

  //error format
  asr_error _error;

  //Detailed information on available models for speech recognition
  QMap<QString, asr_model> _details_model;

  QNetRequestSender _sender;
  QString _address;
  QString _asr_api_key;
};
}
#endif // QASRAPI_H
