#include "value.h"

#include <sstream>

using namespace Platformer;

Value::Value():
type(String){
}

Value::Value(const std::string & value):
type(String),
value(value){
}

Value::Value(int value){
    std::ostringstream out;
    out << value;
    this->value = out.str();
    type = Int;
}

Value::Value(double value){
    std::ostringstream out;
    out << value;
    this->value = out.str();
    type = Double;
}

Value::Value(bool value){
    std::ostringstream out;
    out << value;
    this->value = out.str();
    type = Boolean;
}

Value::Value(const Value & copy):
type(copy.type),
value(copy.value){
}

Value::~Value(){
}

const Value & Value::operator=(const Value & copy) {
    type = copy.type;
    value = copy.value;
    return *this;
}

const Value & Value::operator=(const char * value) {
    this->value = std::string(value);
    type = String;
    return *this;
}

const Value & Value::operator=(const std::string & value) {
    this->value = value;
    type = String;
    return *this;
}

const Value & Value::operator=(int value) {
    std::ostringstream out;
    out << value;
    this->value = out.str();
    type = Int;
    return *this;
}

const Value & Value::operator=(double value) {
    std::ostringstream out;
    out << value;
    this->value = out.str();
    type = Double;
    return *this;
}

const Value & Value::operator=(bool value) {
    std::ostringstream out;
    out << value;
    this->value = out.str();
    type = Boolean;
    return *this;
}

const Value & Value::operator>>(std::string & value) {
    value = this->value;
    return *this;
}

const Value & Value::operator>>(int value) {
    std::istringstream in(this->value);
    in >> value;
    return *this;
}

const Value & Value::operator>>(double value) {
    std::istringstream in(this->value);
    in >> value;
    return *this;
}

const Value & Value::operator>>(bool value) {
    std::istringstream in(this->value);
    in >> value;
    return *this;
}

const std::string & Value::toString() const{
    return value;
}

int Value::toInt() const{
    int v = 0;
    std::istringstream in(value);
    in >> v;
    return v;
}

double Value::toDouble() const{
    double v = 0;
    std::istringstream in(value);
    in >> v;
    return v;
}

bool Value::toBool() const{
    bool v = false;
    std::istringstream in(value);
    in >> v;
    return v;
}

void Value::clear(){
    value.clear();
}
