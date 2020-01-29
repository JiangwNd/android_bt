/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/**
 * Legacy stack manipulation methods to allow the legacy stack to start
 * and stop the stack, and to provide Gd shim stack module API access.
 */
namespace bluetooth {
namespace shim {

struct IAdvertising;
struct IController;
struct IConnectability;
struct IDiscoverability;
struct IDumpsys;
struct IHciLayer;
struct IInquiry;
struct IName;
struct IL2cap;
struct IPage;
struct IScanning;
struct ISecurity;
struct IStorage;

struct IStack {
  virtual void Start() = 0;
  virtual void Stop() = 0;

  virtual IAdvertising* GetAdvertising() = 0;
  virtual IController* GetController() = 0;
  virtual IConnectability* GetConnectability() = 0;
  virtual IDiscoverability* GetDiscoverability() = 0;
  virtual IDumpsys* GetDumpsys() = 0;
  virtual IHciLayer* GetHciLayer() = 0;
  virtual IInquiry* GetInquiry() = 0;
  virtual IName* GetName() = 0;
  virtual IL2cap* GetL2cap() = 0;
  virtual IPage* GetPage() = 0;
  virtual IScanning* GetScanning() = 0;
  virtual ISecurity* GetSecurity() = 0;
  virtual IStorage* GetStorage() = 0;

  virtual ~IStack() {}
};

IStack* GetGabeldorscheStack();

}  // namespace shim
}  // namespace bluetooth