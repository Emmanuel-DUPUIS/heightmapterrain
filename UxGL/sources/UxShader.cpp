//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxShader.h"

#include "UxReportManager.h"
#include "UxError.h"

#include <direct.h>
#include <regex>
#include <iostream>
#include <iomanip>

UxShader::UxShader(GLenum iType, const std::vector<std::string>& iFiles)
{
  _Type   = iType;
  _GLName = 0;

  std::vector<std::string> files = iFiles;
  files.insert(files.begin(), "Shaders/header.glsl");
  load(files);
}

UxShader::UxShader(UxShader&& source)
{
  _Type          = source._Type;
  _GLName        = source._GLName;
  _Reports       = std::move(source._Reports);
  source._Type   = 0;
  source._GLName = 0;
}

UxShader& UxShader::operator =(UxShader&& source)
{
  _Type          = source._Type;
  _GLName        = source._GLName;
  _Reports       = std::move(source._Reports);
  source._Type   = 0;
  source._GLName = 0;

  return *this;
}

UxShader::~UxShader()
{
};

void UxShader::load(std::vector<std::string> iFileNames, bool iCheckErrors)
{
  std::vector<std::string> fileBuffers(iFileNames.size());
  std::vector<uint32_t>    nbLines(iFileNames.size());

  for (uint32_t index = 0; index < iFileNames.size(); index++)
  {
    if (fileBuffers[index] == "")
    {
      FILE* fp = nullptr;
      if (fopen_s(&fp, iFileNames[index].c_str(), "rb"))
      {
        char cwd[512]; cwd[0] = '\0';
        _getcwd(cwd, sizeof(cwd));

        UxError::error(__FILE__, __LINE__) << " Can't open file " << iFileNames[index] << " (current working directory: " << (cwd[0] == 0 ? "failed to retrieve" : cwd) << ") in 'rb' mode.\n";
        UxError::UxError::exit(-1);
      }

      fseek(fp, 0, SEEK_END);
      uint32_t filesize = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      char* buffer = new char[filesize + 1];

      if (!buffer)
      {
        UxError::error(__FILE__, __LINE__) << " Can't allocate " << filesize + 1 << " bytes.\n";
        UxError::UxError::exit(-1);
      }

      fread(buffer, 1, filesize, fp);
      buffer[filesize] = 0;
      fclose(fp);

      fileBuffers[index] = buffer;

      delete buffer;
      buffer = nullptr;

      int32_t reportNb = _Reports.size();
      UxReportManager::parseGLSLDirectives(iFileNames[index], _Reports, fileBuffers[index]);

      nbLines[index] = 0;
      uint32_t lg = fileBuffers[index].length();
      for (uint32_t c = 0; c < lg; c++)
        if (fileBuffers[index][c] == '\n')
          nbLines[index]++;

      // Insert glsl files describing the SSBO associated to the reports
      int32_t addedFileNb = _Reports.size() - reportNb;
      if (addedFileNb > 0)
      {
        nbLines.resize(nbLines.size() + addedFileNb);
        fileBuffers.resize(fileBuffers.size() + addedFileNb);
        iFileNames.resize(iFileNames.size() + addedFileNb);

        nbLines[index + addedFileNb] = nbLines[index];
        fileBuffers[index + addedFileNb] = fileBuffers[index];
        for (uint32_t nbf = iFileNames.size() - 1; nbf >= index; nbf--)
          iFileNames[nbf] = iFileNames[nbf - addedFileNb];

        for (int32_t nf = 0; nf < addedFileNb; nf++)
        {
          iFileNames[index + nf] = _Reports[reportNb + nf]->getGLSLFileName();
        }

        fileBuffers[index] = "";
        index--;
      }
    }
  }

  _GLName = glCreateShader(_Type);
  __CheckGLErrors;

  if (!_GLName)
  {
    UxError::error(__FILE__, __LINE__) << " Can't create a new shader.\n";
    UxError::UxError::exit(-1);
  }

  const char** sources = new const char*[fileBuffers.size()];
  for (uint32_t ns = 0; ns < fileBuffers.size(); ns++)
    sources[ns] = fileBuffers[ns].c_str();

  glShaderSource(_GLName, fileBuffers.size(), sources, NULL);

  delete[] sources;
  sources = nullptr;

  glCompileShader(_GLName);
  __CheckGLErrors;

  if (iCheckErrors)
  {
    GLint status = 0;
    glGetShaderiv(_GLName, GL_COMPILE_STATUS, &status);
    __CheckGLErrors;

    if (!status)
    {
      char buffer[4096];
      glGetShaderInfoLog(_GLName, 4096, NULL, buffer);

      UxError::error(__FILE__, __LINE__) << " Shader compilation has failed:\n";
      displayBuffer(buffer, nbLines, iFileNames);

      glDeleteShader(_GLName);
      UxError::UxError::exit(-1);
    }

    glGetShaderiv(_GLName, GL_INFO_LOG_LENGTH, &status);
    __CheckGLErrors;
    if (status)
    {
      GLchar buffer[4096];
      glGetShaderInfoLog(_GLName, 4096, 0, buffer);
      UxError::error(__FILE__, __LINE__) << " Shader compilation has generated the log below:\n";
      displayBuffer(buffer, nbLines, iFileNames);
    }
  }
}

void UxShader::displayBuffer(char *pBuffer, std::vector<uint32_t> iNbLines, std::vector<std::string> iFileNames)
{
  for (uint16_t index = 0; index<iFileNames.size(); index++)
    std::cerr << std::setw(3) << index << " : " << iFileNames[index] << "\n";

  uint16_t kk = 0;
  char *pCurrent = pBuffer;
  std::cmatch cm;
  while (*pCurrent != '\0' && std::regex_search(pCurrent, cm, std::regex("(\\d)+\\((\\d+)\\)")))
  {
    std::cerr << std::string(pCurrent, cm[1].first - pCurrent);

    std::string token(cm[1].first, cm[1].second - cm[1].first);
    uint32_t nFile = std::stoul(token);
    uint32_t nFileLn = token.length();
    token = std::string(cm[2].first, cm[2].second - cm[2].first);
    uint32_t nLine = std::stoul(token);
    uint32_t nLineLn = token.length();

    uint16_t k = 0;
    while (k < iFileNames.size() && nLine > iNbLines[k])
    {
      nLine -= iNbLines[k];
      k++;
    }

    if (strncmp((char *)cm.suffix().first, " : ", 3) == 0)
    {
      if (k != kk)
      {
        //std::cerr << "Source File: " << iFileNames[k] << ":\n";
        std::cerr << std::setw(6) << "(" << k << ")" << nLine;
        kk = k;
      }
      else
        std::cerr << std::setw(6) << "(" << k << ")" << nLine;
    }
    else
      std::cerr << "(" << k << ")" << nLine;

    pCurrent = (char *)cm.suffix().first;
  }
  std::cerr << pCurrent << std::endl;
}


