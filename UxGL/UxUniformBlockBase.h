//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxResourceAllocator.h"

class UxProgram;
    
//========================================================================
//  Uniform Block encapsulation:
//    Manages automatic resource allocation.
//========================================================================

class UxUniformBlockBase
{
  typedef class UxResourceAllocator<UxUniformBlockBase, GL_MAX_UNIFORM_BUFFER_BINDINGS> allocator;

private:
  std::string _Name;
  GLuint      _Buffer;
  GLuint      _Binding;
  std::string _StructureName;
  size_t      _BufferSize;

protected:
  bool        _IsMapped;

public:

  UxUniformBlockBase(const std::string& iName, GLenum iUsage, int32_t iBinding, const std::string& iStructureName, size_t iBufferSize);
  virtual ~UxUniformBlockBase();
  __DeclareDeletedCtorsAndAssignments(UxUniformBlockBase)

  uint32_t            getLocation() const { return _Binding; }
  const std::string&  getName() const { return _Name; }

  void bindToProgram(const UxProgram* iProgram, const std::string& iUniformName);

protected:
  
  void map();
  void unmap();
};