/******************************************************************************
 *
 *  Copyright 2018 The Android Open Source Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include <gtest/gtest.h>

#include <base/logging.h>
#include <base/strings/string_number_conversions.h>
#include "gatt/database.h"
#include "gatt/database_builder.h"
#include "stack/include/gattdefs.h"

using bluetooth::Uuid;

namespace gatt {

namespace {
const Uuid PRIMARY_SERVICE = Uuid::From16Bit(GATT_UUID_PRI_SERVICE);
const Uuid SECONDARY_SERVICE = Uuid::From16Bit(GATT_UUID_SEC_SERVICE);
const Uuid INCLUDE = Uuid::From16Bit(GATT_UUID_INCLUDE_SERVICE);
const Uuid CHARACTERISTIC = Uuid::From16Bit(GATT_UUID_CHAR_DECLARE);
const Uuid CHARACTERISTIC_EXTENDED_PROPERTIES =
    Uuid::From16Bit(GATT_UUID_CHAR_EXT_PROP);

Uuid SERVICE_1_UUID = Uuid::FromString("1800");
Uuid SERVICE_2_UUID = Uuid::FromString("1801");
Uuid SERVICE_1_CHAR_1_UUID = Uuid::FromString("2a00");
Uuid SERVICE_1_CHAR_1_DESC_1_UUID = Uuid::FromString("2902");
}  // namespace

/* This test makes sure that each possible GATT cache element is properly
 * serialized into StoredAttribute */
TEST(GattDatabaseTest, serialize_deserialize_binary_test) {
  DatabaseBuilder builder;
  builder.AddService(0x0001, 0x000f, SERVICE_1_UUID, true);
  builder.AddService(0x0010, 0x001f, SERVICE_2_UUID, false);
  builder.AddIncludedService(0x0002, SERVICE_2_UUID, 0x0010, 0x001f);
  builder.AddCharacteristic(0x0003, 0x0004, SERVICE_1_CHAR_1_UUID, 0x02);
  builder.AddDescriptor(0x0005, SERVICE_1_CHAR_1_DESC_1_UUID);
  builder.AddDescriptor(0x0006, CHARACTERISTIC_EXTENDED_PROPERTIES);

  // Set value of only «Characteristic Extended Properties» descriptor
  builder.SetValueOfDescriptors({0x0001});

  Database db = builder.Build();
  std::vector<StoredAttribute> serialized = db.Serialize();

  // Primary Service
  EXPECT_EQ(serialized[0].handle, 0x0001);
  EXPECT_EQ(serialized[0].type, PRIMARY_SERVICE);
  EXPECT_EQ(serialized[0].value.service.uuid, SERVICE_1_UUID);
  EXPECT_EQ(serialized[0].value.service.end_handle, 0x000f);

  // Secondary Service
  EXPECT_EQ(serialized[1].handle, 0x0010);
  EXPECT_EQ(serialized[1].type, SECONDARY_SERVICE);
  EXPECT_EQ(serialized[1].value.service.uuid, SERVICE_2_UUID);
  EXPECT_EQ(serialized[1].value.service.end_handle, 0x001f);

  // Included Service
  EXPECT_EQ(serialized[2].handle, 0x0002);
  EXPECT_EQ(serialized[2].type, INCLUDE);
  EXPECT_EQ(serialized[2].value.included_service.handle, 0x0010);
  EXPECT_EQ(serialized[2].value.included_service.end_handle, 0x001f);
  EXPECT_EQ(serialized[2].value.included_service.uuid, SERVICE_2_UUID);

  // Characteristic
  EXPECT_EQ(serialized[3].handle, 0x0003);
  EXPECT_EQ(serialized[3].type, CHARACTERISTIC);
  EXPECT_EQ(serialized[3].value.characteristic.properties, 0x02);
  EXPECT_EQ(serialized[3].value.characteristic.value_handle, 0x0004);
  EXPECT_EQ(serialized[3].value.characteristic.uuid, SERVICE_1_CHAR_1_UUID);

  // Descriptor
  EXPECT_EQ(serialized[4].handle, 0x0005);
  EXPECT_EQ(serialized[4].type, SERVICE_1_CHAR_1_DESC_1_UUID);

  // Characteristic Extended Properties Descriptor
  EXPECT_EQ(serialized[5].handle, 0x0006);
  EXPECT_EQ(serialized[5].type, CHARACTERISTIC_EXTENDED_PROPERTIES);
  EXPECT_EQ(serialized[5].value.characteristic_extended_properties, 0x0001);
}

