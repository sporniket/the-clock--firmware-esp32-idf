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
#include "WifiCredentialsRegistry.hpp"
#include <unity.h>

uint8_t dummySsid[MAX_LENGTH_OF_SSID] = "The dummy ssid";
uint8_t dummySsid2[MAX_LENGTH_OF_SSID] = "The dummy ssid - 2";
uint8_t dummySsid3[MAX_LENGTH_OF_SSID] = "The dummy ssid - 3";
uint8_t dummySsid4[MAX_LENGTH_OF_SSID] = "The dummy ssid - 4";
uint8_t dummyKey[MAX_LENGTH_OF_KEYPASS] = "One dummy key pass";

/**
 * @brief Before test
 */
void setUp(void) {}

/**
 * @brief After test.
 */
void tearDown(void) {}

void test_shouldBeEmptyAfterCreation() {
  // Prepare

  // Execute
  WifiCredentialsRegistry test;

  // Verify
  TEST_ASSERT_EQUAL_UINT8(0, test.getSize());
  TEST_ASSERT_FALSE(test.hasNext());
  TEST_ASSERT_NULL(test.next());
}

void test_shouldRegisterCredentials() {
  // Prepare
  WifiCredentials wc1(dummySsid, dummyKey, PRESHAREDKEY);
  WifiCredentialsRegistry test;

  // Execute
  test.put(&wc1);

  // Verify
  TEST_ASSERT_EQUAL_UINT8(1, test.getSize());
  TEST_ASSERT_TRUE(test.hasNext());
  WifiCredentials *creds = test.next();
  TEST_ASSERT_FALSE_MESSAGE(
      creds == &wc1, "Registry MUST clone credentials to be put inside it.");
  TEST_ASSERT_TRUE(wc1.isSameSsid(creds));
  TEST_ASSERT_EQUAL_UINT8(wc1.getKeyType(), creds->getKeyType());
  TEST_ASSERT_EQUAL_UINT8_ARRAY(wc1.getKey(), creds->getKey(),
                                MAX_LENGTH_OF_KEYPASS);
}

void test_shouldReturnNewlyRegisteredCredentialsFirstThenTheOthers() {
  // Prepare
  WifiCredentials wc1(dummySsid, dummyKey, PRESHAREDKEY);
  WifiCredentials wc2(dummySsid2, dummyKey, PRESHAREDKEY);
  WifiCredentialsRegistry test;
  test.put(&wc1);

  // Execute
  test.put(&wc2);

  // Verify
  TEST_ASSERT_EQUAL_UINT8(2, test.getSize());
  TEST_ASSERT_TRUE(test.hasNext());
  WifiCredentials *creds = test.next();
  TEST_ASSERT_TRUE(wc2.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc1.isSameSsid(creds));
  TEST_ASSERT_FALSE(test.hasNext());
}

void test_shouldForgetOldCredentialsWhenAddingCredentialsToFullRegistry() {
  // Prepare
  WifiCredentials wc1(dummySsid, dummyKey, PRESHAREDKEY);
  WifiCredentials wc2(dummySsid2, dummyKey, PRESHAREDKEY);
  WifiCredentials wc3(dummySsid3, dummyKey, PRESHAREDKEY);
  WifiCredentials wc4(dummySsid4, dummyKey, PRESHAREDKEY);
  WifiCredentialsRegistry test;
  test.put(&wc1);
  test.put(&wc2);
  test.put(&wc3);

  // Execute
  test.put(&wc4);

  // Verify
  TEST_ASSERT_EQUAL_UINT8(3, test.getSize());
  TEST_ASSERT_TRUE(test.hasNext());
  WifiCredentials *creds = test.next();
  TEST_ASSERT_TRUE(wc4.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc3.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc2.isSameSsid(creds));
  TEST_ASSERT_FALSE(test.hasNext());
}

void test_shouldChangeOrderWithSetPreferred() {
  // Prepare
  WifiCredentials wc1(dummySsid, dummyKey, PRESHAREDKEY);
  WifiCredentials wc2(dummySsid2, dummyKey, PRESHAREDKEY);
  WifiCredentials wc3(dummySsid3, dummyKey, PRESHAREDKEY);
  WifiCredentialsRegistry test;
  test.put(&wc1);
  test.put(&wc2);
  test.put(&wc3);

  // Execute
  test.setPreferred(&wc2);

  // Verify
  TEST_ASSERT_EQUAL_UINT8(3, test.getSize());
  TEST_ASSERT_TRUE(test.hasNext());
  WifiCredentials *creds = test.next();
  TEST_ASSERT_TRUE(wc2.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc3.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc1.isSameSsid(creds));
  TEST_ASSERT_FALSE(test.hasNext());
}

void test_shouldReturnToBeginningOfSequenceWithRewind() {
  // Prepare
  WifiCredentials wc1(dummySsid, dummyKey, PRESHAREDKEY);
  WifiCredentials wc2(dummySsid2, dummyKey, PRESHAREDKEY);
  WifiCredentials wc3(dummySsid3, dummyKey, PRESHAREDKEY);
  WifiCredentialsRegistry test;
  test.put(&wc1);
  test.put(&wc2);
  test.put(&wc3);
  while (test.hasNext()) {
    test.next();
  }

  // Execute
  test.rewind();

  // Verify
  TEST_ASSERT_TRUE(test.hasNext());
  WifiCredentials *creds = test.next();
  TEST_ASSERT_TRUE(wc3.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc2.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc1.isSameSsid(creds));
  TEST_ASSERT_FALSE(test.hasNext());
}

void test_shouldRemoveSpecifiedCredentials() {
  // Prepare
  WifiCredentials wc1(dummySsid, dummyKey, PRESHAREDKEY);
  WifiCredentials wc2(dummySsid2, dummyKey, PRESHAREDKEY);
  WifiCredentials wc3(dummySsid3, dummyKey, PRESHAREDKEY);
  WifiCredentialsRegistry test;
  test.put(&wc1);
  test.put(&wc2);
  test.put(&wc3);

  // Execute
  test.remove(&wc2);

  // Verify
  TEST_ASSERT_EQUAL_UINT8(2, test.getSize());
  TEST_ASSERT_TRUE(test.hasNext());
  WifiCredentials *creds = test.next();
  TEST_ASSERT_TRUE(wc3.isSameSsid(creds));
  TEST_ASSERT_TRUE(test.hasNext());
  creds = test.next();
  TEST_ASSERT_TRUE(wc1.isSameSsid(creds));
  TEST_ASSERT_FALSE(test.hasNext());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_shouldBeEmptyAfterCreation);
  RUN_TEST(test_shouldRegisterCredentials);
  RUN_TEST(test_shouldReturnNewlyRegisteredCredentialsFirstThenTheOthers);
  RUN_TEST(test_shouldForgetOldCredentialsWhenAddingCredentialsToFullRegistry);
  RUN_TEST(test_shouldChangeOrderWithSetPreferred);
  RUN_TEST(test_shouldReturnToBeginningOfSequenceWithRewind);
  RUN_TEST(test_shouldRemoveSpecifiedCredentials);
  UNITY_END();
}
