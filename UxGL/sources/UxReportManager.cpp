//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxReportManager.h"
#include "UxError.h"

#include <iostream>
#include <regex>

bool UxReportManager::_Startup = false;
std::vector<UxReportBase*>* UxReportManager::_Reports = nullptr;

inline void UxReportManager::StartUp()
{
  if (!_Startup)
  {
    _Reports = new std::vector<UxReportBase*>;
    _Startup = true;
  }
}

void UxReportManager::addReport(UxReportBase* iReport)
{
  StartUp();
  _Reports->push_back(iReport);
}

void UxReportManager::removeReport(const std::string iReportName)
{
  StartUp();
  for (auto it = _Reports->begin(); it != _Reports->end(); it++)
    if ((*it)->getName() == iReportName)
    {
      _Reports->erase(it);
      break;
    }
}

UxReportBase* UxReportManager::getReport(const std::string iReportName)
{
  StartUp();
  for (auto it = _Reports->begin(); it != _Reports->end(); it++)
    if ((*it)->getName() == iReportName)
    {
      return *it;
    }
  return nullptr;
}


void UxReportManager::parseGLSLDirectives(const std::string& iFileName, std::vector<UxReportBase*>& ioReports, std::string& ioBuffer)
{
  static uint32_t           counterIndex = 0;
  static constexpr uint32_t nbTokens = 2;
  static constexpr char* tokens[nbTokens]   = { "UxReport::addRecord", "UxReport::setValue" };
  //static constexpr char* regexs[nbTokens]   = { "\\s*\\(\\s*\"(\\w+)\"\\s*\\)\\s*;", "\\s*\\(\\s*\"(\\w+)\"\\s*,\\s*([^,]+)\\s*,\\s*([^,]+)\\s*\\)\\s*;" };
  static constexpr char* replaces[nbTokens] = { "uint cnt_UxReport_$1__CounterIndex__ = atomicAdd(UxReport_$1.counter, 1);", "if (cnt_UxReport_$1__CounterIndex__ < __size__) UxReport_$1.data[cnt_UxReport_$1__CounterIndex__].$2 = $3; " };

  uint32_t lineNumber = 1;
  bool blockComment = false;
  bool lineComment  = false;

  uint32_t zero = 0;
  std::vector<uint32_t> matches(nbTokens, zero);

  char pc = 0;
  for (uint32_t indexChar = 0; indexChar < ioBuffer.length(); indexChar++)
  {
    char c = ioBuffer[indexChar];
    if (indexChar > 0)
    {
      if (pc == '/')
      {
        if (c == '*')
          blockComment = true;
        else if (c == '/')
          lineComment = true;
      }
      else if (pc == '*' && c == '/')
        blockComment = false;
      else if (pc == '/' && c == '/')
        blockComment = false;
    }

    if (!blockComment && !lineComment)
    {
      for (uint32_t nt = 0; nt < nbTokens; nt++)
      {
        if (c == tokens[nt][matches[nt]])
        {
          matches[nt]++;
          if (matches[nt] == strlen(tokens[nt]))
          {
            std::string sstr = ioBuffer.substr(indexChar + 1);

            std::vector<std::string> arguments;
            std::string errorMessage;

            uint32_t argNbChars = UxReportManager::parseArgs(sstr, arguments, errorMessage);
            if (argNbChars == 0 || arguments.size() == 0)
            {
              UxError::error(__FILE__, __LINE__) << " Parsing UxReport directives in GLSL text file " << iFileName << " has failed.\n" << sstr.substr(0, sstr.find('\n')) << "\n" << errorMessage << "\n";
              UxError::UxError::exit(-1);
            }

            const std::string reportName = arguments[0] = arguments[0].substr(arguments[0].find("\"") + 1, arguments[0].rfind("\"") - 1);
            UxReportBase* pReport = UxReportManager::getReport(reportName);
            if (pReport == nullptr)
            {
              UxError::error(__FILE__, __LINE__) << " Shader source reference undefined UxReport (" << reportName << ") at " << iFileName << ":" << lineNumber << ".\n";
              UxError::UxError::exit(-1);
            }

            auto it = ioReports.begin();
            for (; it != ioReports.end(); it++)
            {
              if (*it == pReport)
                break;
            }

            if (it == ioReports.end())
            {
              ioReports.push_back(pReport);
            }

            if (nt == 0)
              counterIndex++;

            std::string repl(replaces[nt]);
            repl = regex_replace(repl, std::regex("__size__"), std::to_string(pReport->getMaxRecordNumber()));
            repl = regex_replace(repl, std::regex("__CounterIndex__"), std::to_string(counterIndex));
           

            for (uint16_t ndx = 0; ndx < arguments.size(); ndx++)
              repl = regex_replace(repl, std::regex(std::string("\\$") + std::to_string(ndx + 1)), arguments[ndx]);

            ioBuffer.replace(indexChar - matches[nt] + 1, argNbChars + matches[nt], repl);
            indexChar += repl.length() - matches[nt];

            c = repl[repl.length()-1];
            matches[nt] = 0;
            break;
          }
        }
        else
          matches[nt] = 0;
      }
    }

    if (c == '\n')
    {
      lineComment = false;
    }

    if (c == '\n')
      lineNumber++;

    pc = c;
  }
}

uint32_t UxReportManager::parseArgs(std::string iBuffer, std::vector<std::string>& oArguments, std::string& oErrorMessage)
{
  uint32_t charNb  = iBuffer.length();
  uint32_t parenthesisLevel = 0;

  uint32_t indChar = 0;
  uint32_t indArg = 0;
  char c = iBuffer[0];
  
  while (c != ';' && indChar < charNb)
  {
    if (c == '(')
    {
      parenthesisLevel++;
      if (parenthesisLevel == 1)
        indArg = indChar + 1;
    }
    else if (c == ')')
    {
      parenthesisLevel--;
      if (parenthesisLevel < 0)
      {
        oErrorMessage = "Unexpected closing parenthesis.";
        return 0;
      }
    }
    
    if ((c == ',' && parenthesisLevel == 1) || (c == ')' && parenthesisLevel == 0))
    {
      // Adds another arg
      uint32_t end = indChar - 1;
      while (iBuffer[end] == ' ' || iBuffer[end] == '\t' || iBuffer[end] == '\r')
        end--;
      oArguments.push_back(std::string(iBuffer, indArg, end - indArg + 1));
      indArg = indChar+1;
    }

    // Removes blank at begining of arg
    if (indArg == indChar && (c == ' ' || c == '\t' || c == '\r'))
      indArg++;

    indChar++;
    c = iBuffer[indChar];
  }

  if (parenthesisLevel > 0)
  {
    oErrorMessage = "Missing closing parenthesis.";
    return 0;
  }

  return indChar+1;
}