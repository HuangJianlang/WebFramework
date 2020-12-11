//
// Created by Jianlang Huang on 12/8/20.
//

#include "config.h"
#include <list>

//Config::ConfigVarMap Config::s_datas;

ConfigVarBase::pointer Config::LookupBase(const std::string &name) {
    auto it = GetDatas().find(name);
    return it == GetDatas().end()? nullptr : it->second;
}

// A:
//  B   -> A.B
//
static void ListAllMember(const std::string& prefix,
                          const YAML::Node& node,
                          std::list<std::pair<std::string, const YAML::Node>>& output){
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos){
        LOG_ERROR(LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }

    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
        for (auto& i : node){
            ListAllMember(prefix.empty() ? i.first.Scalar() : prefix+"." + i.first.Scalar(), i.second, output);
        }
    }
}
void Config::LoadFromYaml(const YAML::Node& root){
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);

    for(auto& node : all_nodes){
        std::string key = node.first;
        if (key.empty()){
            continue;
        }

        //把key变成小写
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        auto base = LookupBase(key);
        if (base) {
            if (node.second.IsScalar()){
                base->fromString(node.second.Scalar());
            } else {
                std::stringstream ss;
                ss << node.second;
                base->fromString(ss.str());
            }
        }
    }
}