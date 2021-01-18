/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2012-2021 University of California, Los Angeles
 *
 * This file is part of ndn-svs, synchronization library for distributed realtime
 * applications for NDN.
 *
 * ndn-svs library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, in version 2.1 of the License.
 *
 * ndn-svs library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 */

#include "logic.hpp"

#include "tests/boost-test.hpp"

namespace ndn {
namespace svs {
namespace test {

struct TestLogicFixture
{
  TestLogicFixture()
    : m_syncPrefix("/ndn/test")
    , m_logic(m_face, m_keyChain, m_syncPrefix, bind(&TestLogicFixture::update, this, _1))
  {
  }

  Face m_face;
  KeyChain m_keyChain;
  Name m_syncPrefix;
  Logic m_logic;

  std::vector<MissingDataInfo> missingData;

  void
  update(const std::vector<MissingDataInfo>& v)
  {
    for (auto m : v)
      missingData.push_back(m);
  }
};

BOOST_FIXTURE_TEST_SUITE(TestLogic, TestLogicFixture)

BOOST_AUTO_TEST_CASE(mergeStateVector)
{
  VersionVector v = m_logic.getState();
  BOOST_CHECK_EQUAL(v.get("one"), 0);
  BOOST_CHECK_EQUAL(v.get("two"), 0);
  BOOST_CHECK_EQUAL(v.get("three"), 0);
  BOOST_CHECK_EQUAL(missingData.size(), 0);

  VersionVector v1;
  v1.set("one", 1);
  v1.set("two", 2);
  m_logic.mergeStateVector(v1);

  v = m_logic.getState();
  BOOST_CHECK_EQUAL(v.get("one"), 1);
  BOOST_CHECK_EQUAL(v.get("two"), 2);
  BOOST_CHECK_EQUAL(v.get("three"), 0);
  BOOST_CHECK_EQUAL(missingData.size(), 2);

  VersionVector v2;
  v2.set("one", 1);
  v2.set("two", 1);
  v2.set("three", 3);
  missingData.clear();
  m_logic.mergeStateVector(v2);

  v = m_logic.getState();
  BOOST_CHECK_EQUAL(v.get("one"), 1);
  BOOST_CHECK_EQUAL(v.get("two"), 2);
  BOOST_CHECK_EQUAL(v.get("three"), 3);

  BOOST_CHECK_EQUAL(missingData.size(), 1);
  BOOST_CHECK_EQUAL(missingData[0].session, "three");
  BOOST_CHECK_EQUAL(missingData[0].low, 1);
  BOOST_CHECK_EQUAL(missingData[0].high, 3);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
} // namespace ndn
} // namespace ndn
