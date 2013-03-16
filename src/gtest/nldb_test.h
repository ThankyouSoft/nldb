/*
 * (C) Copyright 2012 ThankyouSoft (http://ThankyouSoft.com/) and Nanolat(http://Nanolat.com).
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the NOSL(Nanolat Open Source License) v1.0.
 * 
 * Simple Description :
 * - If you are in USA, Canada, or Germany, it is *likely* that you get free LGPL v2.1.
 * - In other countries, you are *likely* under GPL v3, but you can by LGPL v2.1 from Nanolat Co., Ltd.
 * - For free software, you *definitely* get free LGPL v2.1 whereever you are.
 * 
 * Detailed Description :
 * - In the hope of seeing increasing open source contributions in countries 
 *   that do not have enough open source contributors, Nanolat Open Source License 
 *   provides free LGPL v2.1 only to companies and people of countries 
 *   that have at least three KudoRank 10 contributors in http://www.ohloh.net/people. 
 *   The list of countries are updated by the beginning of a year. 
 *   In 2013, these countries are USA, Canada, and Germany.
 *
 * - For companies and people of other countries, you are under GPL v3.0, 
 *   but you can buy LGPL v2.1 license for a product from Nanolat Co., Ltd.
 *
 * - However, for non-commercial usage, you get free LGPL v2.1 whereever you are.
 *
 * - A company is of a country if the head of office is in the country.
 *
 * - A person is of a country if the nationality of the person is the country.
 *
 * - A country has a KudoRank 10 contributor 
 *   if the contributor's location is the country in http://www.ohloh.net/.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * Contributors:
 *     Kangmo Kim
 */

 
#ifndef NLDB_TEST_H_
#define NLDB_TEST_H_

#include <gtest/gtest.h>

// for htonl
#include <arpa/inet.h>

#include <nldb/nldb_common.h>
#include <nldb/nldb.h>

#include <stdint.h>
using namespace std;

const int TEST_DB_ID = 1;
const int VOLATILE_TABLE_ID = 1;
const int PERSISTENT_TABLE_ID = 2;

#define KEY_SIZE = 8;
#define VALUE_SIZE = 256;

typedef struct tbl_key {
	uint32_t key1;
	uint32_t key2;
} tbl_key ;
typedef struct tbl_value{
	long value;
} tbl1_value ;


class NLDBTest : public testing::Test {
  protected:
    virtual void SetUp() {
    	ASSERT_TRUE( nldb_init() == 0 );

    	ASSERT_TRUE( nldb_db_create( TEST_DB_ID ) == 0 );

    	ASSERT_TRUE( nldb_db_open( TEST_DB_ID, NULL/*master*/, NULL/*slave*/, &db ) == 0 );

    	ASSERT_TRUE( nldb_table_create(db, VOLATILE_TABLE_ID, NLDB_TABLE_VOLATILE) == 0 );

    	ASSERT_TRUE( nldb_table_create(db, PERSISTENT_TABLE_ID, NLDB_TABLE_PERSISTENT) == 0 );

    	ASSERT_TRUE( nldb_table_open(db, VOLATILE_TABLE_ID, &vol_table) == 0 );

    	ASSERT_TRUE( nldb_table_open(db, PERSISTENT_TABLE_ID, &pers_table) == 0 );

    	ASSERT_TRUE( nldb_tx_init( db, &tx ) == 0 );
    }

    virtual void TearDown() {
    	ASSERT_TRUE( nldb_tx_destroy( tx ) == 0 );

    	ASSERT_TRUE( nldb_table_close(vol_table) == 0 );

    	ASSERT_TRUE( nldb_table_close(pers_table) == 0 );

    	ASSERT_TRUE( nldb_table_drop(db, VOLATILE_TABLE_ID) == 0 );

    	ASSERT_TRUE( nldb_table_drop(db, PERSISTENT_TABLE_ID) == 0 );

    	ASSERT_TRUE( nldb_db_close( db ) == 0 );

    	ASSERT_TRUE( nldb_db_drop( TEST_DB_ID ) == 0 );

    	ASSERT_TRUE( nldb_destroy() == 0 );
    }

	nldb_db_t db;
	nldb_table_t vol_table;
	nldb_table_t pers_table;
	nldb_tx_t tx;
};

// We can't call htonl within the list of initializing values of global variables.
// However, macro function can be used, so we define htonl_macro here.
#define switch_endian32(v) (unsigned int)( ((((unsigned int)v)&0x000000FFU) << 24) | \
                                           ((((unsigned int)v)&0x0000FF00U) << 8)  | \
                                           ((((unsigned int)v)&0x00FF0000U) >> 8)    | \
                                           ((((unsigned int)v)&0xFF000000U) >> 24) )


#define DEFINE_KEY_VALUE(var) \
extern tbl_key      var##_key; \
extern tbl_value    var##_value; \
extern nldb_key_t   var##_nldb_key; \
extern nldb_value_t var##_nldb_value;

// About using htonl : Keys are stored in big endian encoding in NLDB.
// This is to quickly compare keys by calling memcmp on keys whenever NLDB searches a key.
#define DECLARE_KEY_VALUE(var, k1, k2, v) \
tbl_key      var##_key = {switch_endian32(k1), switch_endian32(k2)}; \
tbl_value    var##_value = {v}; \
nldb_key_t   var##_nldb_key = { & var##_key, sizeof(var##_key) }; \
nldb_value_t var##_nldb_value = { & var##_value, sizeof(var##_value) };

#define KEY(var)    var##_nldb_key
#define VALUE(var)  var##_nldb_value

#define IS_VALUE_EQUAL(nldbValue,expectedValue) (memcmp(nldbValue.data, &expectedValue, v.length) == 0)
#define IS_KEY_EQUAL(var, expectedKey) ( (var##_nldb_key.length == expectedKey.length) && (memcmp(var##_nldb_key.data, expectedKey.data, expectedKey.length) == 0) )


#endif /* NLDB_TEST_H_ */
