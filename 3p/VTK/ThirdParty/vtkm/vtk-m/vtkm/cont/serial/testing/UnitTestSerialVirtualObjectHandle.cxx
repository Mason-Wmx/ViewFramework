//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2017 National Technology & Engineering Solutions of Sandia, LLC (NTESS).
//  Copyright 2017 UT-Battelle, LLC.
//  Copyright 2017 Los Alamos National Security.
//
//  Under the terms of Contract DE-NA0003525 with NTESS,
//  the U.S. Government retains certain rights in this software.
//
//  Under the terms of Contract DE-AC52-06NA25396 with Los Alamos National
//  Laboratory (LANL), the U.S. Government retains certain rights in
//  this software.
//============================================================================

// Make sure that the tested code is using the device adapter specified. This
// is important in the long run so we don't, for example, use the CUDA device
// for a part of an operation where the TBB device was specified.
#define VTKM_DEVICE_ADAPTER VTKM_DEVICE_ADAPTER_ERROR

#include <vtkm/cont/RuntimeDeviceTracker.h>
#include <vtkm/cont/testing/TestingVirtualObjectHandle.h>

namespace
{

void TestVirtualObjectHandle()
{
  using DeviceAdapterList = vtkm::ListTagBase<vtkm::cont::DeviceAdapterTagSerial>;
  vtkm::cont::testing::TestingVirtualObjectHandle<DeviceAdapterList>::Run();
}

} // anonymous namespace

int UnitTestSerialVirtualObjectHandle(int, char* [])
{
  auto tracker = vtkm::cont::GetGlobalRuntimeDeviceTracker();
  tracker.ForceDevice(vtkm::cont::DeviceAdapterTagSerial{});
  return vtkm::cont::testing::Testing::Run(TestVirtualObjectHandle);
}
