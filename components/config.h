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
#include <functional>

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
    virtual std::string getTypeName() = 0;
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

template<typename Target>
class LexicalCast<std::string, std::map<std::string, Target>>{
private:
    using ReturnType = std::map<std::string, Target>;

public:
    ReturnType operator()(const std::string& data){
        std::stringstream ss;
        YAML::Node node = YAML::Load(data);
        ReturnType ret;
        for(auto iter = node.begin(); iter != node.end(); iter++){
            ss.str("");
            //value
            ss << iter->second;
            ret.insert(std::make_pair(iter->first.Scalar(), LexicalCast<std::string, Target>()(ss.str())));
        }
        return ret;
    }
};

template<typename Source>
class LexicalCast<std::map<std::string, Source>, std::string> {
private:
    using SourceType = std::map<std::string, Source>;

public:
    std::string operator()(const SourceType& data){
        std::stringstream ss;
        YAML::Node node;
        for (auto& i : data){
            node[i.first] = YAML::Load(LexicalCast<Source, std::string>()(i.second));
        }
        ss << node;
        return ss.str();
    }
};

//hash map
template<typename Target>
class LexicalCast<std::string, std::unordered_map<std::string, Target>>{
private:
    using ReturnType = std::unordered_map<std::string, Target>;

public:
    ReturnType operator()(const std::string& data){
        std::stringstream ss;
        YAML::Node node = YAML::Load(data);
        ReturnType ret;
        for(auto iter = node.begin(); iter != node.end(); iter++){
            ss.str("");
            //value
            ss << iter->second;
            ret.insert(std::make_pair(iter->first.Scalar(), LexicalCast<std::string, Target>()(ss.str())));
        }
        return ret;
    }
};

template<typename Source>
class LexicalCast<std::unordered_map<std::string, Source>, std::string> {
private:
    using SourceType = std::unordered_map<std::string, Source>;

public:
    std::string operator()(const SourceType& data){
        std::stringstream ss;
        YAML::Node node;
        for (auto& i : data){
            node[i.first] = YAML::Load(LexicalCast<Source, std::string>()(i.second));
        }
        ss << node;
        return ss.str();
    }
};
//===========================stl support end=====================

template<typename T, typename FromStr = LexicalCast<std::string, T>, typename ToStr = LexicalCast<T, std::string>>
//这个类的主要作用是将来自字符串中的内容转换为简单类型（如int float等）
class ConfigVar : public ConfigVarBase {
public:
    using pointer = std::shared_ptr<ConfigVar>;
    using on_change_callback = std::function<void (const T& old_value, const T& new_value)>;

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
            setValue(FromStr()(val));
        } catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::fromString exception" << e.what() << " convert: " << "string to " << typeid(m_val).name();
        }
        return false;
    };

    const T getValue() const {
        return m_val;
    }

    void setValue(const T& val) {
        if (val == m_val){
            return;
        }
        for (auto& i : m_callbacks){
            i.second(m_val, val);
        }
        m_val = val;
    }

    std::string getTypeName() override {
        return typeid(T).name();
    }

    void addListener(uint64_t key, on_change_callback cb) {
        m_callbacks[key] = cb;
    }

    void delListener(uint64_t key){
        m_callbacks.erase(key);
    }

    on_change_callback getListener(uint64_t key){
        auto it = m_callbacks.find(key);
        return it == m_callbacks.end() ? nullptr : it->second;
    }

    void clearListener(){
        m_callbacks.clear();
    }
private:
    //这里用于保存所有的配置信息，若在yml中记录的set，这里就是set
    T m_val;
    std::unordered_map<uint64_t, on_change_callback> m_callbacks;
};


class Config{
public:
    using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::pointer>;

    template <typename T>
    static typename ConfigVar<T>::pointer Lookup(const std::string& name,
            const T& default_value, const std::string& description = ""){

        auto it = GetDatas().find(name);

        if (it != GetDatas().end()){
            auto temp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (temp){
                LOG_INFO(LOG_ROOT()) << "Lookup name=" << name << " exists.";
                return temp;
            } else { //invalid type
                LOG_ERROR(LOG_ROOT()) << "Lookup name=" << name << " exists, but type not " << typeid(T).name()
                << " real type is " << it->second->getTypeName() << " " << it->second->toString();
                return nullptr;
            }
        }
        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos){
            LOG_ERROR(LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::pointer config(new ConfigVar<T>(name, default_value, description));
        GetDatas().insert(std::pair<std::string, ConfigVarBase::pointer>(name, config));
        return config;
    }

    template <typename T>
    static typename ConfigVar<T>::pointer Lookup(const std::string& name){
        auto it = GetDatas().find(name);
        if (it == GetDatas().end()){
            return nullptr;
        }
        //dynamic cast 会将基类指针cast到派生类指针
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        //此处返回空有两种情况
        //1 key 不存在
        //2 key 存在，但是需要的类型不一样
    }

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::pointer LookupBase(const std::string& name);
private:
    //这里用一个私有static方法来获得static data, 防止s_datas未先于LookUp方法初始化
    static ConfigVarMap& GetDatas(){
        static ConfigVarMap s_datas;
        return s_datas;
    }


};
#endif //WEBFRAMEWORK_CONFIG_H
