#pragma once
#include <vector>
#include <string>
#include "sqlite3.h"

class CCmSqlite
{
public:
   CCmSqlite(const char*);
   ~CCmSqlite();

   class statement
   {
   public:
      // life cycle
      statement() = delete;
      statement(sqlite3*, const char*);
      ~statement();

      // helper
      bool step();
      bool step_row();
      const char* get_text(size_t);
      bool bind_text(size_t, const char*);
      void reset();
   private:
      std::string m_sql;
      sqlite3_stmt* m_stmt;
   };

   statement* create_statement(const char*);
   void remove_statement(statement*);
   bool execute(const char*);
   bool backup(const char*);
   bool attach(const char*, const char*);
   bool detach(const char*);
private:
   sqlite3* m_db;
   std::vector<statement*> m_stmt;
};

