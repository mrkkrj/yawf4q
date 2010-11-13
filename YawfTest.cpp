//
// YawfTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

#include "CuteHttpServer.hpp"

#include <string>
#include <iostream>
#include <algorithm>
#include <qapplication.h>

using namespace std;

// -----------
#define TRACE_ERR(a) cout << "ERROR: " << a << endl
#define TRACE_ERR2(a, b) cout << "ERROR: " << a << b << endl
#define TRACE(a) cout << " " << a << endl
#define TRACE2(a, b) cout << " " << a << b << endl


struct NarrowChar
{
    // cout already installs a locale, use it!
    char operator() (_TCHAR c) { return cout.narrow(c, ' '); }
};


// --------------


int _tmain(int argc, _TCHAR* argv[])
{
    try
    {           
#   ifdef  _UNICODE
        // converse the arguments: 
        char** argvAscii = new char*[argc];

        for (int i = 0; i < argc; i++)
        {
            string arg(wcslen(argv[i]), ' ');
            transform(&(argv[i][0]), &(argv[i][0]) + wcslen(argv[i]), 
                        arg.begin(), NarrowChar());

            size_t iLen = arg.length();
            argvAscii[i] = new char[iLen+1];
            strncpy_s(argvAscii[i], iLen+1, arg.c_str(), iLen);
            argvAscii[i][iLen] = '\0';
        }
#   else 
        char* argvAscii[] = argv;
#   endif

        // our application:
        QApplication* qtAppl = new QApplication(argc, argvAscii);


        // our server
        ibkrj::yawf4q::CuteHttpServer testServer(3100);
        string error;

        if(!testServer.start(error))
            return -1;
      
        // start the Qt
        TRACE("fw: starting QApplication loop in the main thread context...");

        int dummy_argc = 1;
        char* dummy_argv[] = { "cuteSvrTest" };

        qtAppl->exec();

        // if here: exiting
        TRACE("fw: QApplication finished, main thread exiting");
        return 0;

    }
    catch (...)
    {
        TRACE_ERR("generic exception caught, exiting!!!!");
    }

    // done
    return 0;
}


