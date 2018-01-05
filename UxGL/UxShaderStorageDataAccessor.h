//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxGL.h"
#include "UxError.h"
#include "UxShaderStorage.h"

//========================================================================
//  Data Accessor associated to a Shader Storage:
//    Manages access to the data of the storage.
//========================================================================

template<typename tpStructureType>
class UxShaderStorageDataAccessor
{
private:

  std::shared_ptr<UxShaderStorage<tpStructureType>> _ShaderStorage;
  tpStructureType*                                  _MappedData;

public:

  UxShaderStorageDataAccessor<tpStructureType>(std::shared_ptr<UxShaderStorage<tpStructureType>> iShaderStorage) { _ShaderStorage = iShaderStorage; _MappedData = _ShaderStorage->map(); }
  UxShaderStorageDataAccessor<tpStructureType>(std::shared_ptr<UxShaderStorageBase> iShaderStorage): UxShaderStorageDataAccessor(std::dynamic_pointer_cast<UxShaderStorage<tpStructureType>>(iShaderStorage)) { }
  ~UxShaderStorageDataAccessor<tpStructureType>() { _ShaderStorage->unmap(); _MappedData = nullptr; _ShaderStorage = nullptr; }
  __DeclareDeletedCtorsAndAssignments(UxShaderStorageDataAccessor)

  tpStructureType* operator ->() { __AssertIfNot(_MappedData, "Shader Storage tries to access unmapped buffer."); return _MappedData; };
};
