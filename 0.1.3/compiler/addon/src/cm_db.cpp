/*!
 *    \file  cm_db.cpp
 *   \brief  the database implement for compiler
 *  
 *  the class routine things
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
 * @page pg_srch Page for search target data
 * @brief This page introduce how to search target data such as CR, toll info or HW info.
 * @section sec1 reach the CR info
 * @subsection subsec_1_1 CR info binary format
 * @brief The CR info is up to 16 bytes.
 * @details The bit sequence is like following.
 * @verbatim
00~07 bytes: XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXXXX
             63             48 |  ||||| 39                                           0
             |               | |  ||||| |                                            |
             \---------------/ \  /|||| \------------------- CRID -------------------/
                  VPeriod16     \/ ||||
                                /  ||||
           VPeri_Type <--------/   ||||
                                   \/||
           VP_Approx <-------------/ ||
                                     \/
           VPDir <-------------------/

08~15 bytes: XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXXXX
             127                              96 95                                 64
             |                                 | |                                   |
             \------------- Vehcl_Type --------/ \------------- VPeriod32 -----------/
   @endverbatim
 *
 * @subsection subsec_1_2 field detail
 *
 * -# bit 0~39, 40 bits : CRID
 * -# bit 40~41, 2 bits : VPDir, indicate the restrict direction. The value range is [0, 3].\n
 *  - case value == 0 : Default value. The meaning is no direction.
 *  - case value == 1 : Restriction on the bidirections.
 *  - case value == 2 : Restriction on the order direction.
 *  - case value == 3 : Restriction on the reverse direction.
 * -# bit 42~43, 2 bits : VP_Approx, the value indacates the aproximation for VPeriod. The value ranges as [0, 2].
    - case value == 0 : Not available.
    - case value == 1 : Not the value approximated.
    - case value == 2 : The value is approximated.
    - case value == 3 : Not defined.
 * -# bit 44~47, 4 bits : VPeri_Type, value range [0,2].\n
 *  - case value == 0 : not available
 *  - case value == 1 : MMdd V.S. hhmm
 *  - case value == 2 : hhmm V.S. t1~t7
 *  - case other value: not defined
 * -# bit 48~95, 48 bits: VPeriod, the bit sequence rules the time of traffic regulation.
    - case VPeri_Type == 1 :
     - the part VPeriod16 : bit 48~63, 16 bits.
      -# bit 48~51, 4 bits, start month. The value ranges [0, 12].
       - case value == 0 : not available.
       - case value == 1..12 : Jan .. Dec.
       - case other value : not defined.
      -# bit 52~55, 4 bits, end month. The value ranges [0, 12].
       - case value == 0 : not available.
       - case value == 1..12 : Jan .. Dec.
       - case other value : not defined.
      -# bit 56~63, 8 bits, not defined.
     - the part VPeriod32 : bit 64~95, 32 bits.
      -# bit 64~68, 5 bits, start date. The value ranges [0, 31].
       - case value == 0 : not available.
       - case value == 1..31 : The day in a month.
      -# bit 69~73, 5 bits, end date. The value ranges [0, 31].
       - case value == 0 : not available.
       - case value == 1..31 : The day in a month.
      -# bit 74~78, 5 bits, start hour. The value ranges [0, 23].
       - case value == 0..23 : hour.
       - case the other value : not available.
      -# bit 79~83, 5 bits, end hour. The value ranges [0, 23].
       - case value == 0..23 : hour.
       - case the other value : not available.
      -# bit 84~89, 6 bits, start miniute. The value ranges [0, 59].
       - case value == 0..59 : minute.
       - case the other value : not available.
      -# bit 90~95, 6 bits, start miniute. The value ranges [0, 59].
       - case value == 0..59 : minute.
       - case the other value : not available.
    - case VPeri_Type == 2 :
     - the part VPeriod16 : bit 48~63, 16 bits. Not defined.
     - the part VPeriod32 : bit 64~95, 32 bits.
      -# bit 64~70, 7 bits, bit sequence. 
       - bit 0..6 : Sun, Mon..Sat.
      -# bit 71~73, 3 bits, not defined.
      -# bit 74~78, 5 bits, start hour. The value ranges [0, 23].
       - case value == 0..23 : hour.
       - case the other value : not available.
      -# bit 79~83, 5 bits, end hour. The value ranges [0, 23].
       - case value == 0..23 : hour.
       - case the other value : not available.
      -# bit 84~89, 6 bits, start miniute. The value ranges [0, 59].
       - case value == 0..59 : minute.
       - case the other value : not available.
      -# bit 90~95, 6 bits, start miniute. The value ranges [0, 59].
       - case value == 0..59 : minute.
       - case the other value : not available.

    - case VPeri_Type == 3 :
     - the part VPeriod16 : bit 48~63, 16 bits. Not defined.
     - the part VPeriod32 : bit 64~95, 32 bits.
      -# bit 64~73, 10 bits, not defined.
      -# bit 74~78, 5 bits, start hour. The value ranges [0, 23].
       - case value == 0..23 : hour.
       - case the other value : not available.
      -# bit 79~83, 5 bits, end hour. The value ranges [0, 23].
       - case value == 0..23 : hour.
       - case the other value : not available.
      -# bit 84~89, 6 bits, start miniute. The value ranges [0, 59].
       - case value == 0..59 : minute.
       - case the other value : not available.
      -# bit 90~95, 6 bits, start miniute. The value ranges [0, 59].
       - case value == 0..59 : minute.
       - case the other value : not available.
 * @section sec2 reach the Toll info
 * - The ETA info
 *   One toll ETA info is up to 8 bytes.
 *   The fields in one toll ETA info is like following.
 *  -# bit 00~39, 40 bits : condion ID.
 *  -# bit 40~43,  4 bits : toll type.
 *  -# bit 44~47,  4 bits : lane number.
 *  -# bit 48~63, 16 bits : not defined.
 * - The Pattern Info
 *   One toll pattern info is up to 16 bytes.
 *   The fields in one toll pattern info is like following.
 *  -# bit 00~39, 40 bits : condion ID.
 *  -# bit 40~63, 24 bits : not defined.
 *  -# bit 64~95, 32 bits : pattern number.
 *  -# bit 96~127, 32 bits : arrow nubmer.
 * @section sec3 reach the HW Junction info
 * @brief One HW Junction info is up to 24 bytes and has 3 parts. 
 *  The part 1 is 8 bytes. The part 2 is 6 bytes and the part 3 is 10 bytes.
 * @details the fields in these files are like following.
 * - part 1 : 8 bytes.
 *  -# bit 00~39, 40 bits : ID
 *  -# bit 40~47,  8 bits : not defined.
 *  -# bit 48~51,  4 bits : access type.
 *  -# bit 52~55,  4 bits : attribute.
 *  -# bit 56~63,  8 bits : estab item.
 *   - bit 56 : 1, restaurant. 0 , no restaurant available.
 *   - bit 57 : 1, shop. 0, no shop available.
 *   - bit 58 : 1, inn. 0, no inn available.
 *   - bit 59 : 1, public toilet. 0, no public toilet available.
 *   - bit 60~63 : Natural number, the gasoline station.
 *    - value == 21 : PetreChina.
 *    - value == 22 : sinopec
 *    - value == 23 : shell
 *    - value == 24 : Mobil
 *    - value == 25 : Brithish Petroleum
 *    - value == 26 : other
 * - part 2 : 6 bytes.
 *  bit 00~39, 40 bits : NodeID
 *  bit 40~63, 16 bits : not defined.
 * - part 3 : 10 bytes.
 *  -# byte 0~4 : inLinkID.
 *  -# byte 5~9 : outLinkID.
 */
//-----------------------------------------------------------------------------
//  Header Section
//-----------------------------------------------------------------------------
#include <fstream>
#include <functional>
#include <sstream>
#include <array>
#include <algorithm>
#include <regex>
#include <bitset>
#include <type_traits>
#include "cm_db.hpp"
#include "cm_debug.h"

//-----------------------------------------------------------------------------
//  MACRO Defination
//-----------------------------------------------------------------------------
#define LOG_HEADER "[CM_DB]"

#define TABLE_N            "N"
#define TABLE_C            "C"
#define TABLE_CR           "CR"
#define TABLE_C_CR         "C_CR"
#define TABLE_Toll_ETA     "Toll_ETA"
#define TABLE_Toll_Pattern "Toll_Pattern"
#define TABLE_HW_Junction  "HW_Junction"
#define PROVINCE_PTN       R"((beijing|liaoning|tianjin))"
#define MEM_DB             ":memory:"

//-----------------------------------------------------------------------------
//  Constants Defination
//-----------------------------------------------------------------------------
static const int VP_INVALID_HOUR = 24;
static const int VP_INVALID_MINUTE = 60;

//-----------------------------------------------------------------------------
//  Type Defination
//-----------------------------------------------------------------------------
union UINT16_bytes
{
   uint16_t val;
   char buf[sizeof(val)];
};
union UINT32_bytes
{
   uint32_t val;
   char buf[sizeof(val)];
};
union UINT64_bytes
{
   uint64_t val;
   char buf[sizeof(val)];
};

