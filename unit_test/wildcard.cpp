#include "gtest/gtest.h"

#include "Wildcard.hpp"

TEST(wildcard, construct) {
	Wildcard	w;

	ASSERT_EQ(w.getString(), "*");

	Wildcard	w2("*.a");

	ASSERT_TRUE(w2 == "abc.a");
	ASSERT_TRUE(w2 == "asdasdDDA.a");
	ASSERT_TRUE(w2 == ".a");
	ASSERT_TRUE(w2 == "***.a");
	ASSERT_FALSE(w2 == "abc.ab");

	Wildcard	w3("*abc*cd*.ru");

	ASSERT_TRUE(w3 != "");
	ASSERT_TRUE(w3 != "abc.ru");
	ASSERT_FALSE(w3 != "abccd.ru");

	ASSERT_EQ(w3.getString(), "*abc*cd*.ru");
}

TEST(wildcard, oper_eq) {
	Wildcard	w1("123*");
	Wildcard	w2("123*");
	Wildcard	w3("123*5");

	ASSERT_EQ(w1, w2);
	ASSERT_EQ(w1, w1);
	ASSERT_EQ(w2, w2);

	ASSERT_FALSE(w1 == w3);
	ASSERT_FALSE(w2 != w1);
}
