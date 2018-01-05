//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxProgram.h"

#include "UxGLObjects.h"
#include "UxVertexInputAttribute.h"
#include "UxReportBase.h"
#include "UxError.h"
#include "UxUtils.h"

#include <direct.h> 
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <regex>

UxProgram::UxProgram(const char* iName)
{
  _GLid = glCreateProgram();
  _Name = iName;
}

UxProgram::~UxProgram()
{
  glDeleteProgram(_GLid);
};

void UxProgram::introspect() const
{
  GLint isLinked = 0;
  glGetProgramiv(_GLid, GL_LINK_STATUS, &isLinked);
  assert(isLinked == GL_TRUE);

  std::cerr << "\n=============================================================================================================\nProgram \"" << _Name << "\" (GL name " << _GLid  << "):\n";

  for (int iType = 0; iType < 2; iType++)
  {
    GLenum type = (iType == 0 ? GL_PROGRAM_INPUT : GL_PROGRAM_OUTPUT);
    if (iType == 0)
      std::cerr << "  * Program Inputs:\n";
    else
      std::cerr << "  * Program Outputs:\n";
    int nbResources = 0;
    glGetProgramInterfaceiv(_GLid, type, GL_ACTIVE_RESOURCES, &nbResources);
    __CheckGLErrors;
    for (GLint resourceIndex = 0; resourceIndex < nbResources; resourceIndex++)
    {
      GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE, GL_LOCATION, GL_LOCATION_COMPONENT, GL_IS_PER_PATCH, GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER, GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER, GL_LOCATION_INDEX };
      constexpr GLsizei nbRes = 1024;
      GLint resources[nbRes];
      GLsizei length​;
      glGetProgramResourceiv(_GLid, type, resourceIndex, SizeOfTable(properties) - (iType==0?1:0), properties, nbRes, &length​, resources);
      __CheckGLErrors;

      char* pResourceName = new char[resources[0]];
      glGetProgramResourceName(_GLid, type, resourceIndex, resources[0], &length​, pResourceName);
      
      auto tt = _TypeTable.find(resources[1]);
      std::cerr << "    - " << "\"" << pResourceName << "\" type " << tt->second.first << "|" << tt->second.second << " array_size=" << resources[2] << " at location " << resources[3];
      if (iType == 1)
        std::cerr << " (component=" << resources[4] << ",index=" << resources[SizeOfTable(properties) - 1] << ")";
      else
        std::cerr << " (component=" << resources[4] << ")";
      std::cerr << " Is per patch=" << resources[5] << std::endl;
      int shift = 6;
      std::cerr << "      + Referenced by shaders: ";
      bool empty = true;
      for (const char* str : { "Vertex", "Tesselation Control", "Tesselation Evaluation", "Geometry", "Fragment", "Compute" })
      {
        if (resources[shift])
        {
          if (!empty)
            std::cerr << ", ";
          std::cerr << str;
          empty = false;
        }

        shift++;
      }
      if (empty)
        std::cerr << "none";
      std::cerr << ".\n";
    }
  }

  for (int iType = 0; iType < 2; iType++)
  {
    GLenum type = (iType == 0 ? GL_UNIFORM_BLOCK : GL_SHADER_STORAGE_BLOCK);
    const char* typeName = (iType == 0 ? "Uniform Block (UBO)" : "Shader Storage Block (SSBO)");
    int nbActiveResources = 0;
    glGetProgramInterfaceiv(_GLid, type, GL_ACTIVE_RESOURCES, &nbActiveResources);
    for (GLint resourceIndex = 0; resourceIndex < nbActiveResources; resourceIndex++)
    {
      GLenum properties[] = { GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE, GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES, GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER, GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER };
      constexpr GLsizei nbResources = 1024;
      GLint resources[nbResources];
      GLsizei length​;
      glGetProgramResourceiv(_GLid, type, resourceIndex, SizeOfTable(properties), properties, nbResources, &length​, resources);

      char* pResourceName = new char[resources[0]];
      glGetProgramResourceName(_GLid, type, resourceIndex, resources[0], &length​, pResourceName);
      __CheckGLErrors;

      uint32_t nbActiveVariables = resources[3];
      int shift = 4;
      std::cerr << "  * " << typeName << " \"" << pResourceName << "\" at binding " << resources[1] << ", size=" << resources[2] << " bytes, " << nbActiveVariables << (nbActiveVariables > 1 ? " indices" : " index") << "\n";
      if (nbActiveVariables > 0)
      {
        if (iType == 0)
        {

          GLint* offsets = new GLint[nbActiveVariables];
          GLint* types = new GLint[nbActiveVariables];
          GLint* astrides = new GLint[nbActiveVariables];
          GLint* mstrides = new GLint[nbActiveVariables];

          glGetActiveUniformsiv(_GLid, nbActiveVariables, (const GLuint*)&resources[shift], GL_UNIFORM_OFFSET, offsets);
          glGetActiveUniformsiv(_GLid, nbActiveVariables, (const GLuint*)&resources[shift], GL_UNIFORM_TYPE, types);
          glGetActiveUniformsiv(_GLid, nbActiveVariables, (const GLuint*)&resources[shift], GL_UNIFORM_ARRAY_STRIDE, astrides);
          glGetActiveUniformsiv(_GLid, nbActiveVariables, (const GLuint*)&resources[shift], GL_UNIFORM_MATRIX_STRIDE, mstrides);

          std::vector<std::pair<uint32_t, GLint>> ov;
          for (uint32_t v = 0; v < nbActiveVariables; v++)
            ov.push_back(std::pair<uint32_t, GLint>(v, offsets[v]));
          std::sort(ov.begin(), ov.end(), [](std::pair<uint32_t, GLint> a, std::pair<uint32_t, GLint> b) { return a.second < b.second;  });

          std::cerr << "    - Indices: \n";
          for (auto it = ov.begin(); it < ov.end(); it++)
          {
            GLsizei len;
            char name[1024];
            glGetActiveUniformName(_GLid, resources[shift + it->first], 1024, &len, name);

            auto tt = _TypeTable.find(types[it->first]);
            std::cerr << "      + " << std::setw(3) << resources[shift+it->first] << ": " << name << " type " << tt->second.first << "|" << tt->second.second << " offset=" << offsets[it->first] << " array_stride=" << astrides[it->first] << " matrix_stride=" << mstrides[it->first] << "\n";
          }

          delete[] offsets; offsets = nullptr;
          delete[] types; types = nullptr;
          delete[] astrides; astrides = nullptr;
          delete[] mstrides; mstrides = nullptr;
        }
      }
      shift += nbActiveVariables;
      std::cerr << "    - Referenced by shaders: ";
      bool empty = true;
      for (const char* str : { "Vertex", "Tesselation Control", "Tesselation Evaluation", "Geometry", "Fragment", "Compute" })
      {
        if (resources[shift])
        {
          if (!empty)
            std::cerr << ", ";
          std::cerr << str;
          empty = false;
        }
        
        shift++;
      }
      if (empty)
        std::cerr << "none";
      std::cerr << ".\n";
    }
  }

  std::cerr << "=============================================================================================================\n\n";

}

