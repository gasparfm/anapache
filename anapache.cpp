#include <iostream>
#include "anapacheapp.hpp"

int main()
{
  AnapacheApp anapache;

  anapache.readLog("/var/log/apache/access.log");
  return 0;
}

