//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxGL.h"
#include "UxReportBase.h"
#include "UxProgram.h"

#include <string>
#include <set>
#include <regex>
#include <direct.h>
#include <gl/glew.h>

//========================================================================
//  GPU report:
//    Manages data report of GPU data browsable in CPU (debug session).
//    Template class integrating report structure.
//========================================================================

template<class tRecord, class tpBufferStruct>
class UxReport: public UxReportBase
{
protected:
  tpBufferStruct* _MappedData;

public:

  UxReport() : UxReportBase(tRecord::_name, typeid(tpBufferStruct).name(), sizeof(tpBufferStruct), tRecord::_path, tRecord::_size) { _MappedData = nullptr; }
  virtual ~UxReport() {};
  __DeclareDeletedCtorsAndAssignments(UxReport)

  void init();

  void map();
  void unmap();

  uint32_t getRecordNumber() const;
  tRecord getRecord(uint32_t iIndex) const;
  tpBufferStruct* getRecords() const;

  tpBufferStruct* operator ->() { __AssertIfNot(_MappedData, "Report tries to access unmapped buffer."); return _MappedData; };
};

template<class tRecord, class tpBufferStruct>
void UxReport<tRecord, tpBufferStruct>::init()
{
  map();
  // Sets counter to zero
  _MappedData->_counter = 0;
  unmap();
}

template<class tRecord, class tpBufferStruct>
void UxReport<tRecord, tpBufferStruct>::map()
{
  __AssertIfNot(_MappedData==nullptr, "Invalid map call.");
  UxReportBase::map();
  _MappedData = (tpBufferStruct*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(tpBufferStruct), GL_MAP_WRITE_BIT);
  __CheckGLErrors;
}

template<class tRecord, class tpBufferStruct>
void UxReport<tRecord, tpBufferStruct>::unmap()
{
  __AssertIfNot(_MappedData!=nullptr, "Invalid unmap call.");
  UxReportBase::unmap();
  _MappedData = nullptr;
  __CheckGLErrors;
}

template<class tRecord, class tpBufferStruct>
tRecord UxReport<tRecord, tpBufferStruct>::getRecord(uint32_t iIndex) const
{
  __AssertIfNot(_MappedData!=nullptr && iIndex < _Size, "UxReport::getRecord: Report not mapped or invalid index");
  return *(tRecord*)_MappedData[iIndex * sizeof(tRecord)];
}

template<class tRecord, class tpBufferStruct>
uint32_t UxReport<tRecord, tpBufferStruct>::getRecordNumber() const
{
  __AssertIfNot(_MappedData!=nullptr, "Invalid call to getRecordNumber since report not mapped.");
  return _MappedData->_counter;
}

template<class tRecord, class tpBufferStruct>
tpBufferStruct* UxReport<tRecord, tpBufferStruct>::getRecords() const
{
  __AssertIfNot(_MappedData!=nullptr, "Invalid call to getRecords since report not mapped.");
  return (tpBufferStruct*)_MappedData;
}
