//
// CuteTrace.hpp : 
//      some very basic trace macros, feel free to use your own!

#include <iostream>


#define TR_WEBIF true
#define INFO_LVL true

/////////// start trace /////////////////

#define TRACE(a) cout << " " << a << endl
#define TRACE2(a,b)   cout << " " << a << " " << kmx_trace_shim(b) << endl
#define TRACE3(a,b,c) cout << a << " " << kmx_trace_shim(b) << "/" << kmx_trace_shim(c) << endl

#define TRACE_INFO(a) cout << "INFO: " << a << endl
#define TRACE_INFO2(a,b) cout << "INFO: " << a << " " << kmx_trace_shim(b) << endl
#define TRACE_INFO3(a,b,c) cout << "INFO: " << a << " " << kmx_trace_shim(b) << "/"\
                                << kmx_trace_shim(c) << endl

#define TRACE_ERR(a) cout << "ERROR: " << a << "!" << endl
#define TRACE_ERR2(a,b) cout << "ERROR: " << a << kmx_trace_shim(b) << "!" << endl
#define TRACE_ERR3(a,b,c) cout << "ERROR: " << a << kmx_trace_shim(b) << ", "\
                               << kmx_trace_shim(c) << "!" << endl

#include <qstring.h>
#include <qbytearray.h>

// adapter for tracing of QByteArray
class kmx_qbarray_shim 
{
    QByteArray m_qba;
public:
    kmx_qbarray_shim(const QByteArray& a) : m_qba(a) {};
    operator const char*() { return m_qba.constData(); }
};

// adapter template for traces
template <class T, class U>
inline U kmx_trace_shim(const T& s)
{
    return s; 
}

// specialisation: rebound all types...
template <class T>
inline T kmx_trace_shim(const T& s)
{
    return s; 
}

// except for preventing conversion to QString!
inline const char* kmx_trace_shim(const char* s)
{
    return s;
}

// except for QString
inline kmx_qbarray_shim kmx_trace_shim(const QString& s)
{
    // must wrap as QByteArray's << op. ambigiuos!
    return kmx_qbarray_shim(s.toLatin1()); 
}

// silence VisualStudio warnings when tracing size_t
inline unsigned kmx_trace_shim(const size_t& s)
{
    return (unsigned)s;
}

/////////// end trace ////////////////////////