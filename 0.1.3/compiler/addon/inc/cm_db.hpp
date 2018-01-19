#pragma once
#include <locale>
#include <string>
#include <cstdint>
#include "cm_sqlite.hpp"
/*!
 *  \defgroup grp_db db group
 * 
 *  \brief    something about CM db
 *  
 *  some detail about CM db
 */
/// \brief compiler Database
class CCmDatabase
{
public:
   CCmDatabase();
   ~CCmDatabase();

   bool import_mid(const char*);
   bool parse_db(const char*);
   bool do_argv(std::vector<std::string>&);
private:
   // helper
   bool open_mid(const char*, const size_t, const char*);
   bool open_mid_N(const char*);
   bool open_mid_C(const char*);
   bool open_mid_CR(const char*);
   bool open_mid_Toll_ETA(const char*);
   bool open_mid_Toll_Pattern(const char*);
   bool open_mid_HW_Junction(const char*);
   bool open_db(const char*);
   bool save_as(const char*);
   bool parse_db_CR(const char*);
   bool parse_db_Toll_ETA(const char*);
   bool parse_db_Toll_Pattern(const char*);
   bool parse_db_HW_Junction(const char*);
   bool parse_db_C_CR_Toll(const char*);
   std::string parse_db_C_CR_Toll();
   
   bool combine_db_C_CR(const char*, const char*, const char*);
   bool combine_db_C_CR_Toll(const char*, const char*, const char*, const char*, const char*);
   // utilities
   bool isLeadSameIcStr(const std::string&, const std::string&, std::string::size_type = std::string::npos);
   void fit_to_graph(std::string&);
   std::tuple<std::string, std::string, std::string> parse_path(const std::string&);
private:
   CCmSqlite* m_db;
   std::locale m_loc;
   CCmSqlite::statement *m_stmtSelectCR;
   CCmSqlite::statement *m_stmtSelectTollETA;
   CCmSqlite::statement *m_stmtSelectTollPatern;
   CCmSqlite::statement *m_stmtSelectHWJunction;
};
