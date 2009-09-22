// ScriptingCoreTest.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include "any.h"

using namespace std;
using namespace cdiggins;

int main()
{
    any a = 23.23;

    cout << "Using: " << a.cast<double>() << endl;
    cout << "Using: " << a.convert_cast<int>() << endl;

    a = "This is a test";

    cout << "String: " << a.cast<std::string>().c_str() << endl;
    a = true;
    cout << "a is: " << a.convert_cast<std::string>().c_str() << endl;
    a = 47;
    cout << "a is: " << a.convert_cast<std::string>().c_str() << endl;
    a = 23.554;
    cout << "a is: " << a.convert_cast<std::string>().c_str() << endl;

    a = "37.23";
    cout << "a as a double is: " << a.convert_cast<double>() << endl;
    cout << "a as an int is: " << a.convert_cast<int>() << endl;
    cout << "a as a long is: " << a.convert_cast<long>() << endl;
    cout << "a as a short is: " << a.convert_cast<short>() << endl;
    cout << "a as a bool is: " << a.convert_cast<bool>() << endl;
    system("pause");
    return 0;
}