struct alignas(16) CR_RowData {
   uint64_t CRID : 40;              /* 5 bytes */
   uint32_t VPDir: 2;               /* 2/8 byte */
   uint32_t VP_Approx : 2;          /* 2/8 byte */
   uint32_t VPeri_Type : 4;         /* 4/8 byte */
   uint16_t VPeriod16;              /* 2 bytes */
   uint32_t VPeriod32;              /* 4 bytes */
   uint32_t Vehcl_Type;             /* 4 bytes */
} ;
struct alignas(8) TollETA_RowData{
   uint64_t CondID : 40;
   uint32_t TollType : 4;
   uint32_t lane_num : 4;
};
struct alignas(16) TollPattern_RowData{
   uint64_t CondID : 40;            /* 5 bytes */
   uint32_t : 24;                   /* 3 bytes : padding */
   uint32_t PatterNo;               /* 4 bytes */
   uint32_t ArrowNo;                /* 4 bytes */
};
//-----------------------------------------------------------------------------
//  Local Varibles Declaration
//-----------------------------------------------------------------------------
static const auto g_bname_ptn = std::make_tuple(
   std::regex("N" PROVINCE_PTN),
   std::regex("C" PROVINCE_PTN),
   std::regex("CR" PROVINCE_PTN),
   std::regex("Toll_ETA" PROVINCE_PTN),
   std::regex("Toll_Pattern" PROVINCE_PTN),
   std::regex(PROVINCE_PTN "_C_CR_Toll"),
   std::regex("HW_Junction")
);

static struct{
   std::regex type1;
   std::regex type2;
   std::regex type3;
   std::regex MonthDay;
   std::regex WeekDay;
   std::regex hour;
   std::regex HourMinute;
} g_VPeriadRegex = {
   std::regex(R"(\[\((.+)\)\((.+)\)\]\*\[\((.+)\)\((.+)\)\])"),
   std::regex("\\[\\((.+)\\)\\((.+)\\)\\]\\*\\((.+)\\)"),
   std::regex(R"(\[\((.+)\)\((.+)\)\])"),
   std::regex(R"(M(\d{1,2})d(\d{1,2}))"),
   std::regex(R"((t\d){1,7})"),
   std::regex(R"(h(\d{1,2}))"),
   std::regex(R"(h(\d{1,2})m(\d{1,2}))")
};
static bool g_isPlatformLittleEndian = []{UINT32_bytes u; u.val = 0x87654321; 
   return u.buf[0] < u.buf[1];}();
//-----------------------------------------------------------------------------
//  Local Utility
//-----------------------------------------------------------------------------
template<typename T>
T _LE(T n)
{
   static_assert(std::is_integral<T>::value, "_LE");
   if (  ! g_isPlatformLittleEndian ) {
      union{
         T val;
         char buf[sizeof(val)];
      }u;
      u.val = n;
      std::reverse(std::begin(u.buf), std::end(u.buf));
   }
   return n;
}

template<typename T>
inline void _bzero(T& r)
{
   auto addr = reinterpret_cast<char*>(&r);
   static_assert(std::is_pod<T>::value, "_bzero");
   std::fill_n(addr, sizeof(r), '\0');
}

static std::vector<std::string> _strdiv(const std::string& s, char delim)
{
   std::vector<std::string> v;
   std::string token;

   std::string::size_type find_start = 0;
   auto find_pos = s.find_first_of(delim, find_start);
   while ( std::string::npos != find_pos )
   {
      auto find_size = find_pos - find_start;

      token = s.substr(find_start, find_size);
      if ( ! token.empty() ) 
      {
         v.push_back(std::move(token));
      }

      find_start = find_pos + 1;
      find_pos = s.find_first_of(delim, find_start);
   }

   token = s.substr(find_start);
   if ( ! token.empty() ) 
   {
      v.push_back(std::move(token));
   }

   return v;
}

uint32_t _stou32(const std::string s, int base = 10)
{
   static_assert(sizeof(int) == 4, "_stou32");
   return static_cast<uint32_t>(stoi(s, 0, base));
}

uint64_t _stou64(const std::string s, int base = 10)
{
   static_assert(sizeof(long) == 8, "_stou64");
   return static_cast<uint64_t>(stoul(s, 0, base));
}

static bool _strip_quotes(std::string& token)
{
   bool isQuoted = true;
   char quote = '\"';

   auto first = token.find_first_of(quote);
   if(std::string::npos != first)
   {
      token.erase(0, first + 1);
   }
   else
   {
      isQuoted = false;
   }

   auto last = token.find_last_of(quote);
   if (std::string::npos != last)
   {
      token.erase(last);
   }
   else
   {
      isQuoted = false;
   }

   return isQuoted;
}

static void _strfit8bytes(std::string& s)
{
   auto modsiz = 8;
   auto modnum = s.size() % modsiz;
   if (0 != modnum) 
   {
      auto padnum = modsiz - modnum;
      s.append(padnum, '\0');
   }

}

static CR_RowData _CR_row2data(
   const std::string& txtCRID,
   const std::string& txtVPeriod,
   const std::string& txtVPDir,
   const std::string& txtVeh_Type,
   const std::string& txtVP_Appro
)
{
   CR_RowData  buf;
   auto& vp{g_VPeriadRegex}; 

   static_assert(sizeof(buf) == 16, "buffer is not 16 bytes;");

   //CM_LOG_INFO("%s CRID \"%s\".", LOG_HEADER, txtCRID.c_str());
   buf.CRID = _LE(_stou64(txtCRID)); 
   //CM_LOG_INFO("%s VPDir \"%s\".", LOG_HEADER, txtVPDir.c_str());
   buf.VPDir = _LE(_stou32(txtVPDir)); 
   //CM_LOG_INFO("%s VP_Approx \"%s\".", LOG_HEADER, txtVP_Appro.c_str());
   if ( ! txtVP_Appro.empty() ) 
   {
      buf.VP_Approx = _LE(_stou32(txtVP_Appro) + 1);
   }
   else 
   {
      buf.VP_Approx = 0;
   }

   //CM_LOG_INFO("%s Veh_Type \"%s\".", LOG_HEADER, txtVeh_Type.c_str());
   if ( ! txtVeh_Type.empty() ) 
   {
      auto type64 = _stou64(txtVeh_Type, 2);
      buf.Vehcl_Type = _LE(static_cast<uint32_t>(type64));
   }
   else 
   {
      buf.Vehcl_Type = 0;
   }

   //CM_LOG_INFO("%s VPeriaod \"%s\".", LOG_HEADER, txtVPeriod.c_str());
   if ( ! txtVPeriod.empty() ) 
   {
      std::smatch m;
      if ( std::regex_match(txtVPeriod, m, vp.type1) ) 
      {
         //CM_LOG_INFO("%s type 1 %s.", LOG_HEADER, txtVPeriod.c_str());
         if ( m.size() == 2 ) 
         {
            CM_LOG_INFO("%s %s , %s", LOG_HEADER, m[0].str().c_str(), m[1].str().c_str());
         }
         std::string Dt1, Dt2, t1, t2;
         std::tie(Dt1, Dt2, t1, t2) = std::make_tuple(m[1].str(), m[2].str(), m[3].str(), m[4].str());

         short int M1, M2;
         M1 = M2 = 0;                  /* Month */

         static_assert(sizeof(short int) == 2, "short type is not 2 bytes");

         int d1, d2, h1, h2, m1, m2;
         d1 = d2 = 0;                  /* the day in Month */
         h1 = h2 = VP_INVALID_HOUR;
         m1 = m2 = VP_INVALID_MINUTE;

         if ( std::regex_match(Dt1, m, vp.MonthDay) ) 
         {
            M1 = stoi(m[1].str());
            d1 = stoi(m[2].str());
         }

         if ( std::regex_match(Dt2, m, vp.MonthDay) ) 
         {
            M2 = stoi(m[1].str());
            d2 = stoi(m[2].str());
         }

         if ( std::regex_match(t1, m, vp.HourMinute) ) 
         {
            h1 = stoi(m[1].str());
            m1 = stoi(m[2].str());
         }
         else if ( std::regex_match(t1, m, vp.hour) ) 
         {
            h1 = stoi(m[1].str());
         }

         if ( std::regex_match(t2, m, vp.HourMinute) ) 
         {
            h2 = stoi(m[1].str());
            m2 = stoi(m[2].str());
         }
         else if ( std::regex_match(t2, m, vp.hour) ) 
         {
            h2 = stoi(m[1].str());
         }

         uint16_t peri16 = 0;
         peri16 |= M1;
         peri16 |= M2 << 4;

         uint32_t peri32 = 0;
         peri32 |= d1;
         peri32 |= d2 << 5;
         peri32 |= h1 << 10;
         peri32 |= h2 << 15;
         peri32 |= m1 << 20;
         peri32 |= m2 << 26;

         //CM_LOG_INFO("%s (%d,%d)                 ==> 0x%04x", LOG_HEADER, M1, M2, peri16);
         //CM_LOG_INFO("%s (%d,%d)-(%d,%d)-(%d,%d) ==> 0x%08x", LOG_HEADER, d1, d2, h1, m1, h2, m2, peri32);

         buf.VPeri_Type = 1;
         buf.VPeriod16  = _LE(peri16);
         buf.VPeriod32  = _LE(peri32);
      }
      else if ( std::regex_match(txtVPeriod, m, vp.type2) ) 
      {
         //CM_LOG_INFO("%s type 2, size %d, %s.", LOG_HEADER, m.size(), txtVPeriod.c_str());
         std::string t1, t2, weekday;
         std::tie(t1, t2, weekday) = std::make_tuple(m[1].str(), m[2].str(), m[3].str());

         int h1, h2, m1, m2;
         h1 = h2 = VP_INVALID_HOUR;
         m1 = m2 = VP_INVALID_MINUTE;

         if ( std::regex_match(t1, m, vp.hour) ) 
         {
            h1 = stoi(m[1].str());
         }
         else if ( std::regex_match(t1, m, vp.HourMinute) ) 
         {
            h1 = stoi(m[1].str());
            m1 = stoi(m[2].str());
         }

         if ( std::regex_match(t2, m, vp.hour) ) 
         {
            h2 = stoi(m[1].str());
         }
         else if ( std::regex_match(t2, m, vp.HourMinute) ) 
         {
            h2 = stoi(m[1].str());
            m2 = stoi(m[2].str());
         }

         char wd = 0x00;
         if ( std::regex_match(weekday, m, vp.WeekDay) ) 
         {
            for(auto i = 1; i < weekday.size(); i += 2 )
            {
               switch(weekday[i])
               {
                  case '1':
                     wd |= 0x01;
                     break;

                  case '2':
                     wd |= 0x02;
                     break;

                  case '3':
                     wd |= 0x04;
                     break;

                  case '4':
                     wd |= 0x08;
                     break;

                  case '5':
                     wd |= 0x10;
                     break;

                  case '6':
                     wd |= 0x20;
                     break;

                  case '7':
                     wd |= 0x40;
                     break;

                  default:
                     CM_LOG_INFO("%s unexpected %c.", LOG_HEADER, weekday[i]);
               }
            }
         }

         uint32_t peri32 = 0;
         peri32 |= wd;
         peri32 |= h1 << 10;
         peri32 |= h2 << 15;
         peri32 |= m1 << 20;
         peri32 |= m2 << 26;

         //CM_LOG_INFO("%s (%d,%d)-(%d,%d)-(0x%02x) ==> 0x%08x", LOG_HEADER, h1, m1, h2, m2, wd, peri32);

         buf.VPeri_Type = 2;
         buf.VPeriod16  = 0;
         buf.VPeriod32  = _LE(peri32);
      }
      else if( std::regex_match(txtVPeriod, m, vp.type3) )
      {
         //CM_LOG_INFO("%s type 3 %s.", LOG_HEADER, txtVPeriod.c_str());
         std::string t1, t2;
         std::tie(t1, t2) = std::make_pair(m[1].str(), m[2].str());

         int h1, h2, m1, m2;
         h1 = h2 = VP_INVALID_HOUR;
         m1 = m2 = VP_INVALID_MINUTE;

         if ( std::regex_match(t1, m, vp.hour) ) 
         {
            h1 = stoi(m[1].str());
         }
         else if ( std::regex_match(t1, m, vp.HourMinute) ) 
         {
            h1 = stoi(m[1].str());
            m1 = stoi(m[2].str());
         }

         if ( std::regex_match(t2, m, vp.hour) ) 
         {
            h2 = stoi(m[1].str());
         }
         else if ( std::regex_match(t2, m, vp.HourMinute) ) 
         {
            h2 = stoi(m[1].str());
            m2 = stoi(m[2].str());
         }

         uint32_t peri32 = 0;
         peri32 |= h1 << 10;
         peri32 |= h2 << 15;
         peri32 |= m1 << 20;
         peri32 |= m2 << 26;

         //CM_LOG_INFO("%s (%d,%d)-(%d,%d) ==> 0x%08x", LOG_HEADER, h1, m1, h2, m2, peri32);

         buf.VPeri_Type = 3;
         buf.VPeriod16  = 0;
         buf.VPeriod32  = _LE(peri32);
      }
      else
      {
         CM_LOG_INFO("%s type 4 %s.", LOG_HEADER, txtVPeriod.c_str());
      }
   }
   else 
   {
      buf.VPeri_Type = 0;
   }
   return buf;
}

