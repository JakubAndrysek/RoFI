#pragma once

#include <memory>
#include <string>
#include "nvs_flash.h"

namespace esp {

class EspNvs {
    enum DataType : uint8_t {
        INT64 = 0,
        FLOAT32 = 1,
        STRING = 2,

        NOT_FOUND = 0xFF,
    };

    std::string _nsname;
    nvs_handle_t _handle;
  public:

    static void initNvs();
    static std::unique_ptr<EspNvs> open(const std::string& nsname);

    EspNvs(const std::string& nsname, nvs_handle_t handle);
    ~EspNvs();

    bool erase(const std::string& name);

    void setInt(const std::string& name, int64_t value);
    void setFloat(const std::string& name, float value);
    void setString(const std::string& name, const std::string& value);

    int64_t getInt(const std::string& name, int64_t def_value = 0);
    float getFloat(const std::string& name, float def_value = 0.f);
    std::string getString(const std::string& name, std::string def_value = "");

    DataType getType(const std::string& name);

    bool commit();
};


// nvs exception
class EspNvsException : public std::exception {
    std::string _msg;
  public:
    EspNvsException(std::string msg) : _msg(msg) {
    }

    const char* what() const noexcept override {
        return _msg.c_str();
    }
};

} // namespace esp