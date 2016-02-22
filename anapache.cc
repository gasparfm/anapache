#include <iostream>
#include <cstdlib>
#include "anapacheapp.hpp"
#include "anaexception.hpp"
#include <getopt.h>
#include <cctype>
#include <cstring>

using namespace std;

void help(char *programa)
{
  cout << "anapache help!!!" << endl;
  std::exit(2);
}

int main(int argc, char* argv[])
{
  AnapacheApp anapache(argv[0]);

  /* anapache.readLog("/var/log/apache2/access.log"); */
  /* anapache.readLog("alog.log"); */
  int verbose = 0;      /* flag */
  char *user=NULL, *pass=NULL, *host=NULL;
  int port = 0;
  int i;
  int l;
  int c;
  int oindex;
  char output[10]="none";
  /* getopt vars */
  extern char* optarg;
  extern int optind;
  extern int optopt;
  extern int opterr;
  opterr = 0;
  struct option cli_options[] = {
    {"verbose", 0, NULL, 'v'},
    {"from", 1, NULL, 'f'},
    {"to", 1, NULL, 't'},
    {"all", 0, NULL, 'a'},
    {"help", 0, NULL, 'h'},
    {NULL, 0, NULL, 0}
  };

  try
    {
      while ((c = getopt_long (argc, argv, "vahf:t:", cli_options, &oindex)) != -1)
	{
	  //      printf ("c = %d(%c) index = %d\n", c, c, oindex);
	  switch (c)
	    {
	    case 0:
	      cerr << "Wrong argument!!"<<endl;
	      break;
	    case 'v':
	      verbose=1;
	      break;
	    case 'f':
	      anapache.setFromDate(optarg);
	      break;
	    case 't':
	      anapache.setToDate(optarg);
	      break;
	    case 'a':
	      cout << "ALL ANALYSIS, IT'S THE ONLY WAY NOW\n";
	      break;
	    case 'h':
	      help(argv[0]);
	      break;
	    case '?':
	      if (((string)"ft").find(optopt) != string::npos)
		cerr << "Option -"<<(char)optopt<<" requires an argument.\n";
	      else if (optopt=='?')
		help(argv[0]);
	      else if (isprint (optopt))
		cerr << "Unknown option '-"<<(char)optopt<<"'.\n";
	      else
		cerr << "Invalid char '"<<(char)optopt<<"'.\n";

	      help(argv[0]);
	    default:
	      std::exit(1);
	    }
	}
    } 
  catch (AnaException &e)
    {
      cerr << "An error ocurred: "<< e.what() << std::endl;
      exit(4);
    }

  anapache.summary();

  /* Several files in the future */
  /* for (i = optind; i < argc; i++) */
  /*   anapache.readLog(argv[i]); */

  if (optind < argc)
    anapache.readLog(argv[optind]);
  else
    cerr << "No log files specified" << std::endl;
  return 0;
}

