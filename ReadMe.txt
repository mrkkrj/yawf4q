========================================================================
    YAWF: a pluggable mini HTTP server & template engine for Qt 
========================================================================

Qt-only (no other dependencies) module to add a simple Web interface to your Qt application.
Uses Qt 4.5. Need to set the QTLIB env var to compile the VS project.

-- 1. Use it like this:

#include "CuteHttpServer.hpp"

int main(int argc, char* argv[])
{
    // your application:
    QApplication* qtAppl = new QApplication(argc, argvAscii);
    
    // the webserver on port 3100
    ibkrj::yawf4q::CuteHttpServer testServer(3100);
    string error;

    if(!testServer.start(error))
        return -1;
  
    // run Qt
    qtAppl->exec();
  }
  
    
-- 2. It uses Yawf4qConfig.txt file to configure HTTP routing like this:
      
    # YAWF test configuration
    index.html$ :: TestHandlerBase
    person$ :: TestHandlerPerson
  
  
-- 3. Your request handlers inherit the base handler class like this (warning: oldskool here!):

    class TestHandlerBase
        : public CuteHttpHandler
    {
    public:
        TestHandlerBase() 
            : CuteHttpHandler("TestHandlerBase") {}; // boilerplate

        // base overrides
        virtual CuteHttpHandler* clone() { return new TestHandlerBase; } // boilerplate
        virtual void process(CuteSrvRequest& reqData, std::string& respData)
        {
            // handle your request 
            // ... 
            
            // you may use a template if you want:
            respData = CuteHttpHandler::renderThis(jsonDataMap, templString);
        }

    } g_handler1;


That's all.

PS: template magic is avoided as not to add a Boost dependency in here. If you don't mind that 
    just use cppnetlib instead!


------------------
OPEN TODO:::

- routing: dispatch to different methods of a handler class (design ready, just implement)
- read the templates from files
- cache parsed templates
- add config file parameter to CuteHttpServer's constructor
- add direct parametrization with on the fly handlers and  CuteHttpServer's constructor (???)
- run a memory leak tool over that!
- remove compiler warnings
- serious testing

and more:
- HTTPS support in the sever (that shouldn't be very difficult as Qt offers QSslSocket and other classes)
- configurable threading models: SINGLE_THREADED, THREAD_POOL, XXX???
- cookies and session data handling
- dynamic loading of the servlets via DLLs (kind of what ASP is doing)