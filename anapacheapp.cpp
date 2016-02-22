/**
*************************************************************
* @file anapache.cpp
* @brief
*
*
*
*
*
* @author Gaspar Fernández <blakeyed@totaki.com>
* @version
* @date 10 ene 2016
* Changelog
*
*
*
*
*
*
*
*************************************************************/

#include "anapacheapp.hpp"
#include "anaexception.hpp"
#include "lib/gcommon/gcommon.hpp"
#include "lib/ini/ini.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <ctime>
#include <vector>

AnapacheApp::AnapacheApp(std::string exec)
{
  std::string configFile;
  std::ifstream confFile;
  GCommon::GlobalSettings::basics("Anapache", "anapache", exec, "0.1");
  try
    {
      if (!GCommon::findConfigFile(configFile, "ini"))
	throw AnaException("No config file found");

      confFile.open(configFile);
      if (confFile.fail())
	throw AnaException("Error reading config file");

      INI::Parser config(confFile);
      std::stringstream out;
      confFile.close();
      logFormat = fixLogFormat(config.top()("Apache")["LogFormat"]);
    }
  catch (AnaException& e)
    {
      panic (e.what());
    }
  fromDate = 0;
  toDate = 0;
  /* std::cout << "LF: "<<logFormat<<std::endl; */
}

AnapacheApp::~AnapacheApp()
{
}

void AnapacheApp::panic(std :: string err)
{
  std:: cerr << "An error has ocurred: "<<err<<std::endl;
  std::exit(1);
}

std::string AnapacheApp::fixLogFormat(std :: string lf)
{
  if ( (lf.front()=='"') && (lf.back()=='"') )
    lf = lf.substr(1, lf.length()-2);

  lf = GCommon::replace(lf, "\\\"", "\"");
  return lf;
}

void AnapacheApp::readLog(std :: string file)
{
  std::ifstream ifs(file);
  std::string line;
  std::map < std::string, std::string> parsed;
  struct tm tm;
  AnalizerOptions opts;

  /* std::cout << "READING\n"; */
  if (ifs.fail())
    panic ("Couldn't open log file: "+file);

  std::getline(ifs, line);
  /* Initialize analysis */
  totalQuery_Initialize(opts);
  ipCount_Initialize(opts);
  /* Initialize analysis */
  while (!ifs.eof())
    {
      /* std::cout << line << std::endl; */
      parseLine(line, parsed);
      strptime(parsed["t"].c_str(), "%d/%b/%Y:%T", &tm);
      time_t linetime = mktime(&tm);
      /* std::cout << linetime << " ("<<GCommon::put_time("%d/%h/%Y %H:%M:%S", linetime)<<") : "<<fromDate<<"("<<GCommon::put_time("%d/%h/%Y %H:%M:%S", fromDate)<<"):"<<toDate<<"("<<GCommon::put_time("%d/%h/%Y %H:%M:%S", toDate)<<")"<<std::endl; */
      if ( (toDate) && (linetime>toDate) )
	break;
      if ( (!fromDate) || (linetime>=fromDate) )
	{
	  /* Run Analysis  */
	  /* We should have plugins and so, but now, run all
	     analysis rude mode. */
	  totalQuery_Input(linetime, parsed, opts);
	  ipCount_Input(linetime, parsed, opts);
	}
      /* std::cout << "FECHA2: "<<res<<std::endl; */
      /* for (auto k : parsed) */
      /* 	{ */
      /* 	  std::cout << "KW: "<<k.first<< " = "<<k.second<<std::endl; */
      /* 	} */
      std::getline(ifs, line);
    }

  std::cout << "END OF QUERY\n";

  /* Analyzer results */
  totalQuery_Results(opts);
  ipCount_Results(opts);
  /* Analyzer results */
  ifs.close();
}

