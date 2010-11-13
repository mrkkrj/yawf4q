//
// MstchTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <CuteMoustache.hpp>
#include <CuteTrace.hpp>
#include <string>

using namespace std;
using namespace ibkrj::yawf4q;

/*
    A typical Mustache template:

    Hello {{name}}
    You have just won ${{value}}!
    {{#in_ca}}
    Well, ${{taxed_value}}, after taxes.
    {{/in_ca}}

    Given the following hash:

    {
      "name": "Chris",
      "value": 10000,
      "taxed_value": 10000 - (10000 * 0.4),
      "in_ca": true
    }
    
    Will produce the following:

    Hello Chris
    You have just won $10000!
    Well, $6000.0, after taxes.
*/

int _tmain(int argc, _TCHAR* argv[])
{
    //
    CuteMoustache ms;
    string templ = 
        "Hello {{name}}\nYou have just won ${{value}}!\n" // OPEN TODO:: if \n, eat!?
        "{{#in_ca}}" 
            "Well, ${{taxed_value}}, after taxes."
        "{{/in_ca}}";


    // OPEN TODO:: some templ. magic for nice init syntax like {..., ...}, 
    //  -- but would need Boost.Proto ... ouch!
    CuteMstchValMap valHash;

    valHash.addString("name", "Chris");
    valHash.addInt("value", 10000);
    valHash.addInt("taxed_value", 10000 - (10000 * 0.4)); // OPEN TODO: addFloat() !!!
    valHash.addBool("in_ca", true);


    string out = ms.process(templ, valHash);   
    TRACE2(" ## TEST: got -> \n", out);

    string expected = 
        "Hello Chris\nYou have just won $10000!\nWell, $6000, after taxes.";

    if(out == expected)
        TRACE(" ## TEST1 - ok!");
    else
        TRACE(" ## TEST1 - Error !!!!");

    // test 2.
    {
        CuteMstchValMap valHash;

        valHash.addString("name", "Chris");
        valHash.addInt("value", 10000);
        valHash.addInt("taxed_value", 10000 - (10000 * 0.4));
        valHash.addBool("in_ca", false);

        CuteMoustache ms;
        string out = ms.process(templ, valHash);   

        TRACE2(" ## TEST: got -> \n", out);

        string expected = 
            "Hello Chris\nYou have just won $10000!\n";

        if(out == expected)
            TRACE(" ## TEST2 - ok!");
        else
            TRACE(" ## TEST2 - Error !!!!");
    }


    // test 3.    
    {
        CuteMstchValMap valHash;

        valHash.addString("name", "Chris");
        valHash.addInt("value", 10000);
        valHash.addInt("taxed_value", 10000 - (10000 * 0.4));

        valHash.addToList("in_ca", 0, "taxed_value", "100");
        valHash.addToList("in_ca", 1, "taxed_value", 200);
        valHash.addToList("in_ca", 2, "taxed_value", 300);
        valHash.addToList("in_ca", 3, "taxed_value", (int)(10000 - (10000 * 0.4)));

        CuteMoustache ms;
        string out = ms.process(templ, valHash);   

        TRACE2(" ## TEST: got -> \n", out);

        string expected = 
            "Hello Chris\nYou have just won $10000!\n";

        if(out == expected)
            TRACE(" ## TEST2 - ok!");
        else
            TRACE(" ## TEST2 - Error !!!!");
    }

    // test 3a.
    {
        CuteMstchValMap valHash;
        
        valHash.addToList("fields_tbl", 0, "field_name", "Name");
        valHash.addToList("fields_tbl", 0, "field_value", "Christofer");
        valHash.addToList("fields_tbl", 1, "field_name", "Family Name");
        valHash.addToList("fields_tbl", 1, "field_value", "Robin");
        valHash.addToList("fields_tbl", 2, "field_name", "Age");
        valHash.addToList("fields_tbl", 2, "field_value", 9);

        string respPage = 
        "<html>\n"
            "<head><title>TestHandlerBase is alive!</title></head>\n"
        "<body>\n"
            "<h2> Thank you for your data! </h2>\n"
            "<table border=\"1\"  width=\"70%\">\n"
            "<tr>"
            "<th><b>Field</b></th>"
            "<th><b>Value</b></th>"
            "</tr>\n"
            "{{#fields_tbl}}"
            "<tr> <td> {{field_name}} </td> <td>{{field_value}}</td> </tr>\n"
            "{{/fields_tbl}}"
            "</table>\n"
            "<p></p> <a href=\"/index.html\">Back</a><br>\n"
        "</body>"
        "</html>";

        CuteMoustache ms;
        string out = ms.process(respPage, valHash);   

        TRACE2(" ## TEST: got -> \n", out);
    }

    // test 4 -- trying the mustache homepage exmaple
    {             

        CuteMstchValMap valHash;

        //{
        //  "header": "Colors",
        //  "items": [
        //      {"name": "red", "first": true, "url": "#Red"},
        //      {"name": "green", "link": true, "url": "#Green"},
        //      {"name": "blue", "link": true, "url": "#Blue"}
        //  ],
        //  "empty": false
        //}


        valHash.addString("header", "Colors");

        valHash.addToList("items", 0, "name", "red");
        valHash.addToList("items", 0, "first", true);
        valHash.addToList("items", 0, "url", "#Red");

        valHash.addToList("items", 1, "name", "green");
        valHash.addToList("items", 1, "link", true);
        valHash.addToList("items", 1, "url", "#Green");

        valHash.addToList("items", 2, "name", "blue");
        valHash.addToList("items", 2, "link", true);
        valHash.addToList("items", 2, "url", "#Blue");

        valHash.addBool("empty", true);


        string respPage =             
            "<h1>{{header}}</h1>"
            "{{#bug}}"
            "{{/bug}}"
            "\n"
            "{{#items}}"
            "  {{#first}}"
            "    <li><strong>{{name}}</strong></li>\n"
            "  {{/first}}"
            "  {{#link}}"
            "    <li><a href=\"{{url}}\">{{name}}</a></li>\n"
            "  {{/link}}"
            "{{/items}}"
            ""
            "{{#empty}}"
            "  <p>The list is empty.</p>\n"
            "{{/empty}}";

        CuteMoustache ms;
        string out = ms.process(respPage, valHash);   

        TRACE2(" ## TEST: got -> \n", out);
    }

	return 0;
}

