//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxReportBase.h"

#include <vector>

//========================================================================
//  GPU report manager:
//    Manages set of reports invoked in glsl files.
//========================================================================

class UxReportManager
{
private:
  
  static bool _Startup;
  static std::vector<UxReportBase*>* _Reports;

private:
  
  static void StartUp();

public:

  __DeclareDeletedCtor(UxReportManager)

  static void addReport(UxReportBase* iReport);
  static void removeReport(const std::string iReportName);

  static UxReportBase *getReport(const std::string iReportName);

  static void parseGLSLDirectives(const std::string& iFileName, std::vector<UxReportBase*>& ioReports, std::string& ioBuffer);
  static uint32_t parseArgs(std::string iBuffer, std::vector<std::string>& oArguments, std::string& oErrorMessage);
};
