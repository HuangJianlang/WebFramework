//
// Created by Jianlang Huang on 12/8/20.
//

#include "../components/config.h"
#include "../components/log.h"
#include <yaml-cpp/yaml.h>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <unordered_map>

void test_yaml(){
    YAML::Node root = YAML::LoadFile("../config/test.yml");
    LOG_INFO(LOG_ROOT()) << root;
}

ConfigVar<int>::pointer g_int_value_config = Config::Lookup("system.port", (int)8080, "system port");
ConfigVar<float>::pointer g_float_value_config = Config::Lookup("system.value", (float)10.12, "system value");
ConfigVar<std::vector<int>>::pointer g_int_vec_config = Config::Lookup("system.int_vec", std::vector<int>{1,2,3}, "system vector");
ConfigVar<std::list<int>>::pointer g_int_list_config = Config::Lookup("system.int_list", std::list<int>{4,5,6}, "system list");
ConfigVar<std::set<int>>::pointer g_int_set_config = Config::Lookup("system.int_set", std::set<int>{7,8,9}, "system set");
ConfigVar<std::unordered_set<int>>::pointer g_int_hashset_config = Config::Lookup("system.int_hashset", std::unordered_set<int>{10,11,12}, "system hashset");
ConfigVar<std::map<std::string, int>>::pointer g_int_map_config = Config::Lookup("system.int_map", std::map<std::string, int>{{"key", 50}}, "system int map");
ConfigVar<std::unordered_map<std::string, int>>::pointer g_int_unordered_map_config = Config::Lookup("system.int_unordered_map", std::unordered_map<std::string, int>{{"key", 50}}, "system int unordered_map");

void test_config(){
    LOG_INFO(LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    LOG_INFO(LOG_ROOT()) << "before: " << g_float_value_config->toString();

#define XX(g_val, name, prefix) \
    for (auto i : g_val->getValue()) \
        LOG_INFO(LOG_ROOT()) << #prefix << " " << #name << " : " << i; \
    LOG_INFO(LOG_ROOT()) << "YAML : " << g_val->toString();

#define XX_M(g_val, name, prefix) \
    for (auto i : g_val->getValue()) \
        LOG_INFO(LOG_ROOT()) << #prefix << " " << #name << " : " << "{ " << i.first << " : " << i.second << " }"; \
    LOG_INFO(LOG_ROOT()) << "YAML : " << g_val->toString();

    XX(g_int_vec_config, int_vec, before);
    XX(g_int_list_config, int_list, before);
    XX(g_int_set_config, int_set, before);
    XX(g_int_hashset_config, int_hashset, before);
    XX_M(g_int_map_config, int_map, before);
    XX_M(g_int_unordered_map_config, int_unordered_map, before);

    YAML::Node root = YAML::LoadFile("../config/test.yml");
    Config::LoadFromYaml(root);

    LOG_INFO(LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    LOG_INFO(LOG_ROOT()) << "after: " << g_float_value_config->toString();

    XX(g_int_vec_config, int_vec, after);
    XX(g_int_list_config, int_list, after);
    XX(g_int_set_config, int_set, after);
    XX(g_int_hashset_config, int_hashset, after);
    XX_M(g_int_map_config, int_map, after);
    XX_M(g_int_unordered_map_config, int_unordered_map, after);
}

void test_config_log(){
    static auto system_log = LOG_NAME("system");
    LOG_INFO(LOG_ROOT()) << "loggers:\n" <<LoggerMgr::GetInstance()->toYamlString();
    YAML::Node root = YAML::LoadFile("../config/test_log.yml");
    Config::LoadFromYaml(root);
    std::cout << "========================================" << std::endl;
    LOG_INFO(LOG_ROOT()) << "root loggers:\n" << LoggerMgr::GetInstance()->toYamlString();
    LOG_INFO(system_log) << "system loggers: this is system logger\n";

    system_log->setFormatter("%d - %m%n");
    LOG_INFO(system_log) << "system loggers: after setting new formatter\n";

}

int main(int argc, char* argv[]){
    std::cout << "test for loading yaml\n";
    test_config_log();
    return 0;
}