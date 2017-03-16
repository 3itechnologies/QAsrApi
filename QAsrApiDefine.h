#ifndef QASRAPIDEFINE_H
#define QASRAPIDEFINE_H

#include <QString>
#include <QList>

namespace  asr_api
{
// const for speech api
const QString _str_lang = "language";
const QString _str_lang_rus = "ru";
const QString _str_lang_eng = "en";

const QString _str_sample_rate = "sample_rate";
const int _sample_rate_16k = 16000;
const int _sample_rate_8k = 8000;

const QString _str_priority = "priority";
const QString _str_prior_high = "high";
const QString _str_prior_normal = "normal";
const QString _str_prior_low = "low";

const QString _str_audio = "audio";
const QString _str_content = "content";

const QString _str_created_date = "created_date";

const QString _str_file_id = "file_id";

const QString _str_file_url = "file_url";

const QString _str_id = "id";

const QString _str_model_id = "model_id";

const QString _str_status = "status";
const QString _str_status_ready = "ready";
const QString _str_status_wait = "wait";

const QString _str_type = "type";
const QString _str_asr = "asr";

const QString _str_result = "result";
const QString _str_output_format = "output_format";
const QString _str_timestamps = "timestamps";
const QString _str_text = "text";

const QString _str_error = "error";
const QString  _str_error_process_file = "error_process_file";
const QString _str_details = "details";
const QString _str_http_status = "http_status";

const QString _str_api_key = "api_key";
const QString _str_url_task = "/tasks";
const QString _str_url_models = "/models";

const QString _str_models = "models";
const QString _str_topic = "topic";

const QString _str_end_time = "end_time";
const QString _str_start_time = "start_time";
const QString _str_word = "word";

const QString _str_total = "total";
const QString _str_task = "tasks";
const QString _str_offset = "offset";
const QString _str_limit = "limit";

//error format
struct asr_error
{
  QString error;
  QString details;
  int http_status;
};

//struct with information about the task
struct asr_task
{
  QString created_date;
  QString file_id;
  QString file_url;
  QString id;
  QString model_id;
  QString priority;
  int sample_rate;
  QString status;
  QString type;
  QString result;
};

//struct detailed information on available models for speech recognition
struct asr_model
{
  QString language;
  QString model_id;
  QString topic;
  QString type;
  int sample_rate;
};

//format processing result
struct TWordItem
{
  QString word;
  float startPos  = 0.f; // sec
  float endPos    = 0.f; // sec
};

typedef QList<TWordItem> TWords;

}
#endif // QASRAPIDEFINE_H
