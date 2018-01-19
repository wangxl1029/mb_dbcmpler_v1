// compiler.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
#include "stdafx.h"
#elif defined(__linux__)
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#else
#error "unsupport platform"
#endif
#include <string>
#include <vector>
#include <iostream>
#include "addon.h"
/*!
 *  \mainpage Add on DB compiler MAIN PAGE
 *  \subpage pg_srch
 *  \section sec_pla Working Platform 
 * The working platform is Linux
 *  \section sec_cmd command line option
 * \-m import mid file\n
 * \-d parse db file
 */
#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{
   return 0;
}
#elif defined(__linux__)
#define VERSION   "1.0.4"
#define AUTHOR    "WXL"
#define REVISION  "$Revision: 3623 $"
int main(int argc, char* argv[])
{
   int opt, optnum = 0;
   bool sth_done = false;
   while ((opt = getopt(argc, argv, "vh")) != -1) 
   {
      switch (opt) 
      {
         case 'v':
            ///< parse mid files
            optnum++;
            std::cout << "Addon DATA compiler." << std::endl;
            std::cout << "version : " << VERSION << std::endl;
            std::cout << std::endl;
            std::cout << "\t" << AUTHOR << " compiled at " << __TIME__ << " on " << __DATE__ << std::endl;
            sth_done = true;
            break;
         case 'h':
            //nsecs = atoi(optarg);
            optnum++;
            sth_done = true;
         default: /* '?' */
            //fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
            std::cout << "helper" << std::endl;
            exit(EXIT_FAILURE);
      }
   }

   if (0 == optnum) 
   {
      cm_argv(argc, argv);
   }
   else
   {
      if ( ! sth_done )
      {
         puts("!!!!!!!!!!!!");
      }
   }

	return 0;
}
#endif

