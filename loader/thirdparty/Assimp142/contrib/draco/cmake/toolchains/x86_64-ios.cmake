# Copyright 2021 The Draco Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

if(DRACO_CMAKE_TOOLCHAINS_X86_64_IOS_CMAKE_)
  return()
endif()
set(DRACO_CMAKE_TOOLCHAINS_X86_64_IOS_CMAKE_ 1)

if(XCODE)
  message(FATAL_ERROR "This toolchain does not support Xcode.")
endif()

set(CMAKE_SYSTEM_PROCESSOR "x86_64")
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_OSX_SDK "iphonesimulator")

include("arm-ios-common.cmake")