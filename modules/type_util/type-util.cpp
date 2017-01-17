#include "type-util.h"
#include <sstream>

std::string Util::TypeConverter::int2String(int input) {
    std::ostringstream stream;
    stream<<input;  //n为int类型
    return stream.str();
}