std::pair<TollETA_RowData, std::string> _TollETA_row2data(
      const std::string& txtCondID, 
      const std::string& txtTollMode,
      const std::string& txtTollCard,
      const std::string& txtTollType
)
{
   TollETA_RowData buf;
   std::string extbuf;

   static_assert(sizeof(buf) == 8, "buffer is not 8 bytes;");

   buf.CondID = _LE(_stou64(txtCondID)); 
   buf.TollType = _stou32(txtTollType);
   char delim = '|';
   std::string& TollMode = extbuf;
   std::string& TollCard = extbuf;
   if ( ! txtTollMode.empty() ) 
   {
      auto v = _strdiv(txtTollMode, delim);
      for( auto & e : v)
      {
         TollMode.push_back(static_cast<char>(stoi(e, nullptr, 2)));
      }
   }

   if ( ! txtTollCard.empty() ) 
   {
      auto v = _strdiv(txtTollCard, delim);
      for( auto & e : v)
      {
         TollCard.push_back(static_cast<char>(stoi(e)));
      }
   }


   if( ! TollMode.empty())
   {
      buf.lane_num = TollMode.size();
      //CM_LOG_INFO("%s type %d, lane number %d", LOG_HEADER, buf.TollType, buf.lane_num);
      _strfit8bytes(TollMode);
   }
   else if( ! TollCard.empty())
   {
      buf.lane_num = TollCard.size();
      //CM_LOG_INFO("%s type %d, lane number %d", LOG_HEADER, buf.TollType, buf.lane_num);
      _strfit8bytes(TollCard);
   }
   else
   {
      CM_LOG_WARNING("%s ETA record is something empty!", LOG_HEADER);
   }

   return std::make_pair(std::ref(buf), std::ref(extbuf));;
}

TollPattern_RowData _TollPattern_row2data(
      const std::string& txtCondID,
      const std::string& txtPaternNo,
      const std::string& txtArrowNo 
)
{
   TollPattern_RowData buf;

   static_assert(sizeof(buf) == 16, "buffer is not 16 bytes;");

   buf.CondID = _LE(_stou64(txtCondID)); 
   auto ptnNo = txtPaternNo;
   ptnNo.replace(0,1, "0x");
   buf.PatterNo = _LE(_stou32(ptnNo, 16)); 
   auto arrowNo = txtArrowNo;
   arrowNo.replace(0, 1, "0x");
   buf.ArrowNo = _LE(_stou32(arrowNo, 16)); 

   return buf;
}

bool CCmDatabase::isLeadSameIcStr(const std::string& src, const std::string& dst, std::string::size_type n)
{
   auto m = std::min(dst.length(), src.length());
   if ( std::string::npos == n ) 
   {
      n = m;
   }
   else
   {
      n = std::min(n, m);
   }
   
   bool is_same = true;
   for ( decltype(n) i = 0; i < n; ++i ) 
   {
      auto& a = src[i];
      auto& b = dst[i];
      if ( a != b ) 
      {
         if(std::isalpha(a, m_loc) && std::isalpha(b, m_loc))
         {
            if(std::tolower(a, m_loc) != std::tolower(b, m_loc))
            {
               is_same = false;
               break;
            }
         }
         else
         {
            is_same = false;
            break;
         }
      }
   }

   //CM_LOG_INFO("%s %s , %s is %s", LOG_HEADER, src.c_str(), dst.c_str(), is_same ? "same": "not same");
   return is_same;
}
void CCmDatabase::fit_to_graph(std::string& s)
{
   auto pred = [this](const char& c){return std::isgraph(c, this->m_loc);};
   auto it = std::find_if(s.begin(), s.end(), pred);
   if(s.end() != it)
   {
      ///< remove leading white space
      std::string::size_type num = std::distance(s.begin(), std::next(it));
      s.erase(0, num);
   }

   auto rit = std::find_if(s.rbegin(), s.rend(), pred);
   if ( s.rend() != rit ) 
   {
      ///< erase ending white space
      std::string::size_type num = std::distance(s.rbegin(), std::next(rit));
      auto pos = s.size() - num;
      s.erase(pos);
   }
}

std::tuple<std::string, std::string, std::string> CCmDatabase::parse_path(const std::string& path)
{
   std::string dir, basename, ext;
   std::string filename = path;

   auto pos  = path.find_last_of('/');
   if ( std::string::npos != pos ) 
   {
      dir = path.substr(0, pos);
      filename = path.substr(pos + 1);
   }

   basename = filename;
   pos = filename.find_last_of('.');
   if ( std::string::npos != pos ) 
   {
      ext = filename.substr(pos + 1);
      basename = filename.substr(0, pos);
   }

   return std::make_tuple(std::ref(dir), std::ref(basename), std::ref(ext));
}
//-----------------------------------------------------------------------------
//  Implement Section For class CCmDatabase
//-----------------------------------------------------------------------------
CCmDatabase::CCmDatabase()
: m_db(nullptr)
, m_loc("")
, m_stmtSelectCR(nullptr)
, m_stmtSelectTollETA(nullptr)
, m_stmtSelectTollPatern(nullptr)
, m_stmtSelectHWJunction(nullptr)
{
}

