#ifndef __GENERIC_WRITER_H__
#define __GENERIC_WRITER_H__

#include "commacro.h"
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

namespace custom {
class COMMON_EXPORT GenericWriter {
    enum Type {
        kNullType = 0,  //!< null
        kkeyType = 1,   //!< key
        kValueType = 2, //!< value
    };
    typedef std::pair<int32_t, uint32_t> value_type;
    std::vector<value_type> _stack;
    std::string &_str;
    bool _formatted;

  public:
    explicit GenericWriter(std::string &str, bool formatted = false)
        : _str(str), _formatted(formatted) {}
    void Bool(bool b);
    void Int64(int64_t i64);
    void Uint64(uint64_t u64);
    void Double(double d);
    void Float(float d);
    GenericWriter &Key(const char *szKey);
    void String(const char *szValue);
    void StartObject();
    void EndObject();
    void StartArray();
    void EndArray();

    void Separation() {
        if (!_stack.empty() && _stack.back().first == kNullType) {
            comma(_str);
        }
    }

    bool result() const { return _stack.empty(); }
    // :
    void colon(std::string &str) const;
    // ,
    void comma(std::string &str) const;
    // {
    void leftBrace(std::string &str) const;
    // }
    void rightBrace(std::string &str, int32_t layer) const;
    // [
    void leftBracket(std::string &str) const;
    // ]
    void rightBracket(std::string &str, int32_t layer) const;
    // \t
    void tab(std::string &str, int32_t layer) const;
};

} // namespace custom

#endif
