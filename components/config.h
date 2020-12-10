//
// Created by Jianlang Huang on 12/8/20.
//

#ifndef WEBFRAMEWORK_CONFIG_H
#define WEBFRAMEWORK_CONFIG_H

#include "log.h"
#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

#include <map>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <unordered_map>

class ConfigVarBase {
public:
    using pointer = std::shared_ptr<ConfigVarBase>;

    explicit ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name),
        m_description(description) {
        std::transform(name.begin(), name.end(), m_name.begin(), ::towlower);
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

//===================================================

//用于基本类型的转换器
template<typename Source, typename Target>
class LexicalCast{
public:
    Target operator()(const Source& data){
        return boost::lexical_cast<Target>(data);
    }
};

//支持更多stl的转换，原理是模版偏特化
//用于vector<T> 类型的转换器
// from string to vector<T>
template<typename Target>
class LexicalCast<std::string, std::vector<Target>> {
private:
    using ReturnType = std::vector<Target>;
public:
    ReturnType operator()(std::string data){
        ReturnType ret;
        YAML::Node node = YAML::Load(data);
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.push_back(LexicalCast<std::string, Target>()(ss.str()));
        }
        return ret;
    }
};

//From vector<T> to String
template<typename Source>
class LexicalCast<std::vector<Source>, std::string> {
private:
    using SourceType = std::vector<Source>;
public:
    std::string operator()(SourceType data){
        std::stringstream ss;
        YAML::Node node;
        for (auto& i : data){
            node.push_back(LexicalCast<Source, std::string>()(i));
        }
        ss << node;
        return ss.str();
    }
};

//list
template<typename Target>
class LexicalCast<std::string, std::list<Target>> {
private:
    using ReturnType = std::list<Target>;
public:
    ReturnType operator()(std::string data){
        ReturnType ret;
        YAML::Node node = YAML::Load(data);
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.push_back(LexicalCast<std::string, Target>()(ss.str()));
        }
        return ret;
    }
};

//From list<T> to String
template<typename Source>
class LexicalCast<std::list<Source>, std::string> {
private:
    using SourceType = std::list<Source>;
public:
    std::string operator()(SourceType data){
        std::stringstream ss;
        YAML::Node node;
        for (auto& i : data){
            node.push_back(LexicalCast<Source, std::string>()(i));
        }
        ss << node;
        return ss.str();
    }
};

//set
template<typename Target>
class LexicalCast<std::string, std::set<Target>>{
private:
    using ReturnType = std::set<Target>;

public:
    ReturnType operator()(const std::string& data){
        ReturnType ret;
        std::stringstream ss;
        YAML::Node node = YAML::Load(data);
        for (int i = 0; i < node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.insert(LexicalCast<std::string, Target>()(ss.str()));
        }
        return ret;
    }
};

template<typename Source>
class LexicalCast<std::set<Source>, std::string>{
private:
    using SourceType = std::set<Source>;

public:
    std::string operator()(const SourceType& data){
        YAML::Node node;
        std::stringstream ss;
        for(auto& i : data){
            node.push_back(LexicalCast<Source, std::string>()(i));
        }
        ss << node;
        return ss.str();
    }
};


//hashset
template<typename Target>
class LexicalCast<std::string, std::unordered_set<Target>>{
private:
    using ReturnType = std::unordered_set<Target>;

public:
    ReturnType operator()(const std::string& data){
        ReturnType ret;
        std::stringstream ss;
        YAML::Node node = YAML::Load(data);
        for (int i = 0; i < node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.insert(LexicalCast<std::string, Target>()(ss.str()));
        }
        return ret;
    }
};

template<typename Source>
class LexicalCast<std::unordered_set<Source>, std::string>{
private:
    using SourceType = std::unordered_set<Source>;

public:
    std::string operator()(const SourceType& data){
        YAML::Node node;
        std::stringstream ss;
        for(auto& i : data){
            node.push_back(LexicalCast<Source, std::string>()(i));
        }
        ss << node;
        return ss.str();
    }
};
//===========================stl support=====================

template<typename T, typename FromStr = LexicalCast<std::string, T>, typename ToStr = LexicalCast<T, std::string>>
//这个类的主要作用是将来自字符串中的内容转换为简单类型（如int float等）
class ConfigVar : public ConfigVarBase {
public:
    using pointer = std::shared_ptr<ConfigVar>;

    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        : ConfigVarBase(name, description), m_val(default_value) {

    }

    //Convert type T to String
    virtual std::string toString() override {
        try {
            //return boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        } catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::toString exception" << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return "";
    };

    //Convert String to T
    virtual bool fromString(const std::string& val) override {
        try {
            //m_val = boost::lexical_cast<T>(val);
            m_val = FromStr()(val);
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

        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos){
            LOG_ERROR(LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::pointer config(new ConfigVar<T>(name, default_value, description));
        s_datas.insert(std::pair<std::string, ConfigVarBase::pointer>(name, config));
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

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::pointer LookupBase(const std::string& name);
private:
    static ConfigVarMap s_datas;

};
#endif //WEBFRAMEWORK_CONFIG_H