CCmDatabase::~CCmDatabase()
{
   if ( m_db ) 
   {
      delete m_db;
   }
}

/*!
 *  \brief  import "*.mid" files
 * \param path the mid file path.\n
 * The mid file base name should be leading by Toll_ETA, Toll_Patern
 * \return import OK or not 
 * \retval true import OK
 * \retval false import failed
 *
 */
bool CCmDatabase::import_mid(const char* path)
{
   bool ok = true;

   if ( path ) 
   {
      std::string dir, basename, ext;
      auto result = parse_path(std::string(path));
      std::tie(dir, basename, ext) = result;

      CM_LOG_INFO("%s dir \"%s\", basename \"%s\", ext \"%s\".", LOG_HEADER, dir.c_str(), basename.c_str(), ext.c_str());
      if ( ! basename.empty() && ext == "mid") 
      {
         bool is_opened = open_db(MEM_DB);
         if ( is_opened ) 
         {
            std::regex ptn_N, ptn_C, ptn_CR, ptn_ETA, ptn_Pattern, ptn_C_CR_Toll, ptn_Junction;
            std::tie(ptn_N, ptn_C, ptn_CR, ptn_ETA, ptn_Pattern, ptn_C_CR_Toll, ptn_Junction) = g_bname_ptn;
            if( std::regex_match(basename, ptn_N))
            {
               is_opened = open_mid_N(path);
            }
            else if( std::regex_match(basename, ptn_C))
            {
               is_opened = open_mid_C(path);
            }
            else if( std::regex_match(basename, ptn_CR))
            {
               is_opened = open_mid_CR(path);
            }
            else if( std::regex_match(basename, ptn_ETA))
            {
               is_opened = open_mid_Toll_ETA(path);
            }
            else if( std::regex_match(basename, ptn_Pattern))
            {
               is_opened = open_mid_Toll_Pattern(path);
            }
            else if( std::regex_match(basename, ptn_Junction))
            {
               is_opened = open_mid_HW_Junction(path);
            }
            else
            {
               CM_LOG_WARNING("%s the \"%s\" don't match any targets!", LOG_HEADER, basename.c_str());
               is_opened = false;
            }
         }

         if(is_opened)
         {
            std::string db_path = basename + '.' + "db";
            if( ! dir.empty())
            {
               db_path = dir + '/' + db_path;
            }
            ok = save_as(db_path.c_str());
            if(!ok)
            {
               CM_LOG_WARNING("%s save as \"%s\" failed!", LOG_HEADER, db_path.c_str());
            }
         }
      }
      else
      {
         CM_LOG_WARNING("%s inpute path is not illegal : basename \"%s\", ext \"%s\".", LOG_HEADER, basename.c_str(), ext.c_str());
      }
   }
   else 
   {
      CM_LOG_ERROR("%s the input path is NULL!", LOG_HEADER);
      ok = false;
   }

   return ok;
}

bool CCmDatabase::open_mid(const char* path, const size_t fldnum, const char* sqlins)
{
   bool ok = true;

   if ( nullptr != path && nullptr != sqlins && fldnum > 0) 
   {
      CM_LOG_INFO("%s open \"%s\", field number : %d, insert SQL is \"%s\".", LOG_HEADER, path, fldnum, sqlins);
      auto stmt = m_db->create_statement(sqlins);
      if (stmt)
      {
         std::ifstream mid(path);
         if (mid.is_open())
         {
            size_t lineno = 0;
            std::string linebuf;
            std::string field[fldnum];

            while (getline(mid, linebuf))
            {
               size_t tkn_num = 0;
               std::string token;
               std::istringstream is(linebuf);
               while (std::getline(is, token, ','))
               {

                  size_t fld_idx = tkn_num++;
                  if (fld_idx < fldnum)
                  {
                     _strip_quotes(token);
                     // NOTE : the the fields to bind have to be the same life cycle with the statement step.
                     field[fld_idx] = std::move(token);
                     stmt->bind_text(tkn_num, field[fld_idx].c_str());
                  }
                  else
                  {
                     CM_LOG_WARNING("%s field number(%d) exceeded!", LOG_HEADER, tkn_num);
                     break;
                  }
               }
            
               if ( tkn_num > 0 ) 
               {
                  stmt->step();
                  stmt->reset();
               }

               decltype(lineno) print_step = 1000;
               if ( 0 == lineno++ % print_step ) 
               {
                  CM_LOG_INFO("%s ====> line NO:%d", LOG_HEADER, lineno);
               }
            }
            CM_LOG_INFO("%s ====>last line NO:%d", LOG_HEADER, ++lineno);
         }
         else
         {
            CM_LOG_ERROR("%s open %s failed!", LOG_HEADER, path);
            ok = false;
         }
      }

   }
   else 
   {
      CM_LOG_ERROR("%s Illegal parameters : open path is %p, field number is %d, insert SQL is %d." LOG_HEADER, path, fldnum, sqlins);
   }
   return ok;
}

bool CCmDatabase::open_mid_N(const char* path)
{
   static const size_t FIELD_NUM = 13;
   std::string sql = "create table if not exists " 
      TABLE_N
      "("
         "MapID text not null,"
         "ID text not null,"
         "Kind_num text not null,"
         "Kind text not null,"
         "Cross_flag text not null,"
         "Light_flag text not null,"
         "Cross_LID text not null,"
         "mainNodeID text not null,"
         "subNodeID text not null,"
         "subNodeID2 text not null,"
         "Adjoin_MID text not null,"
         "Adjoin_NID text not null,"
         "Node_LID text not null"
      ");";

   bool ok = m_db->execute( sql.c_str() );
   if (ok)
   {
      sql = "insert into " TABLE_N " values(?,?,?,?,?,?,?,?,?,?,?,?,?)";
      ok = open_mid(path, FIELD_NUM, sql.c_str());
   }
   else
   {
      CM_LOG_ERROR("%s create table failed!", LOG_HEADER);
   }

   return ok;
}

bool CCmDatabase::open_mid_C(const char* path)
{
   static const size_t FIELD_NUM = 10;
   std::string sql = "create table if not exists " 
      TABLE_C
      "("
         "MapID text not null,"
         "CondID text not null,"
         "ID text not null,"
         "inLinkId text not null,"
         "outLinkId text not null,"
         "CondType text not null,"
         "CRID text not null,"
         "Passage text not null,"
         "Slope text not null,"
         "SGNL_LOCTION text not null"
      ");";

   bool ok = m_db->execute( sql.c_str() );
   if (ok)
   {
      sql = "insert into " TABLE_C " values(?,?,?,?,?,?,?,?,?,?)";
      ok = open_mid(path, FIELD_NUM, sql.c_str());
   }
   else
   {
      CM_LOG_ERROR("%s create table failed!", LOG_HEADER);
   }

   return ok;
}

bool CCmDatabase::open_mid_CR(const char* path)
{
   static const size_t FIELD_NUM = 5;
   std::string sql = "create table if not exists " 
      TABLE_CR
      "("
         "CRID text not null,"               // CRID field is not set as primary key, because is could be multiple
         "VPeriod text not null,"
         "VPDir text not null,"
         "Vehcl_Type text not null,"
         "VP_Approx text not null"
      ");";

   bool ok = m_db->execute( sql.c_str() );
   if (ok)
   {
      sql = "insert into " TABLE_CR " values(?,?,?,?,?)";
      ok = open_mid(path, FIELD_NUM, sql.c_str());
   }
   else
   {
      CM_LOG_ERROR("%s create table failed!", LOG_HEADER);
   }

   return ok;
}

bool CCmDatabase::open_mid_Toll_ETA(const char* path)
{
   static const size_t FIELD_NUM = 4;
   std::string sql = "create table if not exists " 
      TABLE_Toll_ETA
      "("
         "CondId text not null primary key,"
         "TollMode text not null,"
         "CardMode text not null,"
         "TollType text not null"
      ");"; 

   bool ok = m_db->execute( sql.c_str() );
   if (ok)
   {
      sql = "insert into " TABLE_Toll_ETA " values(?,?,?,?)";
      ok = open_mid(path, FIELD_NUM, sql.c_str());
   }
   else
   {
      CM_LOG_ERROR("%s Toll_ETA path is NULL!", LOG_HEADER);
   }

   return ok;
}

bool CCmDatabase::open_mid_Toll_Pattern(const char* path)
{
   static const size_t FIELD_NUM = 3;
   std::string sql = "create table if not exists " 
      TABLE_Toll_Pattern
      "("
         "CondId text not null primary key,"
         "Pattern text not null,"
         "ArrowNo text not null"
      ");";

   bool ok = m_db->execute( sql.c_str() );
   if (ok)
   {
      sql = "insert into " TABLE_Toll_Pattern " values(?,?,?)";
      ok = open_mid(path, FIELD_NUM, sql.c_str());
   }
   else
   {
      CM_LOG_ERROR("%s Toll_Pattern path is NULL!", LOG_HEADER);
   }

   return ok;
}

