//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxGL.h"
#include "UxUniformBlock.h"
#include <cassert>

//========================================================================
//  Data Accessor associated to a Uniform Block:
//    Manages access to the data of the uniform.
//========================================================================

template<typename tpStructureType>
class UxUniformBlockDataAccessor
{
private:

  std::shared_ptr<UxUniformBlock<tpStructureType>> _UniformBlock;
  tpStructureType*                                 _MappedData;

public:

  UxUniformBlockDataAccessor<tpStructureType>(std::shared_ptr<UxUniformBlock<tpStructureType>> iUniformBlock) { _UniformBlock = iUniformBlock; _MappedData = _UniformBlock->map(); }
  UxUniformBlockDataAccessor<tpStructureType>(std::shared_ptr<UxUniformBlockBase> iUniformBlock): UxUniformBlockDataAccessor(std::dynamic_pointer_cast<UxUniformBlock<tpStructureType>>(iUniformBlock)) { }
  ~UxUniformBlockDataAccessor<tpStructureType>() { _UniformBlock->unmap(); _MappedData = nullptr; _UniformBlock = nullptr; }
  __DeclareDeletedCtorsAndAssignments(UxUniformBlockDataAccessor)

  tpStructureType* operator ->() { assert(_MappedData); return _MappedData; };
};