/* This test makes sure that Service represented in StoredAttribute have proper
 * binary format. */
TEST(GattCacheTest, stored_attribute_to_binary_service_test) {
  StoredAttribute attr;

  /* make sure padding at end of union is cleared */
  memset(&attr, 0, sizeof(attr));

  attr = {
      .handle = 0x0001,
      .type = PRIMARY_SERVICE,
      .value = {.service = {.uuid = Uuid::FromString("1800"),
                            .end_handle = 0x001c}},
  };

  constexpr size_t len = sizeof(StoredAttribute);
  // clang-format off
  uint8_t binary_form[len] = {
      /*handle */ 0x01, 0x00,
      /* type*/ 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB,
      /* service uuid */ 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB,
      /* end handle */ 0x1C, 0x00,
      /* cleared padding at end of union*/ 0x00, 0x00};
  // clang-format on

  // useful for debugging:
  // LOG(ERROR) << " " << base::HexEncode(&attr, len);
  EXPECT_EQ(memcmp(binary_form, &attr, len), 0);
}

/* This test makes sure that Service represented in StoredAttribute have proper
 * binary format. */
TEST(GattCacheTest, stored_attribute_to_binary_included_service_test) {
  StoredAttribute attr;

  /* make sure padding at end of union is cleared */
  memset(&attr, 0, sizeof(attr));

  attr = {
      .handle = 0x0001,
      .type = INCLUDE,
      .value = {.included_service =
                    {
                        .handle = 0x0010,
                        .end_handle = 0x001f,
                        .uuid = Uuid::FromString("1801"),
                    }},
  };

  constexpr size_t len = sizeof(StoredAttribute);
  // clang-format off
  uint8_t binary_form[len] = {
      /*handle */ 0x01, 0x00,
      /* type*/ 0x00, 0x00, 0x28, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB,
      /* handle */ 0x10, 0x00,
      /* end handle */ 0x1f, 0x00,
      /* service uuid */ 0x00, 0x00, 0x18, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
  // clang-format on

  // useful for debugging:
  // LOG(ERROR) << " " << base::HexEncode(&attr, len);
  EXPECT_EQ(memcmp(binary_form, &attr, len), 0);
}

/* This test makes sure that «Characteristic Extended Properties» descriptor
 * represented in StoredAttribute have proper binary format. */
TEST(GattCacheTest, stored_attribute_to_binary_characteristic_test) {
  StoredAttribute attr;

  /* make sure padding at end of union is cleared */
  memset(&attr, 0, sizeof(attr));

  attr = {
      .handle = 0x0002,
      .type = CHARACTERISTIC,
      .value = {.characteristic = {.properties = 0x02,
                                   .value_handle = 0x0003,
                                   .uuid = Uuid::FromString("2a00")}},
  };

  constexpr size_t len = sizeof(StoredAttribute);
  // clang-format off
  uint8_t binary_form[len] = {
      /*handle */ 0x02, 0x00,
      /* type */ 0x00, 0x00, 0x28, 0x03, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB,
      /* properties */ 0x02,
      /* after properties there is one byte padding. This might cause troube
         on other platforms, investigate if it's ever a problem */ 0x00,
      /* value handle */ 0x03, 0x00,
      /* uuid */ 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
  // clang-format on

  // useful for debugging:
  // LOG(ERROR) << " " << base::HexEncode(&attr, len);
  EXPECT_EQ(memcmp(binary_form, &attr, len), 0);
}

/* This test makes sure that Descriptor represented in StoredAttribute have
 * proper binary format. */
TEST(GattCacheTest, stored_attribute_to_binary_descriptor_test) {
  StoredAttribute attr;

  /* make sure padding at end of union is cleared */
  memset(&attr, 0, sizeof(attr));

  attr = {.handle = 0x0003, .type = Uuid::FromString("2902"), .value = {}};

  constexpr size_t len = sizeof(StoredAttribute);
  // clang-format off
  uint8_t binary_form[len] = {
      /*handle */ 0x03, 0x00,
      /* type */ 0x00, 0x00, 0x29, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB,
      /* clear padding    */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  // clang-format on

  // useful for debugging:
  // LOG(ERROR) << " " << base::HexEncode(&attr, len);
  EXPECT_EQ(memcmp(binary_form, &attr, len), 0);
}

// Example from Bluetooth SPEC V5.2, Vol 3, Part G, APPENDIX B
TEST(GattDatabaseTest, hash_test) {
  DatabaseBuilder builder;
  builder.AddService(0x0001, 0x0005, Uuid::From16Bit(0x1800), true);
  builder.AddService(0x0006, 0x000D, Uuid::From16Bit(0x1801), true);
  builder.AddService(0x000E, 0x0013, Uuid::From16Bit(0x1808), true);
  builder.AddService(0x0014, 0xFFFF, Uuid::From16Bit(0x180F), false);

  builder.AddCharacteristic(0x0002, 0x0003, Uuid::From16Bit(0x2A00), 0x0A);
  builder.AddCharacteristic(0x0004, 0x0005, Uuid::From16Bit(0x2A01), 0x02);

  builder.AddCharacteristic(0x0007, 0x0008, Uuid::From16Bit(0x2A05), 0x20);
  builder.AddDescriptor(0x0009, Uuid::From16Bit(0x2902));
  builder.AddCharacteristic(0x000A, 0x000B, Uuid::From16Bit(0x2B29), 0x0A);
  builder.AddCharacteristic(0x000C, 0x000D, Uuid::From16Bit(0x2B2A), 0x02);

  builder.AddIncludedService(0x000F, Uuid::From16Bit(0x180F), 0x0014, 0x0016);
  builder.AddCharacteristic(0x0010, 0x0011, Uuid::From16Bit(0x2A18), 0xA2);
  builder.AddDescriptor(0x0012, Uuid::From16Bit(0x2902));
  builder.AddDescriptor(0x0013, Uuid::From16Bit(0x2900));

  builder.AddCharacteristic(0x0015, 0x0016, Uuid::From16Bit(0x2A19), 0x02);

  Database db = builder.Build();

  // Big endian example from Bluetooth SPEC V5.2, Vol 3, Part G, APPENDIX B
  Octet16 expected_hash{0xF1, 0xCA, 0x2D, 0x48, 0xEC, 0xF5, 0x8B, 0xAC,
                        0x8A, 0x88, 0x30, 0xBB, 0xB9, 0xFB, 0xA9, 0x90};

  Octet16 hash = db.Hash();
  // Convert output hash from little endian to big endian
  std::reverse(hash.begin(), hash.end());

  EXPECT_EQ(hash, expected_hash);
}

/* This test makes sure that Descriptor represented in StoredAttribute have
 * proper binary format. */
TEST(GattCacheTest,
     stored_attribute_to_binary_characteristic_extended_properties_test) {
  StoredAttribute attr;

  /* make sure padding at end of union is cleared */
  memset(&attr, 0, sizeof(attr));

  attr = {.handle = 0x0003,
          .type = Uuid::FromString("2900"),
          .value = {.characteristic_extended_properties = 0x0001}};

  constexpr size_t len = sizeof(StoredAttribute);
  // clang-format off
  uint8_t binary_form[len] = {
      /*handle */ 0x03, 0x00,
      /* type */ 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB,
      /* characteristic extended properties */ 0x01, 0x00,
      /* clear padding    */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00};
  // clang-format on

  // useful for debugging:
  // LOG(ERROR) << " " << base::HexEncode(&attr, len);
  EXPECT_EQ(memcmp(binary_form, &attr, len), 0);
}
}  // namespace gatt