bool CCmDatabase::open_mid_HW_Junction(const char* path)
{   
   static const size_t FIELD_NUM = 11;
   std::string sql = "create table if not exists "
      TABLE_HW_Junction
      "("
         "MapID text not null,"              /* #1 */
         "ID text not null,"
         "NodeID text not null,"
         "inLinkID text not null,"
         "outLinkID text not null,"
         "AccessType text not null,"         /* #6 */
         "Attr text not null,"
         "Dis_Betw text not null,"
         "Seq_Nm text not null,"
         "HW_PID text not null,"
         "Estab_Item text not null"          /* #11 */
      ");";

   bool ok = m_db->execute(sql.c_str());
   if (ok)
   {
      sql = "insert into " TABLE_HW_Junction " values(?,?,?,?,?,?,?,?,?,?,?)";
      ok = open_mid(path, FIELD_NUM, sql.c_str());
   }
   else
   {
      CM_LOG_ERROR("%s create table \"%s\" failed!" LOG_HEADER, sql.c_str());
   }

   return ok;

}

bool CCmDatabase::save_as(const char* path)
{
   return m_db->backup(path);
}

bool CCmDatabase::open_db(const char* path)
{
   bool ok = false;
   if (path && nullptr == m_db)
   {
      m_db = new CCmSqlite(path);
      ok = true;
   }
   else
   {
      CM_LOG_ERROR("%s DB path : \"%s\", DB pointer %p!", LOG_HEADER, path, m_db);
   }

   return ok;
}

bool CCmDatabase::parse_db(const char* path)
{
   bool ok = true;

   if ( path ) 
   {
      std::string dir, basename, ext;
      std::tie(dir, basename, ext) = parse_path(std::string(path));

      CM_LOG_INFO("%s dir \"%s\", basename \"%s\", ext \"%s\".", LOG_HEADER, dir.c_str(), basename.c_str(), ext.c_str());
      if ( ! basename.empty() && ext == "db") 
      {
         auto bin_path = [&](){
            std::string bp = basename + '.' + "bin";
            if( ! dir.empty())
            {
               bp = dir + '/' + bp;
            }
            return bp;
         };

         std::regex ptn_N, ptn_C, ptn_CR, ptn_ETA, ptn_Pattern, ptn_C_CR_Toll, ptn_Junction;
         std::tie(ptn_N, ptn_C, ptn_CR, ptn_ETA, ptn_Pattern, ptn_C_CR_Toll, ptn_Junction) = g_bname_ptn;

         bool is_opened = false;
         if( std::regex_match(basename, ptn_CR))
         {
            is_opened = open_db(path);
            if ( is_opened ) 
            {
               ok = parse_db_CR(bin_path().c_str());
            }
         }
         else if( std::regex_match(basename, ptn_ETA))
         {
            is_opened = open_db(path);
            if ( is_opened ) 
            {
               ok = parse_db_Toll_ETA(bin_path().c_str());
            }
         }
         else if( std::regex_match(basename, ptn_Pattern))
         {
            is_opened = open_db(path);
            if ( is_opened ) 
            {
               ok = parse_db_Toll_Pattern(bin_path().c_str());
            }
         }
         else if( std::regex_match(basename, ptn_C_CR_Toll))
         {
            is_opened = open_db(path);
            if ( is_opened ) 
            {
               ok = parse_db_C_CR_Toll(bin_path().c_str());
            }
         }
         else if( std::regex_match(basename, ptn_Junction))
         {
            is_opened = open_db(path);
            if ( is_opened ) 
            {
               ok = parse_db_HW_Junction(bin_path().c_str());
            }
         }
         else
         {
            CM_LOG_WARNING("%s the \"%s\" don't match any targets!", LOG_HEADER, basename.c_str());
            is_opened = false;
         }

      }
      else
      {
         CM_LOG_WARNING("%s inpute path is not illegal : basename \"%s\", ext \"%s\".", LOG_HEADER, basename.c_str(), ext.c_str());
      }
   }
   else 
   {
      CM_LOG_ERROR("%s the input path is NULL!", LOG_HEADER);
      ok = false;
   }

   return ok;
}

bool CCmDatabase::parse_db_CR(const char* bin_path)
{
   bool ok = false;

   if ( nullptr == m_stmtSelectTollETA ) 
   {
      std::string sql = "select * from " TABLE_CR ";";
      m_stmtSelectCR = m_db->create_statement(sql.c_str());
   }

   if ( nullptr != m_stmtSelectCR && nullptr != bin_path ) 
   {
      std::ofstream ofs(bin_path);
      if ( ofs.is_open() ) 
      {
         CM_LOG_INFO("%s %s open OK.", LOG_HEADER, bin_path);

         auto& vp    = g_VPeriadRegex; 
         auto& sel   = m_stmtSelectCR;

         while(sel->step_row())
         {
            size_t fld_pos = 0;
            std::string txtCRID     = sel->get_text(fld_pos++);
            std::string txtVPeriod  = sel->get_text(fld_pos++);
            std::string txtVPDir    = sel->get_text(fld_pos++);
            std::string txtVeh_Type = sel->get_text(fld_pos++);
            std::string txtVP_Appro = sel->get_text(fld_pos++);

            auto buf = _CR_row2data(txtCRID, txtVPeriod, txtVPDir, txtVeh_Type, txtVP_Appro);
            ofs.write(reinterpret_cast<const char*>(&buf), sizeof(buf));
         }

         sel->reset();
         ok = true;
      }
   }

   return ok;
}

bool CCmDatabase::parse_db_Toll_ETA(const char* bin_path)
{
   bool ok = false;

   if ( nullptr == m_stmtSelectTollETA ) 
   {
      std::string sql = "select * from " TABLE_Toll_ETA ";";
      m_stmtSelectTollETA = m_db->create_statement(sql.c_str());
   }

   if ( nullptr != m_stmtSelectTollETA && nullptr != bin_path ) 
   {
      std::ofstream ofs(bin_path);
      if ( ofs.is_open() ) 
      {
         CM_LOG_INFO("%s %s open OK.", LOG_HEADER, bin_path);
         auto& sel = m_stmtSelectTollETA;
         while(sel->step_row())
         {
            size_t fld_pos = 0;
            std::string txtCondID   = sel->get_text(fld_pos++);
            std::string txtTollMode = sel->get_text(fld_pos++);
            std::string txtTollCard = sel->get_text(fld_pos++);
            std::string txtTollType = sel->get_text(fld_pos++);

            TollETA_RowData buf;
            std::string extbuf;
            std::tie(buf, extbuf) = _TollETA_row2data(txtCondID, txtTollMode, txtTollCard, txtTollType);

            ofs.write(reinterpret_cast<const char*>(&buf), sizeof(buf));
            ofs.write(extbuf.data(), extbuf.size());
         }

         sel->reset();
         ok = true;
      }
   }

   return ok;
}

bool CCmDatabase::parse_db_Toll_Pattern(const char* bin_path)
{
   bool ok = false;

   if ( nullptr == m_stmtSelectTollPatern ) 
   {
      std::string sql = "select * from " TABLE_Toll_Pattern ";";
      m_stmtSelectTollPatern = m_db->create_statement(sql.c_str());
   }

   if ( nullptr != m_stmtSelectTollPatern && nullptr != bin_path ) 
   {
      std::ofstream ofs(bin_path);
      if ( ofs.is_open() ) 
      {
         CM_LOG_INFO("%s %s open OK.", LOG_HEADER, bin_path);
         auto& sel = m_stmtSelectTollPatern;
         while(sel->step_row())
         {
            size_t fld_pos = 0;
            std::string txtCondID   = sel->get_text(fld_pos++);
            std::string txtPaternNo = sel->get_text(fld_pos++);
            std::string txtArrowNo  = sel->get_text(fld_pos++);

            auto buf = _TollPattern_row2data(txtCondID, txtPaternNo, txtArrowNo);

            ofs.write(reinterpret_cast<const char*>(&buf), sizeof(buf));
         }

         ok = true;
         sel->reset();
      }

   }
   return ok;
}

/*!
 *  \brief  parse DB file for HighWay Junction
 */
