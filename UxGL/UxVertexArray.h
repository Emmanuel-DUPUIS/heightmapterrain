//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxVertexArrayBase.h"


//========================================================================
//  Vertex Array Object encapsulation:
//    Manages data content (vertices), specifying a template structure.
//========================================================================

template<typename tpVertexStructure>
class UxVertexArray: public UxVertexArrayBase
{
  // Vertex data structure has to be a POD for offset accuracy
  static_assert(std::is_pod<tpVertexStructure>::value, "Not a POD.");

private:
  std::vector<tpVertexStructure> _Data;

public:

  UxVertexArray() {}
  ~UxVertexArray() {}
  __DeclareDeletedCtorsAndAssignments(UxVertexArray)

  void store(GLenum iUsage);
  void addVertex(const tpVertexStructure& iVertexData);

  template<typename tpAttributeType>
  void linkAttribute(std::shared_ptr<UxVertexInputAttribute> iInputAttribute, tpAttributeType tpVertexStructure::* iMember, GLenum iDataAttributeType, bool iNormalized);

  virtual uint32_t     getElementNumber() const { return _Data.size(); }
  virtual uint32_t     getStructureSize() const { return sizeof(tpVertexStructure); }
  virtual const void*  getData() const { return reinterpret_cast<const void*>(_Data.data()); }
  virtual void         clearVector() { UxVertexArrayBase::clearVector(); _Data.clear(); }
};

template<typename tpVertexStructure>
void UxVertexArray<tpVertexStructure>::addVertex(const tpVertexStructure& iVertexData)
{
  _Data.push_back(iVertexData);
}

template<typename tpVertexStructure>
void UxVertexArray<tpVertexStructure>::store(GLenum iUsage)
{
  UxVertexArrayBase::store(iUsage);
  _Data.clear();
}

template<typename tpVertexStructure>
template<typename tpAttributeType>
void UxVertexArray<tpVertexStructure>::linkAttribute(std::shared_ptr<UxVertexInputAttribute> iInputAttribute, tpAttributeType tpVertexStructure::* iMember, GLenum iDataAttributeType, bool iNormalized)
{
  uint32_t nbComponents    = sizeof(tpAttributeType) / 4;
  uint32_t vertexSize      = sizeof(tpVertexStructure);
  uint32_t attributeOffset = offsetof(tpVertexStructure, *iMember);
  
  UxVertexArrayBase::linkAttribute(iInputAttribute, iDataAttributeType, nbComponents, vertexSize, attributeOffset, iNormalized);
}
