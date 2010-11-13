//
// TestAppHandlers.cpp: some HTTP request handlers for the YAWF Qt module
//


#include "CuteHttpHandler.hpp"
#include "CuteHttpServer.hpp" // for CuteSrvRequest

using namespace std;
using namespace ibkrj::yawf4q;


//----------- TestHandlerBase ---------------

///////////// boilerplate:

class TestHandlerBase
    : public CuteHttpHandler
{
public:
    TestHandlerBase() 
        : CuteHttpHandler("TestHandlerBase") {};

    // base overrides
    virtual CuteHttpHandler* clone() { return new TestHandlerBase; }
    virtual void process(CuteSrvRequest& reqData, std::string& respData);

} g_handler1;

// --- OR maybe MACRO?:
//  REGISTER_TEST_HANDLER(TestHandlerBase, 1)


///////////// impl:

const char* testform = 
    "<form action=\"person\" method=\"post\" accept-charset=\"ISO-8859-1\">"
    "<p>Name:<br>"
    "<input name=\"UserName\" size=\"40\"></p>"
    "<p>E-Mail:<br>"
    "<input name=\"UserMail\" size=\"40\"></p>"
    "<p>Text:<br>"
    "<textarea name=\"Text\" rows=\"5\" cols=\"50\"></textarea></p>"
    "<p><input type=\"submit\" value=\"Submit user data\"></p>"
    "</form>";


void TestHandlerBase::process(CuteSrvRequest& reqData, std::string& respData)
{    
    respData = " TestHandlerBase alive!!!! ";    

    respData = 
        "<html>"
        "<head>"
        "<title>TestHandlerBase is alive!</title>"
        "</head>"
        "<body>";

    respData.append("<h1> Input your user data: </h1><p></p>");

    respData.append(testform);

    respData += 
        "</body>"
        "</html>";
}


//----------- TestHandlerPerson ---------------

///////////// boilerplate:

class TestHandlerPerson
    : public CuteHttpHandler
{
public:
    TestHandlerPerson() 
        : CuteHttpHandler("TestHandlerPerson") {};

    // base overrides
    virtual CuteHttpHandler* clone() { return new TestHandlerPerson; }
    virtual void process(CuteSrvRequest& reqData, std::string& respData);

} g_handler2;

// --- OR maybe MACRO?:
//class TestHandlerPerson
//    : public CuteHttpHandler
//{
//public:
//  BASE_TEST_HANDLER_FUNC(TestHandlerPerson) 
//} xxx;


///////////// impl:

#if 0
void TestHandlerPerson::process(CuteSrvRequest& reqData, std::string& respData)
{    
    respData = 
        "<html>"
        "<head>"
        "<title>TestHandlerBase is alive!</title>"
        "</head>"
        "<body>";

    respData.append("<h2> Thank you for your data! </h2>");

    // parse the data for display:
    map<string,string>& data = reqData.formularData;
    map<string,string>::iterator iter;

    respData.append(
        "<table border=\"1\"  width=\"70%\">"
        "<tr>"
        "<th><b>Field</b></th>"
        "<th><b>Value</b></th>"
        "</tr>");

    for(iter = data.begin(); iter != data.end(); iter++)
    {
        respData.append("<tr><td>").append(iter->first).append("</td>");
        respData.append("<td>").append(iter->second).append("</td></tr>");
    }

    respData.append("</table>");
    respData.append("<p></p> <a href=\"/index.html\">Back</a><br>");

    respData += 
        "</body>"
        "</html>";
}
#endif

// using templating:
#include "CuteMstchData.hpp"

void TestHandlerPerson::process(CuteSrvRequest& reqData, std::string& respData)
{    
    string templ = 
        "<html><head>"
        "<title>TestHandlerBase is alive!</title>"
        "</head> <body>"
        "<h2> Thank you for your data! </h2>"
        "<table border=\"1\"  width=\"70%\">"
        "<tr><th><b>Field</b></th>"
             "<th><b>Value</b></th></tr>"
             "{{#datatable}}"
             "<tr><td>{{field}}</td> <td>{{value}}</td></tr>"
             "{{/datatable}}"
        "</table>"
        "<p></p> <a href=\"/index.html\">Back</a><br>"
        "</body></html>";

    // convert data to JSON represenation:
    map<string,string>& inpData = reqData.formularData;
    map<string,string>::iterator iter;
    int i;

    ibkrj::yawf4q::CuteMstchValMap templData;    

    for(iter = inpData.begin(), i = 0; iter != inpData.end(); iter++, i++)
    {
        templData.addToList("datatable", i, "field", iter->first);
        templData.addToList("datatable", i, "value", iter->second);
    }

    // display page with data
    respData = CuteHttpHandler::renderThis(templData, templ);
}


/////////////////// TEST:::


#if 0



// --> or use template??
template <class T> class T : public CuteHttpHandler
{
public:
    T() : CuteHttpHandler("T") {};

    // base overrides
    virtual CuteHttpHandler* clone() { return new T; }
    virtual void process(CuteSrvRequest& reqData, std::string& respData);


};

template TestHandlerTst<TestHandlerTst> g_test;

// OR use CRTP HERE:
http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

// Base class has a pure virtual function for cloning
class Shape {
public:
    virtual ~Shape() {}
    virtual Shape *clone() const = 0;
};
// This CRTP class implements clone() for Derived
template <typename Derived> class Shape_CRTP: public Shape {
public:
    Shape *clone() const {
        return new Derived(dynamic_cast<Derived const&>(*this));
    }
};

// Every derived class inherits from Shape_CRTP instead of Shape
class Square: public Shape_CRTP<Square> {};
class Circle: public Shape_CRTP<Circle> {};

#endif

//----------------------- END ------------------------