bool CCmDatabase::parse_db_HW_Junction(const char* bin_path)
{
   bool ok = false;

   if ( nullptr == m_stmtSelectHWJunction ) 
   {
      std::string sql = "select * from " TABLE_HW_Junction ";";
      m_stmtSelectHWJunction = m_db->create_statement(sql.c_str());
   }

   if ( nullptr != m_stmtSelectHWJunction && nullptr != bin_path ) 
   {
      std::ofstream ofs(bin_path);
      if ( ofs.is_open() ) 
      {
         CM_LOG_INFO("%s %s open OK.", LOG_HEADER, bin_path);
         auto& sel = m_stmtSelectHWJunction;
         while(sel->step_row())
         {
            size_t fld_pos = 0;
            std::string txtMapID       = sel->get_text(fld_pos++);
            std::string txtID          = sel->get_text(fld_pos++);
            std::string txtNodeID      = sel->get_text(fld_pos++);
            std::string txtinLinkID    = sel->get_text(fld_pos++);
            std::string txtoutLinkID   = sel->get_text(fld_pos++);
            std::string txtAccessType  = sel->get_text(fld_pos++);
            std::string txtAttr        = sel->get_text(fld_pos++);
            std::string txtDis_Betw    = sel->get_text(fld_pos++);
            std::string txtSeq_Nm      = sel->get_text(fld_pos++);
            std::string txtHW_PID      = sel->get_text(fld_pos++);
            std::string txtEst_Item    = sel->get_text(fld_pos++);

            struct{
               uint64_t ID : 40;                /* 5 bytes */
               uint32_t : 8;                    /* 1 byte : padding */
               int32_t AccessType : 4;          /* 0.5 byte */
               int32_t Attr : 4;                /* 0.5 byte */
               uint32_t Estab_item : 8;         /* 1 byte */
            }buf1;

            static_assert(sizeof(buf1) == 8, "buffer is not 8 bytes;");

            buf1.ID = _LE(_stou64(txtID)); 
            buf1.AccessType = _LE(_stou32(txtAccessType)); 
            buf1.Attr = _LE(_stou32(txtAttr)); 
            buf1.Estab_item = 0;
            char delim = '|';
            if ( ! txtEst_Item.empty() ) 
            {
               unsigned char b = 0;
               unsigned char gaso = 0;
               auto v = _strdiv(txtEst_Item, delim);
               for( auto & e : v)
               {
                  auto item = stoi(e);
                  switch(item)
                  {
                     case 1:
                     b |= 0x01;                 /* restaurant */
                     break;

                     case 2:
                     b |= 0x02;                 /* shop */
                     break;

                     case 3:
                     b |= 0x04;                 /* inn */
                     break;

                     case 4:
                     b |= 0x08;                 /* pub toilet */
                     break;

                     case 21:
                     gaso = 1;                  /* PetreChina */
                     break;

                     case 22:
                     gaso = 2;                  /* sinopec */
                     break;

                     case 23:
                     gaso = 3;                  /* shell */
                     break;

                     case 24:
                     gaso = 4;                  /* Mobil */
                     break;

                     case 25:
                     gaso = 5;                  /* British Petroleum */
                     break;

                     case 26:
                     gaso = 0x0f;               /* Other */
                     break;

                     default:
                     CM_LOG_WARNING("%s not expect the Estab_item %d", LOG_HEADER, item);
                  }
               }

               if ( b ) 
               {
                  buf1.Estab_item |= b;
               }
               if ( gaso ) 
               {
                  buf1.Estab_item |= gaso << 4;
               }
            }

            ofs.write(reinterpret_cast<const char*>(&buf1), sizeof(buf1));

            struct{
               uint64_t NodeID : 40;
               char inLinkID[3];
            } buf2;

            buf2.NodeID = _LE(_stou64(txtNodeID));

            union {
               uint64_t inLinkID;
               char buf[sizeof(inLinkID)];
            } u;
   
            static_assert(sizeof(buf2) == 8, "buf2 is not 8 bytes!");
            u.inLinkID = _LE(_stou64(txtinLinkID));
            std::copy_n(u.buf, 3, buf2.inLinkID);

            ofs.write(reinterpret_cast<const char*>(&buf2), sizeof(buf2));

            struct{
               char inLinkID[2];
               uint64_t outLinkID : 40;
            } buf3;

            static_assert(sizeof(buf3) == 8, "buf3 is not 8 bytes!");
            std::copy_n(u.buf + 3, 2, buf3.inLinkID);
            buf3.outLinkID = _LE(_stou64(txtoutLinkID));

            ofs.write(reinterpret_cast<const char*>(&buf3), sizeof(buf3));
         }

         ok = true;
         sel->reset();
      }
   }

   return ok;
}

