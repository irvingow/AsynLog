//
// Created by 刘文景 on 2020-03-26.
//

#include <iostream>
#include "logStream.h"
#include <climits>

using namespace AsynLog;
using std::cout;
using std::endl;

int main(int argc, char **argv) {
  LogStream logStream;
  logStream << "hello world";
  cout << logStream.getAppendBuffer().peekBuffer() << endl;
  logStream << -1;
  cout << logStream.getAppendBuffer().peekBuffer() << endl;
  logStream << UINT_MAX;
  cout << logStream.getAppendBuffer().peekBuffer() << endl;
  logStream << LONG_MAX;
  cout << logStream.getAppendBuffer().peekBuffer() << endl;
  logStream << ULONG_LONG_MAX;
  cout << logStream.getAppendBuffer().peekBuffer() << endl;
  int *a = new int(4);
  logStream << a;
  cout << logStream.getAppendBuffer().peekBuffer() << endl;
  cout << "a:" << a << endl;
  delete (a);
  double b = 0.1234579;
  logStream.resetAppendBuffer();
  logStream << b;
  cout << logStream.getAppendBuffer().peekBuffer() << endl;
  return 0;
}