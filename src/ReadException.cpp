//this code was copied from the QFUN implementation by Mikol� Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
#include "ReadException.h"
ReadException::ReadException(const string& message) {
    s=new char[message.size()+1];
    message.copy(s,message.size(),0);
}

const char* ReadException::what() const throw() {return s;}
