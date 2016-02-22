/* @(#)anapacheapp.hpp
 */

#pragma once

#include <string>
#include <map>
#include <ctime>
#include <functional>

class AnapacheApp
{
 public:
  struct AnalizerOptions
  {
    int id;
  };

  AnapacheApp(std::string exec);
  virtual ~AnapacheApp();
  void readLog(std::string file);

  void setFromDate(std::string from);
  void setToDate(std::string to);

  void summary();
 protected:
  std::string logFormat;
  time_t fromDate;
  time_t toDate;
  struct
  {
    unsigned long total_queries;
    bool started;
    time_t firstone;
    time_t lastone;
    time_t currentAnalysis;
    unsigned long currentQueryStrike;
    time_t maxQueryStrike;
    unsigned long maxQueryStrikeHits;
  } totalQuery_data;

  struct
  {
    std::map<std::string, unsigned long> queryCount;
  } ipCount_data;

  std::string fixLogFormat(std::string lf);
  int parseLine(std::string line, std::map<std::string, std::string> &res);
  std::string readLogData(std::string::iterator& it, bool quotes, std::string::iterator strEnd);
  time_t parseTime(std::string strTime);
  void panic(std::string err);

  void totalQuery_Initialize(AnalizerOptions opts);
  void totalQuery_Input(time_t moment, std::map<std::string, std::string>& parsed, AnalizerOptions opts);
  void totalQuery_Results(AnalizerOptions opts);

  void ipCount_Initialize(AnalizerOptions opts);
  void ipCount_Input(time_t moment, std::map<std::string, std::string>& parsed, AnalizerOptions opts);
  void ipCount_Results(AnalizerOptions opts);
};
