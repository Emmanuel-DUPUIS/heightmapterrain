//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxUniformBlockBase.h"


//========================================================================
//  Uniform Block encapsulation:
//    Specifies data structure as a class template.
//========================================================================

template<typename tpUniformStructure>
class UxUniformBlock: public UxUniformBlockBase
{
public:
  UxUniformBlock(const std::string& iName, GLenum iUsage = GL_DYNAMIC_DRAW, GLint iBinding = -1);
  virtual ~UxUniformBlock() {}
  __DeclareDeletedCtorsAndAssignments(UxUniformBlock)

  template<typename tpUniformStructure>
  friend class UxUniformBlockDataAccessor;

protected:
  tpUniformStructure* map(GLbitfield iAccess = GL_MAP_WRITE_BIT);
};

template<typename tpUniformStructure>
UxUniformBlock<tpUniformStructure>::UxUniformBlock(const std::string& iName, GLenum iUsage, GLint iBinding)
: UxUniformBlockBase(iName, iUsage, iBinding, typeid(tpUniformStructure).name(), sizeof(tpUniformStructure))
{
}

template<typename tpUniformStructure>
tpUniformStructure* UxUniformBlock<tpUniformStructure>::map(GLbitfield iAccess)
{
  UxUniformBlockBase::map();
  tpUniformStructure* pMappedData = (tpUniformStructure*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(tpUniformStructure), iAccess);
  __CheckGLErrors;
  return pMappedData;
}
