// ==============================
// File:                        objects/objects.cp
// Project:                     Albert
//
// Copyright 2003-2008 by Matthias Melcher (albert@matthiasm.com).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================


#include <Albert/objects/objects.h>
#include <Albert/objects/TTask.h>
#include <Albert/objects/TMonitor.h>
#include <Albert/objects/TObjectTable.h>
#include <Albert/sys/globals.h>


namespace Albert {

  
ObjectId LocalToGlobalId(ObjectId localId)
{
  TTask *currentTask = GetCurrentTask();

  switch (localId) 
  {
  case kBuiltInSMemMsgId:
    if (currentTask) 
      return currentTask->GetSharedMemMsgId();
    break;

  case kBuiltInSMemId:
    if (currentTask) 
      return currentTask->GetSharedMemId();
    break;

  case kBuiltInSMemMonitorFaultId: 
    {
      ObjectId monitorId = currentTask->GetMonitorId();
      if (ObjectType(monitorId)==kMonitorType) {
        TObjectTable *objectTable = GetObjectTable();
        TMonitor *monitor = (TMonitor*)objectTable->Get(monitorId);
        if (monitor)
          return monitor->GetMsgId();
      }
    }
    break;
  }

  return localId;
}

  
/**
 * Convert a kernel object’s token to the object itself.
 * 
 * \param[in]   inType  the object’s expected type
 * \param[in]   inId    its token
 * \param[out]  outObj  a pointer to its pointer
 * \return error code
 */
NewtonErr ConvertIdToObj(KernelTypes inType, ObjectId inId, TObject **outObj)
{
  NewtonErr	err = noErr;
  TObject *obj = IdToObj(inType, LocalToGlobalId(inId));    
  
  if (!obj)
    err = kOSErrBadObjectId;
  if (outObj)
     setMem32((KUInt32)outObj, (KUInt32)obj);
     // *outObj = obj;
  
  return err;
}


TObject *IdToObj(KernelTypes inType, ObjectId inId)
{ 
  return (ObjectType(inId) == inType) ? GetObjectTable()->Get(inId) : nil; 
}
  
  
}

