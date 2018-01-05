//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxShader.h"
#include "UxVertexArrayBase.h"
#include "UxIndexBuffer.h"

#include <gl/glew.h>
#include <string>
#include <memory>
#include <map>
#include <set>

class UxVertexInputAttribute;

//========================================================================
//  Program encapsulation:
//    Manages shaders, associated reports and vertex attributes. 
//========================================================================

class UxProgram
{
private:
  // Correlation table between GL_xxx OpenGL define types and GLSL types
  static std::map<int, std::pair<const char*, const char*>> _TypeTable;

private:
  GLuint                                    _GLid;     // GL program id
  std::string                               _Name;     // Application name
  std::set<std::shared_ptr<UxReportBase>>   _Reports;  // Set of reports invoked by the shaders linked to the program

public:
  UxProgram(const char* iName);
  ~UxProgram();
  __DeclareDeletedCtorsAndAssignments(UxProgram)

  GLuint id() const { return _GLid; };
  const char* getName() const { return _Name.c_str(); }

  // Displays on the standard output the structure (inputs/outputs, attributes, UBo, SSBo, ...) of the program
  void introspect() const;

  void attachShaders(const std::vector<UxShader>& shaders, uint32_t indexStart, uint32_t indexEnd);
  void link();

  // Binds vertex attribute (input of vertex shader) to the program
  void bindVertexAttributes(const char* iBindings[][2], uint32_t iBindingNb);
  void bindVertexAttribute(std::shared_ptr<UxVertexInputAttribute> iInputAttribute, const std::string& iShaderInputName);

  // Bind the SSBo associated to the report to the program
  void bindReports();

  // Encapsulation of glUseProgram
  void use() const;

  // Draw elements specified in a VAO using an Element Array Buffer
  void draw(GLenum iMode, const UxVertexArrayBase& iVertexArray, const UxIndexBuffer& iIndexBuffer);
};