std::string CCmDatabase::parse_db_C_CR_Toll()
{
   std::string bin;
   std::string sql = R"(select * from C where CondID != "" or CRID != "";)";
   auto stmt_sel_C = m_db->create_statement(sql.c_str());
   if ( stmt_sel_C ) 
   {
      uint32_t row_num = 0;
      std::ostringstream os;

      decltype(stmt_sel_C) stmt_sel_CR, stmt_sel_TollETA, stmt_sel_TollPattern;
      stmt_sel_CR = stmt_sel_TollETA = stmt_sel_TollPattern = nullptr;
      while(stmt_sel_C->step_row())
      {
         size_t fld_pos = 0;
         std::string txtMapID          = stmt_sel_C->get_text(fld_pos++);
         std::string txtCondId         = stmt_sel_C->get_text(fld_pos++);
         std::string txtID             = stmt_sel_C->get_text(fld_pos++);
         std::string txtInLinkId       = stmt_sel_C->get_text(fld_pos++);
         std::string txtOutLinkId      = stmt_sel_C->get_text(fld_pos++);
         std::string txtCondType       = stmt_sel_C->get_text(fld_pos++);
         std::string txtCRID           = stmt_sel_C->get_text(fld_pos++);
         std::string txtPassage        = stmt_sel_C->get_text(fld_pos++);
         std::string txtSlope          = stmt_sel_C->get_text(fld_pos++);
         std::string txtSGNL_LOCTION   = stmt_sel_C->get_text(fld_pos++);

         struct alignas(16) {
            uint64_t InLinkId : 40;             /* 5 bytes */
            char     OutLinkId [5];             /* 5 bytes */
            uint8_t  CondType : 4;              // half byte
            uint32_t cnt_CRID : 4;              // half byte
            uint8_t  ETA_flag : 1;              /* 1/8 byte */
            uint8_t  ptn_flag : 1;              /* 1/8 byte */
         }rec_header;

         static_assert(sizeof(rec_header) == 16, "The buffer for C table is not 16 bytes;");

         _bzero(rec_header);
         //CM_LOG_INFO("%s[C] CondType %s.", LOG_HEADER, txtCondType.c_str());
         // condition type
         rec_header.CondType = _LE(_stou32(txtCondType) - 1);
         //CM_LOG_INFO("%s[C] inlink ID %s.", LOG_HEADER, txtInLinkId.c_str());
         // inlink ID
         if ( ! txtInLinkId.empty() ) {
            rec_header.InLinkId = _LE(_stou64(txtInLinkId));
         }
         else {
            rec_header.InLinkId = 0;
         }
         // outlink ID
         if ( ! txtOutLinkId.empty() ) {
            UINT64_bytes OutLinkId;
            {
               //CM_LOG_INFO("%s[C] outlink ID %s.", LOG_HEADER, txtOutLinkId.c_str());
               OutLinkId.val = _LE(_stou64(txtOutLinkId));
               std::copy_n(OutLinkId.buf, sizeof(rec_header.OutLinkId), rec_header.OutLinkId);
            }
         }
         else {
            std::fill(std::begin(rec_header.OutLinkId), std::end(rec_header.OutLinkId), 0); 
         }

         const size_t max_uint4bits = 15;

         // CRID
         rec_header.cnt_CRID = 0;

         struct alignas(16){
            uint32_t VPDir: 2;               /* 2/8 byte */
            uint32_t VP_Approx : 2;          /* 2/8 byte */
            uint32_t VPeri_Type : 4;         /* 4/8 byte */
            uint8_t  reserved[5];      // 5 bytes
            uint16_t VPeriod16;              /* 2 bytes */
            uint32_t VPeriod32;              /* 4 bytes */
            uint32_t Vehcl_Type;             /* 4 bytes */
         } buf_CR;
         
         static_assert(sizeof(buf_CR) == 16, "buffer CR is not 16 bytes");

         _bzero(buf_CR);
         std::vector<decltype(buf_CR)> vec_CR;
         if( ! txtCRID.empty())
         {
            // statment create/reset
            if ( stmt_sel_CR ) 
            {
               stmt_sel_CR->reset();
            }
            else
            {
               sql = "select * from " TABLE_CR " where CRID == ?;";
               stmt_sel_CR = m_db->create_statement(sql.c_str());
            }

            // loop the statement
            if ( stmt_sel_CR ) 
            {
               stmt_sel_CR->bind_text(1, txtCRID.c_str());
               while(stmt_sel_CR->step_row())
               {
                  size_t fld_pos = 0;
                  std::string txtCRID     = stmt_sel_CR->get_text(fld_pos++);
                  std::string txtVPeriod  = stmt_sel_CR->get_text(fld_pos++);
                  std::string txtVPDir    = stmt_sel_CR->get_text(fld_pos++);
                  std::string txtVeh_Type = stmt_sel_CR->get_text(fld_pos++);
                  std::string txtVP_Appro = stmt_sel_CR->get_text(fld_pos++);

                  auto row_buf = _CR_row2data(txtCRID, txtVPeriod, txtVPDir, txtVeh_Type, txtVP_Appro);

                  _bzero(buf_CR);
                  buf_CR.VPDir      = row_buf.VPDir;
                  buf_CR.VP_Approx  = row_buf.VP_Approx;
                  buf_CR.VPeri_Type = row_buf.VPeri_Type;
                  buf_CR.VPeriod16  = row_buf.VPeriod16;
                  buf_CR.VPeriod32  = row_buf.VPeriod32;
                  buf_CR.Vehcl_Type = row_buf.Vehcl_Type;

                  vec_CR.push_back(std::move(buf_CR));
               }

               rec_header.cnt_CRID = std::min(vec_CR.size(), max_uint4bits);
               if ( rec_header.cnt_CRID > 1 )
               {
                  CM_LOG_INFO("%s CRID %s, cnt %d. ", LOG_HEADER, txtCRID.c_str(), rec_header.cnt_CRID);
               }
            }
         }

         rec_header.ETA_flag = 0;
         rec_header.ptn_flag = 0;

         struct alignas(16){
            uint32_t ETA_type : 4;           /* half byte */
            uint32_t lane_num : 4;           /* half byte */
            char     laneinfo[15];           /* 15 bytes */
         } buf_TollETA;

         static_assert(sizeof(buf_TollETA) == 16, "The buffer for toll table is not 16 bytes;");

         struct alignas(16){
            uint32_t PatterNo;               /* 4 bytes */
            uint32_t ArrowNo;                /* 4 bytes */
         } buf_TollPattern;

         static_assert(sizeof(buf_TollPattern) == 16, "The buffer for toll table is not 16 bytes;");

         _bzero(buf_TollETA);
         _bzero(buf_TollPattern);

         if ( !txtCondId.empty() ) 
         {
            // Toll ETA : statment create/reset
            if ( stmt_sel_TollETA ) 
            {
               stmt_sel_TollETA->reset();
            }
            else
            {
               sql = "select * from " TABLE_Toll_ETA " where CondID == ?;";
               stmt_sel_TollETA = m_db->create_statement(sql.c_str());
            }

            if(stmt_sel_TollETA)
            {
               stmt_sel_TollETA->bind_text(1, txtCondId.c_str());

               TollETA_RowData buf;
               std::string lane;
               size_t eta_cnt = 0;
               while ( stmt_sel_TollETA->step_row() ) 
               {
                  size_t fld_pos = 0;
                  std::string txtCondID_1 = stmt_sel_TollETA->get_text(fld_pos++);
                  std::string txtTollMode = stmt_sel_TollETA->get_text(fld_pos++);
                  std::string txtTollCard = stmt_sel_TollETA->get_text(fld_pos++);
                  std::string txtTollType = stmt_sel_TollETA->get_text(fld_pos++);

                  std::tie(buf, lane) = _TollETA_row2data(txtCondID_1, txtTollMode, txtTollCard, txtTollType);;
                  eta_cnt++;
               }

               if ( eta_cnt == 1 && lane.size() <= sizeof(buf_TollETA.laneinfo)) {
                  buf_TollETA.ETA_type = buf.TollType;
                  buf_TollETA.lane_num = buf.lane_num;
                  lane.copy(buf_TollETA.laneinfo, lane.size());

                  rec_header.ETA_flag = 1;
               }
               else if ( eta_cnt > 1 ) {
                  CM_LOG_WARNING("%s[Toll] unexpected the Toll ETA number %d.", LOG_HEADER, eta_cnt);
               }
            }

            // Toll pattern : statment create/reset
            if ( stmt_sel_TollPattern ) 
            {
               stmt_sel_TollPattern->reset();
            }
            else
            {
               sql = "select * from " TABLE_Toll_Pattern " where CondID == ?;";
               stmt_sel_TollPattern = m_db->create_statement(sql.c_str());
            }

            if ( stmt_sel_TollPattern ) {
               stmt_sel_TollPattern->bind_text(1, txtCondId.c_str());

               size_t ptn_cnt = 0;
               while ( stmt_sel_TollPattern->step_row() )
               {
                  size_t fld_pos = 0;
                  std::string txtCondID_2 = stmt_sel_TollPattern->get_text(fld_pos++);
                  std::string txtPaternNo = stmt_sel_TollPattern->get_text(fld_pos++);
                  std::string txtArrowNo  = stmt_sel_TollPattern->get_text(fld_pos++);

                  auto buf = _TollPattern_row2data(txtCondID_2, txtPaternNo, txtArrowNo);
                  buf_TollPattern.PatterNo = buf.PatterNo;
                  buf_TollPattern.ArrowNo  = buf.ArrowNo;
                  ptn_cnt++;
               }

               if ( ptn_cnt == 1 ) {
                  rec_header.ptn_flag = 1;
               }
               else if ( ptn_cnt > 1 ) {
                  CM_LOG_WARNING("%s[Toll] unexpected the pattern number %d.", LOG_HEADER, ptn_cnt);
               }
            }
         }

         os.write(reinterpret_cast<const char*>(&rec_header), sizeof(rec_header));

         if ( rec_header.ETA_flag ) {
            os.write(reinterpret_cast<const char*>(&buf_TollETA), sizeof(buf_TollETA));
         }

         if ( rec_header.ptn_flag ) {
            os.write(reinterpret_cast<const char*>(&buf_TollPattern), sizeof(buf_TollPattern));
         }

         for(auto i = 0; i < rec_header.cnt_CRID; ++i)
         {
            buf_CR = vec_CR[i];
            os.write(reinterpret_cast<const char*>(&buf_CR), sizeof(buf_CR));
         }

         if ( ++row_num % 100 == 0 )
         {
            CM_LOG_INFO("%s stepped %d rows", LOG_HEADER, row_num);
         }

         //if ( row_num >= 5 )
         if(false)
         {
            break;
         }
      }

      m_db->remove_statement(stmt_sel_C);
      m_db->remove_statement(stmt_sel_CR);
      m_db->remove_statement(stmt_sel_TollETA);
      m_db->remove_statement(stmt_sel_TollPattern);

      uint32_t datasize = os.str().size() / 16;
      uint32_t dirtsize = os.str().size() % 16;
      CM_LOG_INFO("%s All stepped rows number is %d, data size %d, dirty data %d.", LOG_HEADER,
         row_num, datasize, dirtsize);

      //if (  dirtsize )
      if(true)
      {
         struct alignas(16){
            uint32_t recnum;
            uint32_t datsiz;
            uint64_t reserved;
         }header = {_LE(row_num), _LE(datasize), 0};

         static_assert(sizeof(header) == 16, "header is not 16 bytes!");

         const char* addr = reinterpret_cast<char*>(&header);
         bin = std::string(addr, sizeof(header)) + os.str();
      }
   }
   else{
      CM_LOG_ERROR("%s statement \"%s\" create error!", LOG_HEADER, sql.c_str());
   }

   return bin;
}


bool CCmDatabase::parse_db_C_CR_Toll(const char* bin_path)
{
   bool ok = false;
   CM_LOG_INFO("%s parse C-CR-Toll to \"%s\" .", LOG_HEADER, bin_path);

   std::ofstream ofs(bin_path);
   if ( ofs.is_open() ) 
   {
      auto bin = parse_db_C_CR_Toll();
      ofs.write(bin.data(), bin.size());
      ok = true;
   }

   return ok;
}

bool CCmDatabase::combine_db_C_CR(const char* path_C, const char* path_CR, const char* db_path)
{
   bool ok = false;

   if ( path_C && path_CR && db_path) 
   {
      if ( open_db(MEM_DB) ) 
      {
         const char* alias_C = "DB_C";
         const char* alias_CR = "DB_CR";
         ok = m_db->attach(path_C, alias_C);
         if ( ok ) {
            ok = m_db->attach(path_CR, alias_CR);
            if ( ok ) 
            {
               std::string sql = "create table C_CR as select * from DB_C." TABLE_C " as C, DB_CR.CR as CR where C.CRID == CR.CRID;";
               ok = m_db->execute(sql.c_str());
               if ( ok ) 
               {
                  ok = m_db->detach(alias_C);
                  if ( ok ) 
                  {
                     ok = m_db->detach(alias_CR);
                     if ( ok ) 
                     {
                        CM_LOG_INFO("%s =======> OK" , LOG_HEADER);
                        ok = save_as(db_path); 
                        if ( !ok ) 
                        {
                           CM_LOG_WARNING("%s save as %s failed!", LOG_HEADER, db_path);
                        }
                     }
                     else
                     {
                        CM_LOG_WARNING("%s failed detach : \"%s\"!", LOG_HEADER, alias_CR);
                     }
                  }
                  else
                  {
                     CM_LOG_WARNING("%s failed detach : \"%s\"!", LOG_HEADER, alias_C);
                  }
               }
               else
               {
                  CM_LOG_WARNING("%s failed create table : \"%s\"!", LOG_HEADER, sql.c_str());
               }
            }
            else
            {
               CM_LOG_WARNING("%s failed attach : \"%s\"!", LOG_HEADER, path_CR);
            }
         }
         else
         {
            CM_LOG_WARNING("%s failed attach : \"%s\"!", LOG_HEADER, path_C);
         }
      }
   }
   else 
   {
      CM_LOG_ERROR("%s illegal parameter C : %s, CR : %s, output DB path : %s", LOG_HEADER, path_C, path_CR, db_path);
   }

   return ok;
}