void UxProgram::attachShaders(const std::vector<UxShader>& shaders, uint32_t indexStart, uint32_t indexEnd)
{
  for (uint32_t index = indexStart; index <= indexEnd; index++)
  {
    glAttachShader(_GLid, shaders[index].getGLName());
    __CheckGLErrors;
    for (auto it = shaders[index].beginReport(); it != shaders[index].endReport(); it++)
      _Reports.insert(std::shared_ptr<UxReportBase>(*it));
  }

  link();
}

void UxProgram::link()
{
  glLinkProgram(_GLid);
  __CheckGLErrors;

  GLint status;
  glGetProgramiv(_GLid, GL_LINK_STATUS, &status);

  if (!status)
  {
    char buffer[4096];
    glGetProgramInfoLog(_GLid, 4096, NULL, buffer);
      
    UxError::error(__FILE__, __LINE__) << " Link program \"" << _Name.c_str() << "\" has failed:\n";
    std::cerr << buffer;
      
    glDeleteProgram(_GLid);
    UxError::UxError::exit(-1);
  }

  glGetProgramiv(_GLid, GL_INFO_LOG_LENGTH, &status);
  if (status)
  {
    char buffer[4096];
    glGetProgramInfoLog(_GLid, 4096, NULL, buffer);

    UxError::error(__FILE__, __LINE__) << " Link program has generated the log below:\n";
    std::cerr << buffer;

    glDeleteProgram(_GLid);
    UxError::UxError::exit(-1);
  }
}