int AnapacheApp::parseLine(std :: string line, std :: map < std :: string, std :: string > & res)
{
  std::string::iterator lineit = line.begin();
  int kk = 0;
  bool keywordFound = false;
  std::string kword;
  bool quotes = false;
  int state = 0;		/* current state:
				   0 - Just reading
				   -1 - Reading first keyword character (a % has been read before)
				   -2 - Read a { as first keyword char, so let's wait for }" 
				   -3 - Waiting for keyword last char 
				   2  - Two characters pending
				   1  - One character pending */
  res.clear();
  for (char f : logFormat)
    {
      switch (state)
	{
	case 0:
	  if (f == '%')
	    {
	      state = -1;
	      kword.clear();
	    }
	  else if (f=='"')
	    quotes = !quotes;
	  break;
	case -1:
	  kword+=f;
	  if (f == '{')
	    state = -2;
	  else if (f == '>')
	    state = 1;
	  else if (f == '%')
	    {
	      state = 0;
	      kword.clear();
	    }
	  else
	    keywordFound = true;
	  break;
	case -2:
	  kword+=f;
	  if (f == '}')
	    state=-3;
	  break;
	case -3:
	  kword+=f;
	  if (f == '^')
	    state = 2;
	  else
	    keywordFound = true;
	  break;
	default:
	  if (state<0)
	    panic ("You shouldn't be here");
	  kword+=f;
	  if (--state==0)
	    keywordFound = true;
	}

      if (keywordFound)
	{
	  keywordFound = false;
	  auto data = readLogData(lineit, quotes, line.end());
	  res.insert({kword, data});

	  state = 0;
	}
      else if (state==0)
	{
	  if (lineit == line.end())
	    throw AnaException("Unexpected end of line");
	  else if (*lineit != f)
	    throw AnaException ("Unexpected character");
	  /* std::cout << "CHAR: '"<<*lineit<<"' EXPECTED: '"<<f<<"'\n"; */
	  ++lineit;
	  /* Normal character */
	}
    }
}

std::string AnapacheApp::readLogData(std::string::iterator& it, bool quotes, std::string::iterator strEnd)
{
  std::string out = "";
  bool brackets = false;
  int k = 0;

  if (*it=='[')
    {
      brackets=true;
      it++;
    }
  else if ( (*it == '-') && (!quotes) )
    {
      /* Information not available - */
      it++;
      return "-";
    }
  do
    {
      if ( it == strEnd)
	throw AnaException("Unexpected end of line");
      if ( (*it==']') && (brackets) )
	{
	  ++it;
	  return out;
	}
      else if ( (*it=='"') && (quotes) )
	  return out;
      else if ( (*it==' ') && (!quotes) && (!brackets) )
	  return out;
      else
	out+=*it;
      ++it;
    } while (1);
  return out;
}

void AnapacheApp::setFromDate(std::string from)
{
  fromDate = parseTime(from);
  if (!fromDate)
    throw AnaException ("Wrong from date");
}

void AnapacheApp::setToDate(std::string to)
{
  toDate = parseTime(to);
  if (!toDate)
    throw AnaException ("Wrong to date");
}

time_t AnapacheApp::parseTime(std::string strTime)
{
  /* We must improve it a little bit */
  std::string format;
  struct tm tm;
  time_t now = time(NULL);
  localtime_r(&now, &tm);

  if (strTime == "now")
    return now;
  else if (strTime == "yesterday")
    return now-86400;

  if (strTime.find('/') == std::string::npos)
    format = "%H:%M:%S";
  else
    format ="%d/%m/%Y %H:%M:%S";

  char *conv = strptime(strTime.c_str(), format.c_str(), &tm);

  time_t newtime = mktime(&tm);
  if ( (conv == NULL) && (newtime == now) )
    return 0;

  return newtime;
}

void AnapacheApp::summary()
{
  if (fromDate)
    std::cout << "From: "<< GCommon::put_time("%d/%m/%Y %H:%M:%S", fromDate) << std::endl;
  if (toDate)
    std::cout << "To: "<< GCommon::put_time("%d/%m/%Y %H:%M:%S", toDate) << std::endl;
}


/* Analyzers */
using ParsedInput  = std::map<std::string, std::string>;