bool CCmDatabase::combine_db_C_CR_Toll(
   const char* path_C, 
   const char* path_CR, 
   const char* path_Toll_ETA, 
   const char* path_Toll_Pattern, 
   const char* db_path)
{
   bool ok = false;

   if ( path_C && path_CR && db_path && path_Toll_ETA && path_Toll_Pattern) 
   {
      if ( open_db(MEM_DB) ) 
      {
         const char* alias_C = "DB_C";
         const char* alias_CR = "DB_CR";
         const char* alias_Toll_ETA = "DB_Toll_ETA";
         const char* alias_Toll_Pattern = "DB_Toll_Pattern";

         std::bitset<4> grp_ret("0000");
         grp_ret[0] = m_db->attach(path_C,            alias_C);
         grp_ret[1] = m_db->attach(path_CR,           alias_CR);
         grp_ret[2] = m_db->attach(path_Toll_ETA,     alias_Toll_ETA);
         grp_ret[3] = m_db->attach(path_Toll_Pattern, alias_Toll_Pattern);

         ok = grp_ret.all(); 
         if (ok) 
         {
            auto tabsql = [](const char* t, const char* a)
            {
               std::ostringstream os;
               os << "create table " << t << " as select * from " << a << "." << t << ";";;
               return os.str();
            };

            grp_ret[0] = m_db->execute(tabsql(TABLE_C,            alias_C           ).c_str());
            grp_ret[1] = m_db->execute(tabsql(TABLE_CR,           alias_CR          ).c_str());
            grp_ret[2] = m_db->execute(tabsql(TABLE_Toll_ETA,     alias_Toll_ETA    ).c_str());
            grp_ret[3] = m_db->execute(tabsql(TABLE_Toll_Pattern, alias_Toll_Pattern).c_str());

            ok = grp_ret.all();
            if ( ok ) 
            {
               grp_ret[0] = m_db->detach(alias_C);
               grp_ret[1] = m_db->detach(alias_CR);
               grp_ret[2] = m_db->detach(alias_Toll_ETA);
               grp_ret[3] = m_db->detach(alias_Toll_Pattern);

               ok = grp_ret.all();
               if ( ok ) 
               {
                  ok = save_as(db_path); 
                  if ( ok ) 
                  {
                     CM_LOG_INFO("%s combine C-CR-Toll table OK!" , LOG_HEADER);
                  }
                  else
                  {
                     CM_LOG_WARNING("%s save as %s failed!", LOG_HEADER, db_path);
                  }
               }
               else
               {
                  CM_LOG_WARNING("%s detach tables failed %s!", LOG_HEADER, grp_ret.to_string().c_str());
               }
            }
         }
         else
         {
            CM_LOG_WARNING("%s attach tables failed %s!", LOG_HEADER, grp_ret.to_string().c_str());
         }
      }
   }
   else 
   {
      CM_LOG_ERROR("%s illegal parameter C : %s, CR : %s, Toll ETA : %s, Toll Pattern : %s, output DB path : %s", 
         LOG_HEADER, path_C, path_CR, path_Toll_ETA, path_Toll_Pattern, db_path);
   }


   return ok;
}

bool CCmDatabase::do_argv(std::vector<std::string> &v)
{
   //CM_LOG_INFO("%s the number of arguments is %d.", LOG_HEADER, v.size());
   bool ok = false;

   std::vector<std::tuple<std::string, std::string, std::string>> vecMid, vecDB;
   for(const auto& arg : v)
   {
      std::string dir, bname, ext;
      std::tie(dir, bname, ext) = parse_path(arg);
      if ( "mid" == ext ) {
         vecMid.push_back(std::make_tuple(dir, bname, arg));
      }
      else if ( "db" == ext ) {
         vecDB.push_back(std::make_tuple(dir, bname, arg));
      }
   }

   std::regex ptn_N, ptn_C, ptn_CR, ptn_ETA, ptn_Pattern, ptn_C_CR_Toll, ptn_Junction;
   std::tie(ptn_N, ptn_C, ptn_CR, ptn_ETA, ptn_Pattern, ptn_C_CR_Toll, ptn_Junction) = g_bname_ptn;

   if( ! vecMid.empty() && ! vecDB.empty())
   {
      CM_LOG_WARNING("do on not know what to do!", LOG_HEADER);
   }
   else if( ! vecMid.empty())
   {
      std::string mid_dir;
      auto& dst = vecMid[0];
      if(std::all_of(vecMid.begin(), vecMid.end(), [&dst](decltype(dst)& src){return std::get<0>(src) == std::get<0>(dst);}))
      {
         std::tie(mid_dir, std::ignore, std::ignore) = dst;
      }

      if( 1 == vecMid.size())
      {
         std::string mid_path;
         std::tie(std::ignore, std::ignore, mid_path) = dst;
         ok = import_mid(mid_path.c_str());
      }

#if  0     /* ----- #if 0 : If0Label_1 ----- */
      else if( 2 == vecMid.size())
      {
         //ok = im
      }
#endif     /* ----- #if 0 : If0Label_1 ----- */

      else
      {
         CM_LOG_WARNING("%s unexpected the number(%d) of mid files!", LOG_HEADER, vecMid.size());
      }
   }
   else if( ! vecDB.empty())
   {
      std::string out_dir;
      auto& dst = vecDB[0];
      if(std::all_of(vecDB.begin(), vecDB.end(), [&dst](decltype(dst)& src){return std::get<0>(src) == std::get<0>(dst);}))
      {
         std::tie(out_dir, std::ignore, std::ignore) = dst;
      }

      //CM_LOG_INFO("%s DB number : %d, out dir : %s.", LOG_HEADER, vecDB.size(), out_dir.c_str());
      if( 1 == vecDB.size())
      {
         std::string db_path;
         std::tie(std::ignore, std::ignore, db_path) = dst;
         ok = parse_db(db_path.c_str());
      }
      else if( 2 == vecDB.size())
      {
         std::string path_C, path_CR;
         std::string prvnc_C, prvnc_CR;
         for(auto& e : vecDB)
         {
            std::string basname, db_path;
            std::tie(std::ignore, basname, db_path) = e;
            std::smatch m;

            //CM_LOG_INFO("%s the DB base name \"%s\".", LOG_HEADER, basname.c_str());
            if ( path_C.empty() && std::regex_match(basname, m, ptn_C) ) {
               path_C = db_path;
               prvnc_C = m[1].str();
            }
            else if ( path_CR.empty() && std::regex_match(basname, m, ptn_CR) ) {
               path_CR = db_path;
               prvnc_CR = m[1].str();
            }
         }

         //CM_LOG_INFO("%s province C : \"%s\", province CR : \"%s\".", LOG_HEADER, prvnc_C.c_str(), prvnc_CR.c_str());
         if((prvnc_CR == prvnc_C) && ! path_CR.empty() && ! path_C.empty())
         {
            std::string db_path = prvnc_C + "_C_CR.db";
            if( ! out_dir.empty())
            {
               db_path = out_dir + '/' + db_path;
            }
            ok = combine_db_C_CR(path_C.c_str(), path_CR.c_str(),  db_path.c_str());
         }
      }
      else if( 4 == vecDB.size())
      {
         std::string path_C,  path_CR,    path_Toll_ETA,    path_Toll_Pattern;
         std::string prvnc_C, prvnc_CR,   prvnc_Toll_ETA,   prvnc_Toll_Pattern;
         for(auto& e : vecDB)
         {
            std::string basname, db_path;
            std::tie(std::ignore, basname, db_path) = e;
            std::smatch m;

            //CM_LOG_INFO("%s the DB base name \"%s\".", LOG_HEADER, basname.c_str());
            if ( path_C.empty() && std::regex_match(basname, m, ptn_C) ) {
               path_C = db_path;
               prvnc_C = m[1].str();
            }
            else if ( path_CR.empty() && std::regex_match(basname, m, ptn_CR) ) {
               path_CR = db_path;
               prvnc_CR = m[1].str();
            }
            else if ( path_Toll_ETA.empty() && std::regex_match(basname, m, ptn_ETA) ) {
               path_Toll_ETA = db_path;
               prvnc_Toll_ETA = m[1].str();
            }
            else if ( path_Toll_Pattern.empty() && std::regex_match(basname, m, ptn_Pattern) ) {
               path_Toll_Pattern = db_path;
               prvnc_Toll_Pattern = m[1].str();
            }
         }

         // checking helpers
         std::vector<std::string> ref_prvnc{ std::ref(prvnc_C),   std::ref(prvnc_CR),  std::ref(prvnc_Toll_ETA),  std::ref(prvnc_Toll_Pattern)};  
         std::vector<std::string> ref_path{  std::ref(path_C),    std::ref(path_CR),   std::ref(path_Toll_ETA),   std::ref(path_Toll_Pattern)};  

         auto province = ref_prvnc[0];
         if ( std::all_of(ref_prvnc.begin(), ref_prvnc.end(), [&province](std::string& s){return s == province;}) 
           && std::all_of(ref_path.begin(),  ref_path.end(),  [](std::string& s){return !s.empty();})) 
         {
            std::string db_path = province + "_C_CR_Toll.db";
            if( ! out_dir.empty())
            {
               db_path = out_dir + '/' + db_path;
            }
            ok = combine_db_C_CR_Toll(path_C.c_str(), path_CR.c_str(), path_Toll_ETA.c_str(), path_Toll_Pattern.c_str(), db_path.c_str());
         }
         else
         {
            CM_LOG_WARNING("%s checking failed!", LOG_HEADER);
         }
      }
      else
      {
         CM_LOG_WARNING("%s unexpected the number(%d) of DB files!", LOG_HEADER, vecDB.size());
      }
   }
   else
   {
      CM_LOG_WARNING("%s No input available!!", LOG_HEADER);
   }

   return ok;
}