void UxProgram::use() const
{
  glUseProgram(_GLid);
}

void UxProgram::draw(GLenum iMode, const UxVertexArrayBase& iVertexArray, const UxIndexBuffer& iIndexBuffer)
{
  glUseProgram(_GLid);
  iVertexArray.bind(iIndexBuffer);
  __CheckGLErrors;
  glDrawElements(iMode, iIndexBuffer.getBufferSize(), GL_UNSIGNED_INT, 0);
  __CheckGLErrors;
  UxVertexArrayBase::unbind();
  __CheckGLErrors;
}

void UxProgram::bindVertexAttributes(const char* iBindings[][2], uint32_t iBindingNb)
{
  for (uint32_t index = 0; index < iBindingNb; index++)
  {
    bindVertexAttribute(UxGLObjects::getInputAttribute(iBindings[index][0]), iBindings[index][1]);
  }

  link();
}

void UxProgram::bindVertexAttribute(const std::shared_ptr<UxVertexInputAttribute> iInputAttribute, const std::string& iShaderInputName)
{
  assert(iInputAttribute);

  // Verifies that iShaderInputName is a valid name (i.e. well defined as input in a shader attached to the program)
  assert(glGetAttribLocation(_GLid, iShaderInputName.c_str()) != -1);

  int32_t location = iInputAttribute->getLocation();
  assert(location >= 0);

  glBindAttribLocation(_GLid, location, iShaderInputName.c_str());
  __CheckGLErrors;
}

void UxProgram::bindReports()
{
  for (auto report: _Reports)
    report->bindToProgram(this);
}

