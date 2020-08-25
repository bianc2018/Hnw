#include <iostream>

#include <gtest/gtest.h>

#include "./util/http_util.hpp"

// case1
TEST(HttpUtil, base64) 
{
    std::string data = "test base64";
    auto encode =hnw::util::encode_base64(data);
    auto decode = hnw::util::decode_base64(encode);
    EXPECT_EQ(encode, "dGVzdCBiYXNlNjQ=");
    EXPECT_EQ(decode, data);

    EXPECT_EQ("", hnw::util::encode_base64(""));
}

TEST(HttpUtil, encode_base64)
{
    EXPECT_EQ("", hnw::util::encode_base64(""));
    EXPECT_EQ("PT09", hnw::util::encode_base64("==="));
    EXPECT_EQ("MDAwMA==", hnw::util::encode_base64("0000"));
}

TEST(HttpUtil, decode_base64)
{
    EXPECT_EQ("", hnw::util::decode_base64(""));
    EXPECT_EQ("", hnw::util::decode_base64("\t\n"));
    EXPECT_EQ("===", hnw::util::decode_base64("PT09"));
    EXPECT_EQ("111111111111111111111111111111111", hnw::util::decode_base64("MTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTEx"));
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}