#pragma once

#include <memory>
#include <iostream>
#include <unordered_map>

#include "Misc/Recv.hpp"
#include "../Singleton.hpp"

class NetvarSys
    : public Singleton<NetvarSys>
{
    struct netvar_table
    {
        std::string               name;
        RecvProp*                 prop;
        uint32_t                  offset;
        std::vector<RecvProp*>    child_props;
        std::vector<netvar_table> child_tables;
    };
public:
    void Initialize();

    void Dump();
    void Dump(std::ostream& stream);

    uint32_t      GetOffset(const std::string& tableName, const std::string& propName);
    RecvProp*     GetNetvarProp(const std::string& tableName, const std::string& propName);

private:
    static netvar_table  LoadTable(RecvTable* clientClass);
    static void          DumpTable(std::ostream& stream, const netvar_table& table, uint32_t indentation);
    static uint32_t      GetOffset(const netvar_table& table, const std::string& propName);
    static RecvProp*     GetNetvarProp(const netvar_table& table, const std::string& propName);

private:
    std::vector<netvar_table>  database;
};