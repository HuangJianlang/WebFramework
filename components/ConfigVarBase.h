//
// Created by Jianlang Huang on 12/8/20.
//

#ifndef WEBFRAMEWORK_CONFIGVARBASE_H
#define WEBFRAMEWORK_CONFIGVARBASE_H

#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <map>

#include "log.h"

class ConfigVarBase {
public:
    using pointer = std::shared_ptr<ConfigVarBase>;

    explicit ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name),
        m_description(description) {

    }

    virtual ~ConfigVarBase() = default;

    const std::string& getName() const {
        return m_name;
    }

    const std::string getDescription() const {
        return m_description;
    }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;

protected:
    std::string m_name;
    std::string m_description;
};

template<typename T>
class ConfigVar : public ConfigVarBase {
public:
    using pointer = std::shared_ptr<ConfigVar>;

    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        : ConfigVarBase(name, description), m_val(default_value) {

    }

    //Convert type T to String
    virtual std::string toString() override {
        try {
            return boost::lexical_cast<std::string>(m_val);
        } catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::toString exception" << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return "";
    };

    //Convert String to T
    virtual bool fromString(const std::string& val) override {
        try {
            m_val = boost::lexical_cast<T>(val);
        } catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::fromString exception" << e.what() << " convert: " << "string to " << typeid(m_val).name();
        }
        return false;
    };

    const T getValue() const {
        return m_val;
    }

    void setValue(const T& val) {
        m_val = val;
    }

private:
    T m_val;
};

class Config{
public:
    using ConfigVarMap = std::map<std::string, ConfigVarBase::pointer>;

    template <typename T>
    static typename ConfigVar<T>::pointer Lookup(const std::string& name,
            const T& default_value, const std::string& description = ""){
        auto temp = Lookup<T>(name);
        if (temp) {
            LOG_INFO(LOG_ROOT()) << "Lookup name=" << name << " exists.";
            return temp;
        }

        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != std::string::npos){
            LOG_ERROR(LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::pointer config(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = config;
        return config;
    }

    template <typename T>
    static typename ConfigVar<T>::pointer Lookup(const std::string& name){
        auto it = s_datas.find(name);
        if (it == s_datas.end()){
            return nullptr;
        }
        //dynamic cast 会将基类指针cast到派生类指针
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }
private:
    static ConfigVarMap s_datas;

};
#endif //WEBFRAMEWORK_CONFIGVARBASE_H
