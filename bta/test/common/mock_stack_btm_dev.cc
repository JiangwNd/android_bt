/*
 * Copyright 2021 The Android Open Source Project
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

/*
 * Generated mock file from original source file
 *   Functions generated:12
 */

#include <cstdint>
#include <map>
#include <string>

extern std::map<std::string, int> mock_function_count_map;

#include "stack/btm/security_device_record.h"
#include "stack/include/bt_types.h"
#include "types/raw_address.h"

#ifndef UNUSED_ATTR
#define UNUSED_ATTR
#endif

bool BTM_SecAddDevice(const RawAddress& bd_addr, DEV_CLASS dev_class,
                      BD_NAME bd_name, uint8_t* features, LinkKey* p_link_key,
                      uint8_t key_type, uint8_t pin_length) {
  mock_function_count_map[__func__]++;
  return false;
}
bool BTM_SecDeleteDevice(const RawAddress& bd_addr) {
  mock_function_count_map[__func__]++;
  return false;
}
void wipe_secrets_and_remove(tBTM_SEC_DEV_REC* p_dev_rec) {
  mock_function_count_map[__func__]++;
}
char* BTM_SecReadDevName(const RawAddress& bd_addr) {
  mock_function_count_map[__func__]++;
  return nullptr;
}
void BTM_SecClearSecurityFlags(const RawAddress& bd_addr) {
  mock_function_count_map[__func__]++;
}
