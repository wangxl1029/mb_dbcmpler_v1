#include <cstdlib>
#include "cm_db.hpp"
#include "cm_debug.h"

int cm_import_mid(const char* path)
{
	int retval = EXIT_SUCCESS;
	CCmDatabase db;
   db.import_mid(path); 

	return retval;
}

int cm_parse_db(const char* path)
{
	int retval = EXIT_SUCCESS;
	CCmDatabase db;
   db.parse_db(path); 

   return retval;
}

int cm_argv(int argc, char* argv[])
{
   int retval = EXIT_SUCCESS;
   std::vector<std::string> v;
   for(int i = 1; i < argc; i++)
   {
      v.push_back(argv[i]);
   }
   CCmDatabase db;
   db.do_argv(v);

   return retval;
}
