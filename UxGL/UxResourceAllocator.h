//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxError.h"

#include <vector>
#include <memory>
#include <cassert>
#include <gl/glew.h>


//========================================================================
//  Generic Resource Allocator:
//    Manages allocation inside a set of linear resource.
//========================================================================

template<typename tpResourceType, GLenum tpMaxResourceEnum>
class UxResourceAllocator
{
private:
  // Static initialisations
  static bool _Startup;
  static void Startup();

  // Table of allocated resources by the application
  static std::vector<std::shared_ptr<tpResourceType>>* _Allocations;

public:
  
  // Allocate a new resource
  static uint32_t allocate(tpResourceType* iResourceInstance, const std::string& iName, int32_t iResourceIndex);

  // Retrieves resource from its name in the table of allocations
  static std::shared_ptr<tpResourceType> getResource(const std::string& iName);

  // Removes the resource from the table
  static void remove(const tpResourceType* iResource);
  
  __DeclareDeletedCtor(UxResourceAllocator)
};

template<typename tpResourceType, GLenum tpMaxResourceEnum>
std::vector<std::shared_ptr<tpResourceType>>* UxResourceAllocator<tpResourceType, tpMaxResourceEnum>::_Allocations = nullptr;

template<typename tpResourceType, GLenum tpMaxResourceEnum>
bool UxResourceAllocator<tpResourceType, tpMaxResourceEnum>::_Startup = false;

template<typename tpResourceType, GLenum tpMaxResourceEnum>
void UxResourceAllocator<tpResourceType, tpMaxResourceEnum>::Startup()
{
  if (!_Startup)
  {
    _Allocations = new std::vector<std::shared_ptr<tpResourceType>>();

    GLint size;
    glGetIntegerv(tpMaxResourceEnum, &size);
    __CheckGLErrors;
    _Allocations->resize(size, nullptr);
    _Startup = true;
  }
}

template<typename tpResourceType, GLenum tpMaxResourceEnum>
uint32_t UxResourceAllocator<tpResourceType, tpMaxResourceEnum>::allocate(tpResourceType* iResourceInstance, const std::string& iName, int32_t iResourceIndex)
{
  Startup();

  // If location is explicit must be possible (less than max attribute location) and not yet allocated
  assert(iResourceIndex == -1 || ((uint32_t)iResourceIndex < _Allocations->size() && !_Allocations->at(iResourceIndex)));

  // verifies name unicity
  for (auto it = _Allocations->begin(); it != _Allocations->end(); it++)
  {
    if (*it && ((*it)->getName() == iName))
    {
      UxError::error(__FILE__, __LINE__) << "Resource name, " << iName.c_str() << ",is not unique for " << typeid(tpResourceType).name() << ".\n";
      UxError::UxError::exit(-1);
    }
  }

  if (iResourceIndex == -1)
  {
    // Allocate the first free location to the new attribute
    for (auto it = _Allocations->begin(); it != _Allocations->end(); it++)
    {
      if (!*it)
      {
        *it = std::shared_ptr<tpResourceType>(iResourceInstance);
        return it - _Allocations->begin();
      }
    }

    // No place left, exits
    UxError::error(__FILE__, __LINE__) << "Maximum number of resource " << typeid(tpResourceType).name() << " (" << _Allocations->size() << ") reached.\n";
    UxError::UxError::exit(-1);
  }
  
  // Direct allocation
  (*_Allocations)[iResourceIndex] = std::shared_ptr<tpResourceType>(iResourceInstance);

  return iResourceIndex;
}

template <typename tpResourceType, GLenum tpMaxResourceEnum>
std::shared_ptr<tpResourceType> UxResourceAllocator<tpResourceType, tpMaxResourceEnum>::getResource(const std::string& iName)
{
  Startup();

  for (auto it = _Allocations->begin(); it != _Allocations->end(); it++)
  {
    if (*it && iName == (*it)->getName())
      return *it;
  }

  assert(0);
  return nullptr;
}

template <typename tpResourceType, GLenum tpMaxResourceEnum>
void UxResourceAllocator<tpResourceType, tpMaxResourceEnum>::remove(const tpResourceType* iResource)
{
  Startup();

  for (auto it = _Allocations->begin(); it != _Allocations->end(); it++)
  {
    if (it->get() == iResource)
    {
      *it = std::shared_ptr<tpResourceType>(nullptr);
      return;
   }
  }
}
