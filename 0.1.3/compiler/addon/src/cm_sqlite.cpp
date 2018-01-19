/*!
 *    \file  cm_sqlite.cpp
 *   \brief  sqilte wrapper implement
 *  
 *  wrapper create table, statement step etc.
 *  
 *  \author  Wang Xiaolong (WXL), wangxl3@mapbar.com
 *  
 *  \internal
 *       Created:  11/28/2016
 *      Revision:  none
 *      Compiler:  gcc
 *  Organization:  mapbar co.
 *     Copyright:  mapbar
 *  
 *  This source code is released for free distribution under the terms of the
 *  GNU General Public License as published by the Free Software Foundation.
 */

/*!
 *  \page cm_sqlite sqlite wrapper
 *  some detail for sqlite wrapper
 */
//-----------------------------------------------------------------------------
//  Header Section
//-----------------------------------------------------------------------------
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include "cm_sqlite.hpp"
#include "cm_debug.h"

//-----------------------------------------------------------------------------
//  Macro Defination Section
//-----------------------------------------------------------------------------
#define LOG_HEADER "[CM_SQLITE]"

//-----------------------------------------------------------------------------
//  Class CCmSqlite Implement Section
//-----------------------------------------------------------------------------
CCmSqlite::CCmSqlite(const char* path)
{
   int rc = sqlite3_open_v2(path, &m_db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
   if (SQLITE_OK != rc)
   {
      std::ostringstream os;
      os << "open db error! bad database path : " << path;
      throw std::logic_error(os.str());
   }
}

CCmSqlite::~CCmSqlite()
{
   for (auto e : m_stmt)
   {
      if (e)
      {
         delete e;
      }
   }
}

bool CCmSqlite::execute(const char* sql)
{
   bool ok = true;
   char* err = NULL;
   int rc = sqlite3_exec(m_db, sql, NULL, NULL, &err);
   if (SQLITE_OK != rc)
   {
      if (NULL != err)
      {
         CM_LOG_ERROR("%s failed to execute : %s by excute %s!", LOG_HEADER, err, sql);
         sqlite3_free(err);
      }

      ok = false;
   }

   return ok;
}

CCmSqlite::statement* CCmSqlite::create_statement(const char* s)
{
   statement* stmt = nullptr;
   try
   {
      stmt = new statement(m_db, s);
   }
   catch(std::exception& e)
   {
      CM_LOG_ERROR("%p %s\n", stmt, e.what());
   }

   if (stmt)
   {
      m_stmt.push_back(stmt);
   }

   return stmt;
}

void CCmSqlite::remove_statement(CCmSqlite::statement* s)
{
   if ( s ) 
   {
      //CM_LOG_INFO("%s remove statement pointer %p", LOG_HEADER, s);
      auto pos = std::remove(m_stmt.begin(), m_stmt.end(), s);
      if ( m_stmt.end() != pos )
      {
         m_stmt.pop_back();
      }
   }
}

bool CCmSqlite::backup(const char* path)
{
   bool ok = false;
   if(path)
   {
      sqlite3* dst_db = nullptr;
      int rc = sqlite3_open_v2(path, &dst_db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
      if (SQLITE_OK == rc)
      {
         sqlite3_backup* bak = sqlite3_backup_init(dst_db, "main", m_db, "main");
         if ( bak ) 
         {
            sqlite3_backup_step(bak, -1);
            sqlite3_backup_finish(bak);
            ok = true;
         }
         else
         {
            CM_LOG_WARNING("%s backup destinate path \"%s\" initialize failed!", LOG_HEADER, path);
         }
      }
      else
      {
         CM_LOG_WARNING("%s backup destinate path \"%s\" open failed!", LOG_HEADER, path);
      }
   }
   else
   {
      CM_LOG_WARNING("%s backup null path!", LOG_HEADER);
   }

   return ok;
}

bool CCmSqlite::attach(const char* path, const char* alias)
{
   bool ok = false;
   if ( path && alias ) {
      std::string sql = std::string("attach database \'") + path + "\' as " + alias + ";";
      ok = execute(sql.c_str());
   }
   return ok;
}

bool CCmSqlite::detach(const char* db)
{
   bool ok = false;
   if ( db ) {
      std::string sql = std::string("detach database ") + db + ";";
      ok = execute(sql.c_str());
   }
   return ok;
}
//-----------------------------------------------------------------------------
//  Class CCmSqlite::statement Implement Section
//-----------------------------------------------------------------------------
CCmSqlite::statement::statement(sqlite3* db, const char* s) : m_sql(s)
{
   int rc = sqlite3_prepare(db, s, -1, &m_stmt, NULL);
   if (SQLITE_OK != rc)
   {
      std::ostringstream os;
      os << "statement prepared error! The statement is \"" << s <<"\". " << sqlite3_errmsg(db);
      throw std::logic_error(os.str());
   }
}

CCmSqlite::statement::~statement()
{
   sqlite3_finalize(m_stmt);
}

bool CCmSqlite::statement::step()
{
   bool ok = false;
   int rc = sqlite3_step(m_stmt);
   if (SQLITE_OK == rc)
   {
      ok = true;
   }

   return ok;
}

bool CCmSqlite::statement::step_row()
{
   bool row = false;
   int rc = sqlite3_step(m_stmt);
   if (SQLITE_ROW == rc)
   {
      row = true;
   }

   return row;
}

const char* CCmSqlite::statement::get_text(size_t pos)
{
   return reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, pos));
}

bool CCmSqlite::statement::bind_text(size_t pos, const char* s)
{
   sqlite3_bind_text(m_stmt, pos, s, -1, NULL);
   return true;
}

void CCmSqlite::statement::reset()
{
   sqlite3_reset(m_stmt);
}
