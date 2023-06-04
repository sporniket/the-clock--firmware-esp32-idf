// Copyright 2023 David SPORN
// ---
// This file is part of 'Wifi Simplist'.
// ---
// 'Wifi Simplist' is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// 'Wifi Simplist' is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'Wifi Simplist'. If not, see <https://www.gnu.org/licenses/>. 
#include "WifiCredentials.hpp"
#include <unity.h>

/**
 * @brief Before test
 */
void setUp(void) {}

/**
 * @brief After test.
 */
void tearDown(void) {}

uint8_t dummySsid[MAX_LENGTH_OF_SSID] = "The dummy ssid";
uint8_t dummyKey[MAX_LENGTH_OF_KEYPASS] = "One dummy key pass";
uint8_t dummyKey2[MAX_LENGTH_OF_KEYPASS] = "Another dummy key pass";

bool shouldHaveSameRank(WifiCredentials *wc1, WifiCredentials *wc2) {
  return false == (WifiCredentials::compareByRank(wc1, wc2) ||
                   WifiCredentials::compareByRank(wc2, wc1));
}

void test_simpleConstructor() {
  // Prepare

  // Execute
  WifiCredentials test(dummySsid, dummyKey, WifiKeyType::PASSWORD);

  // Verify
  TEST_ASSERT_EQUAL_INT(WifiKeyType::PASSWORD, test.getKeyType());
  TEST_ASSERT_EQUAL_UINT8_ARRAY(dummySsid, test.getSsid(), MAX_LENGTH_OF_SSID);
  TEST_ASSERT_TRUE(dummySsid != test.getSsid());
  TEST_ASSERT_EQUAL_UINT8_ARRAY(dummyKey, test.getKey(), MAX_LENGTH_OF_KEYPASS);
  TEST_ASSERT_TRUE(dummyKey != test.getKey());
}

void test_copyConstructor() {
  // Prepare
  WifiCredentials dummy(dummySsid, dummyKey, WifiKeyType::PASSWORD);

  // Execute
  WifiCredentials test(dummy);

  // Verify
  TEST_ASSERT_EQUAL_INT(WifiKeyType::PASSWORD, test.getKeyType());
  TEST_ASSERT_EQUAL_UINT8_ARRAY(dummySsid, test.getSsid(), MAX_LENGTH_OF_SSID);
  TEST_ASSERT_TRUE(dummySsid != test.getSsid());
  TEST_ASSERT_TRUE(dummy.getSsid() != test.getSsid());
  TEST_ASSERT_EQUAL_UINT8_ARRAY(dummyKey, test.getKey(), MAX_LENGTH_OF_KEYPASS);
  TEST_ASSERT_TRUE(dummyKey != test.getKey());
  TEST_ASSERT_TRUE(dummy.getKey() != test.getKey());

  // -- verify queries between instances
  TEST_ASSERT_TRUE(shouldHaveSameRank(&test, &dummy));
  TEST_ASSERT_TRUE(dummy.isSameSsid(&dummy));
}

void test_rankManagementAndComparison() {
  // Prepare
  WifiCredentials dummy(dummySsid, dummyKey, WifiKeyType::PASSWORD);
  WifiCredentials test(dummy);

  // ========[ STEP 1 ]========
  // Execute
  test.rankDown(); // test comes "after" dummy

  // Verify
  TEST_ASSERT_TRUE(WifiCredentials::compareByRank(&dummy, &test));
  TEST_ASSERT_FALSE(WifiCredentials::compareByRank(&test, &dummy));

  // ========[ STEP 2 ]========
  // Execute
  dummy.rankDown(); // test and dummy are at the same place

  // Verify
  TEST_ASSERT_FALSE(WifiCredentials::compareByRank(&dummy, &test));
  TEST_ASSERT_FALSE(WifiCredentials::compareByRank(&test, &dummy));

  // ========[ STEP 3 ]========
  // Execute
  dummy.rankDown(); // test comes "before" dummy

  // Verify
  TEST_ASSERT_FALSE(WifiCredentials::compareByRank(&dummy, &test));
  TEST_ASSERT_TRUE(WifiCredentials::compareByRank(&test, &dummy));

  // ========[ STEP 4 ]========
  // Execute
  dummy.rankFirst(); // test comes "after" dummy

  // Verify
  TEST_ASSERT_TRUE(WifiCredentials::compareByRank(&dummy, &test));
  TEST_ASSERT_FALSE(WifiCredentials::compareByRank(&test, &dummy));

  // ========[ STEP 5 ]========
  // Execute
  test.rankUp(); // test and dummy are at the same place

  // Verify
  TEST_ASSERT_FALSE(WifiCredentials::compareByRank(&dummy, &test));
  TEST_ASSERT_FALSE(WifiCredentials::compareByRank(&test, &dummy));
}

void test_changeOfKey() {
  // Prepare
  WifiCredentials test(dummySsid, dummyKey, WifiKeyType::PASSWORD);

  // Execute
  test.setKeyType(WifiKeyType::PRESHAREDKEY);
  test.setKey(dummyKey2);

  // Verify
  TEST_ASSERT_EQUAL_INT(WifiKeyType::PRESHAREDKEY, test.getKeyType());
  TEST_ASSERT_EQUAL_UINT8_ARRAY(dummyKey2, test.getKey(),
                                MAX_LENGTH_OF_KEYPASS);
  TEST_ASSERT_TRUE(dummyKey2 != test.getKey());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_simpleConstructor);
  RUN_TEST(test_copyConstructor);
  RUN_TEST(test_rankManagementAndComparison);
  RUN_TEST(test_changeOfKey);
  UNITY_END();
}
