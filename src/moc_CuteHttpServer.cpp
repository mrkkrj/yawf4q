/****************************************************************************
** Meta object code from reading C++ file 'CuteHttpServer.hpp'
**
** Created: Sat 13. Nov 14:37:44 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CuteHttpServer.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CuteHttpServer.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ibkrj__yawf4q__CuteHttpServer[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x08,
      47,   30,   30,   30, 0x08,
      66,   30,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ibkrj__yawf4q__CuteHttpServer[] = {
    "ibkrj::yawf4q::CuteHttpServer\0\0"
    "acceptNewConn()\0removeClosedConn()\0"
    "readRequestString()\0"
};

const QMetaObject ibkrj::yawf4q::CuteHttpServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ibkrj__yawf4q__CuteHttpServer,
      qt_meta_data_ibkrj__yawf4q__CuteHttpServer, 0 }
};

const QMetaObject *ibkrj::yawf4q::CuteHttpServer::metaObject() const
{
    return &staticMetaObject;
}

void *ibkrj::yawf4q::CuteHttpServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ibkrj__yawf4q__CuteHttpServer))
        return static_cast<void*>(const_cast< CuteHttpServer*>(this));
    return QObject::qt_metacast(_clname);
}

int ibkrj::yawf4q::CuteHttpServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: acceptNewConn(); break;
        case 1: removeClosedConn(); break;
        case 2: readRequestString(); break;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