void AnapacheApp::totalQuery_Initialize(AnalizerOptions opts)
{
  totalQuery_data.total_queries = 0;
  totalQuery_data.started = false;
  totalQuery_data.currentAnalysis = 0;
  totalQuery_data.currentQueryStrike = 0;
  totalQuery_data.maxQueryStrike = 0;
  totalQuery_data.firstone = 0;
  totalQuery_data.lastone = 0;
  totalQuery_data.maxQueryStrikeHits = 0;
}

void AnapacheApp::totalQuery_Input(time_t moment, std::map<std::string, std::string>& parsed, AnalizerOptions opts)
{
  totalQuery_data.total_queries++;
  if (!totalQuery_data.started)
    totalQuery_data.firstone = moment;

  totalQuery_data.lastone = moment;
  totalQuery_data.started = true;
  if (totalQuery_data.currentAnalysis == moment)
    {
      totalQuery_data.currentQueryStrike++;
      if (totalQuery_data.currentQueryStrike > totalQuery_data.maxQueryStrikeHits)
	{
	  totalQuery_data.maxQueryStrikeHits = totalQuery_data.currentQueryStrike;
	  totalQuery_data.maxQueryStrike = moment;
	}
    }
  else
    {
      totalQuery_data.currentQueryStrike=1;
    }

  totalQuery_data.currentAnalysis = moment;
}

void AnapacheApp::totalQuery_Results(AnalizerOptions opts)
{
  if (!totalQuery_data.started)
    {
      std::cout << "No queries found!"<<std::endl;
    }
  std::cout << "Total queries: "<< totalQuery_data.total_queries<<std::endl;
  std::cout << "Total time: "<< difftime (totalQuery_data.lastone, totalQuery_data.firstone) << "seconds"<<std::endl;
  std::cout << "First query: "<<GCommon::put_time("%d/%m/%Y %H:%M:%S", totalQuery_data.firstone)<<std::endl;
  std::cout << "Last query: "<<GCommon::put_time("%d/%m/%Y %H:%M:%S", totalQuery_data.lastone)<<std::endl;
  if (totalQuery_data.firstone != totalQuery_data.lastone)
    {
      std::cout << "Queries/second: "<< ( totalQuery_data.total_queries / difftime (totalQuery_data.lastone, totalQuery_data.firstone) ) << std::endl;
    }
  if (totalQuery_data.maxQueryStrike)
    {
      std::cout << "Max. query strike: "<<GCommon::put_time("%d/%m/%Y %H:%M:%S", totalQuery_data.maxQueryStrike)<< "("<<totalQuery_data.maxQueryStrikeHits<<")"<<std::endl;
    }

}

void AnapacheApp::ipCount_Initialize(AnalizerOptions opts)
{
  ipCount_data.queryCount.clear();
}

void AnapacheApp::ipCount_Input(time_t moment, ParsedInput & parsed, AnalizerOptions opts)
{
  auto ipAddress = parsed.find("h");
  if (ipAddress == parsed.end())
    {
      std::cerr << "No IP found on request"<<std::endl;
      return;
    }

  auto fip = ipCount_data.queryCount.find(ipAddress->second);
  if (fip == ipCount_data.queryCount.end())
    ipCount_data.queryCount.insert({ipAddress->second, 1});
  else
    fip->second++;
}

void AnapacheApp::ipCount_Results(AnalizerOptions opts)
{
  std::cout << "Found "<<ipCount_data.queryCount.size()<<" different IPs"<<std::endl;
  std::cout << "Ordering IPs.."<<std::endl;
  std::vector < std::pair<std::string, unsigned long> >orderedIps;
  for (auto ip : ipCount_data.queryCount)
    {
      auto pos = orderedIps.begin();

      while ( (pos!=orderedIps.end()) && (pos->second<ip.second) )
      	pos++;
      orderedIps.insert(pos, ip);
      /* std:: cout << ip.first <<": "<<ip.second<<std::endl; */
    }
  std::cout << "ready!"<<std::endl;
  for (auto ip : orderedIps)
    {
      std:: cout << ip.first <<": "<<ip.second<<std::endl;
    }

}
