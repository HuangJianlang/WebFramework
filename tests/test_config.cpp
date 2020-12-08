//
// Created by Jianlang Huang on 12/8/20.
//

#include "../components/ConfigVarBase.h"

ConfigVar<int>::pointer g_int_value_config = Config::Lookup("system.port", (int)8080, "system port");

int main(int argc, char* argv[]){
    LOG_INFO(LOG_ROOT()) << g_int_value_config->getValue();
    LOG_INFO(LOG_ROOT()) << g_int_value_config->getName();
    return 0;
}