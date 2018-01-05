//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#ifndef CONCAT_
#define CONCAT_(a,b) a ## b
#endif

#ifndef CONCAT
#define CONCAT(a, b) CONCAT_(a, b)
#endif

#ifndef QUOTE_
#define QUOTE_(str) #str
#endif

#ifndef QUOTE
#define QUOTE(str) QUOTE_(str) 
#endif

#if defined(__UxReportName) && defined(__UxReportSize) && defined (__UxReportInstance)

struct CONCAT(UxReport_, __UxReportName)
{
  constexpr static char*    _name = QUOTE(__UxReportName);
  constexpr static char*    _path = QUOTE(CONCAT(Reports/, __UxReportName));
  constexpr static uint32_t _size = __UxReportSize;
#ifdef __UxReportPath
#include QUOTE(CONCAT(__UxReportPath, CONCAT(/Reports/,__UxReportName)))
#else
#include QUOTE(CONCAT(Reports/,__UxReportName))
#endif   
};

struct CONCAT(SSBO_UxReport_, __UxReportName)
{
  CONCAT(UxReport_, __UxReportName) _data[__UxReportSize];
  uint32_t _counter;
};

UxReport<CONCAT(UxReport_, __UxReportName), CONCAT(SSBO_UxReport_, __UxReportName)> __UxReportInstance;

#undef __UxReportName
#undef __UxReportSize
#undef __UxReportInstance

#endif




