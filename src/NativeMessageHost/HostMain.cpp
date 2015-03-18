/**********************************************************\
Original Author: Richard Bateman

Created:    Mar 17, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 GradeCam, Richard Bateman, and the
               Firebreath development team
\**********************************************************/

#include "MainLoop.h"
#include "ReadLoop.h"

int main(int argc, char* argv[]) {

  if (argv[1] != NULL) {
    std::string output;
    unsigned int len = 0;

    std::cin.read((char*) &len, sizeof(len));

    std::cout.sync_with_stdio(false);
    std::cin.sync_with_stdio(false);

    char str[len];

    size_t res = fread(str, sizeof(char), len, stdin);

    std::cerr << "received message:"
              << std::endl
              << str
              << std::endl;

    if (res) { // We are going to pretend that any data without the string "echo"
               // is a request for the version
      apitest apitest;
      if (strstr (str,"echo"))
        output = apitest.echo(str);
      else
        output = apitest.get_version();
    }

    writeOut(output);
  }
  return 0;
}