std::map<int, std::pair<const char*, const char*>> UxProgram::_TypeTable = {
  { GL_FLOAT, std::pair<const char*, const char*>("GL_FLOAT", "float") },
  { GL_FLOAT_VEC2, std::pair<const char*, const char*>("GL_FLOAT_VEC2", "vec2") },
  { GL_FLOAT_VEC3, std::pair<const char*, const char*>("GL_FLOAT_VEC3", "vec3") },
  { GL_FLOAT_VEC4, std::pair<const char*, const char*>("GL_FLOAT_VEC4", "vec4") },
  { GL_DOUBLE, std::pair<const char*, const char*>("GL_DOUBLE", "double") },
  { GL_DOUBLE_VEC2, std::pair<const char*, const char*>("GL_DOUBLE_VEC2", "dvec2") },
  { GL_DOUBLE_VEC3, std::pair<const char*, const char*>("GL_DOUBLE_VEC3", "dvec3") },
  { GL_DOUBLE_VEC4, std::pair<const char*, const char*>("GL_DOUBLE_VEC4", "dvec4") },
  { GL_INT, std::pair<const char*, const char*>("GL_INT", "int") },
  { GL_INT_VEC2, std::pair<const char*, const char*>("GL_INT_VEC2", "ivec2") },
  { GL_INT_VEC3, std::pair<const char*, const char*>("GL_INT_VEC3", "ivec3") },
  { GL_INT_VEC4, std::pair<const char*, const char*>("GL_INT_VEC4", "ivec4") },
  { GL_UNSIGNED_INT, std::pair<const char*, const char*>("GL_UNSIGNED_INT", "unsigned int") },
  { GL_UNSIGNED_INT_VEC2, std::pair<const char*, const char*>("GL_UNSIGNED_INT_VEC2", "uvec2") },
  { GL_UNSIGNED_INT_VEC3, std::pair<const char*, const char*>("GL_UNSIGNED_INT_VEC3", "uvec3") },
  { GL_UNSIGNED_INT_VEC4, std::pair<const char*, const char*>("GL_UNSIGNED_INT_VEC4", "uvec4") },
  { GL_BOOL, std::pair<const char*, const char*>("GL_BOOL", "bool") },
  { GL_BOOL_VEC2, std::pair<const char*, const char*>("GL_BOOL_VEC2", "bvec2") },
  { GL_BOOL_VEC3, std::pair<const char*, const char*>("GL_BOOL_VEC3", "bvec3") },
  { GL_BOOL_VEC4, std::pair<const char*, const char*>("GL_BOOL_VEC4", "bvec4") },
  { GL_FLOAT_MAT2, std::pair<const char*, const char*>("GL_FLOAT_MAT2", "mat2") },
  { GL_FLOAT_MAT3, std::pair<const char*, const char*>("GL_FLOAT_MAT3", "mat3") },
  { GL_FLOAT_MAT4, std::pair<const char*, const char*>("GL_FLOAT_MAT4", "mat4") },
  { GL_FLOAT_MAT2x3, std::pair<const char*, const char*>("GL_FLOAT_MAT2x3", "mat2x3") },
  { GL_FLOAT_MAT2x4, std::pair<const char*, const char*>("GL_FLOAT_MAT2x4", "mat2x4") },
  { GL_FLOAT_MAT3x2, std::pair<const char*, const char*>("GL_FLOAT_MAT3x2", "mat3x2") },
  { GL_FLOAT_MAT3x4, std::pair<const char*, const char*>("GL_FLOAT_MAT3x4", "mat3x4") },
  { GL_FLOAT_MAT4x2, std::pair<const char*, const char*>("GL_FLOAT_MAT4x2", "mat4x2") },
  { GL_FLOAT_MAT4x3, std::pair<const char*, const char*>("GL_FLOAT_MAT4x3", "mat4x3") },
  { GL_DOUBLE_MAT2, std::pair<const char*, const char*>("GL_DOUBLE_MAT2", "dmat2") },
  { GL_DOUBLE_MAT3, std::pair<const char*, const char*>("GL_DOUBLE_MAT3", "dmat3") },
  { GL_DOUBLE_MAT4, std::pair<const char*, const char*>("GL_DOUBLE_MAT4", "dmat4") },
  { GL_DOUBLE_MAT2x3, std::pair<const char*, const char*>("GL_DOUBLE_MAT2x3", "dmat2x3") },
  { GL_DOUBLE_MAT2x4, std::pair<const char*, const char*>("GL_DOUBLE_MAT2x4", "dmat2x4") },
  { GL_DOUBLE_MAT3x2, std::pair<const char*, const char*>("GL_DOUBLE_MAT3x2", "dmat3x2") },
  { GL_DOUBLE_MAT3x4, std::pair<const char*, const char*>("GL_DOUBLE_MAT3x4", "dmat3x4") },
  { GL_DOUBLE_MAT4x2, std::pair<const char*, const char*>("GL_DOUBLE_MAT4x2", "dmat4x2") },
  { GL_DOUBLE_MAT4x3, std::pair<const char*, const char*>("GL_DOUBLE_MAT4x3", "dmat4x3") },
  { GL_SAMPLER_1D, std::pair<const char*, const char*>("GL_SAMPLER_1D", "sampler1D") },
  { GL_SAMPLER_2D, std::pair<const char*, const char*>("GL_SAMPLER_2D", "sampler2D") },
  { GL_SAMPLER_3D, std::pair<const char*, const char*>("GL_SAMPLER_3D", "sampler3D") },
  { GL_SAMPLER_CUBE, std::pair<const char*, const char*>("GL_SAMPLER_CUBE", "samplerCube") },
  { GL_SAMPLER_1D_SHADOW, std::pair<const char*, const char*>("GL_SAMPLER_1D_SHADOW", "sampler1DShadow") },
  { GL_SAMPLER_2D_SHADOW, std::pair<const char*, const char*>("GL_SAMPLER_2D_SHADOW", "sampler2DShadow") },
  { GL_SAMPLER_1D_ARRAY, std::pair<const char*, const char*>("GL_SAMPLER_1D_ARRAY", "sampler1DArray") },
  { GL_SAMPLER_2D_ARRAY, std::pair<const char*, const char*>("GL_SAMPLER_2D_ARRAY", "sampler2DArray") },
  { GL_SAMPLER_1D_ARRAY_SHADOW, std::pair<const char*, const char*>("GL_SAMPLER_1D_ARRAY_SHADOW", "sampler1DArrayShadow") },
  { GL_SAMPLER_2D_ARRAY_SHADOW, std::pair<const char*, const char*>("GL_SAMPLER_2D_ARRAY_SHADOW", "sampler2DArrayShadow") },
  { GL_SAMPLER_2D_MULTISAMPLE, std::pair<const char*, const char*>("GL_SAMPLER_2D_MULTISAMPLE", "sampler2DMS") },
  { GL_SAMPLER_2D_MULTISAMPLE_ARRAY, std::pair<const char*, const char*>("GL_SAMPLER_2D_MULTISAMPLE_ARRAY", "sampler2DMSArray") },
  { GL_SAMPLER_CUBE_SHADOW, std::pair<const char*, const char*>("GL_SAMPLER_CUBE_SHADOW", "samplerCubeShadow") },
  { GL_SAMPLER_BUFFER, std::pair<const char*, const char*>("GL_SAMPLER_BUFFER", "samplerBuffer") },
  { GL_SAMPLER_2D_RECT, std::pair<const char*, const char*>("GL_SAMPLER_2D_RECT", "sampler2DRect") },
  { GL_SAMPLER_2D_RECT_SHADOW, std::pair<const char*, const char*>("GL_SAMPLER_2D_RECT_SHADOW", "sampler2DRectShadow") },
  { GL_INT_SAMPLER_1D, std::pair<const char*, const char*>("GL_INT_SAMPLER_1D", "isampler1D") },
  { GL_INT_SAMPLER_2D, std::pair<const char*, const char*>("GL_INT_SAMPLER_2D", "isampler2D") },
  { GL_INT_SAMPLER_3D, std::pair<const char*, const char*>("GL_INT_SAMPLER_3D", "isampler3D") },
  { GL_INT_SAMPLER_CUBE, std::pair<const char*, const char*>("GL_INT_SAMPLER_CUBE", "isamplerCube") },
  { GL_INT_SAMPLER_1D_ARRAY, std::pair<const char*, const char*>("GL_INT_SAMPLER_1D_ARRAY", "isampler1DArray") },
  { GL_INT_SAMPLER_2D_ARRAY, std::pair<const char*, const char*>("GL_INT_SAMPLER_2D_ARRAY", "isampler2DArray") },
  { GL_INT_SAMPLER_2D_MULTISAMPLE, std::pair<const char*, const char*>("GL_INT_SAMPLER_2D_MULTISAMPLE", "isampler2DMS") },
  { GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, std::pair<const char*, const char*>("GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY", "isampler2DMSArray") },
  { GL_INT_SAMPLER_BUFFER, std::pair<const char*, const char*>("GL_INT_SAMPLER_BUFFER", "isamplerBuffer") },
  { GL_INT_SAMPLER_2D_RECT, std::pair<const char*, const char*>("GL_INT_SAMPLER_2D_RECT", "isampler2DRect") },
  { GL_UNSIGNED_INT_SAMPLER_1D, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_1D", "usampler1D") },
  { GL_UNSIGNED_INT_SAMPLER_2D, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_2D", "usampler2D") },
  { GL_UNSIGNED_INT_SAMPLER_3D, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_3D", "usampler3D") },
  { GL_UNSIGNED_INT_SAMPLER_CUBE, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_CUBE", "usamplerCube") },
  { GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_1D_ARRAY", "usampler1DArray") },
  { GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_2D_ARRAY", "usampler2DArray") },
  { GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE", "usampler2DMS") },
  { GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY", "usampler2DMSArray") },
  { GL_UNSIGNED_INT_SAMPLER_BUFFER, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_BUFFER", "usamplerBuffer") },
  { GL_UNSIGNED_INT_SAMPLER_2D_RECT, std::pair<const char*, const char*>("GL_UNSIGNED_INT_SAMPLER_2D_RECT", "usampler2DRect") },
};

