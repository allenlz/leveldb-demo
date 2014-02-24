#include <stdlib.h>
#include "util.h"
#include "custom_leveldb.h"

using namespace store;

leveldb::DB *g_db;
CustomComparator g_comparator;
CustomDeletePolicy g_delete_policy;
std::string g_data_dir = "./data/expired_time_demo";

int init() {
    leveldb::Options init_options;
    init_options.comparator = &g_comparator;
    init_options.delete_policy = &g_delete_policy;
    init_options.create_if_missing = true;
    leveldb::Status s = leveldb::DB::Open(init_options, g_data_dir.c_str(), &g_db);
    if (!s.ok()) {
        warning("db open failed:%s\n", s.ToString().c_str());
        return -1;
    }
    return 0;
}

void write_random() {
    std::string user_key = "box";
    string_random_append(&user_key, 1, 5);
    CustomKey key(user_key, time(NULL) + 3);
    leveldb::Slice value = leveldb::Slice(key.data(), key.size());
    leveldb::Status s = g_db->Put(leveldb::WriteOptions(),
        leveldb::Slice(key.data(), key.size()), value);
    if (!s.ok()) {
        warning("put failed: %s\n", s.ToString().c_str());
    } else {
        notice("put ok. key:%s, user_key_len:%lu, key_len:%lu\n",
            user_key.c_str(), user_key.size(), key.size());
    }
}

void multi_write(int n) {
    for (int i = 0; i < n; ++i) {
        write_random();
    }
}

void write(std::string key_str, int expired_time_s) {
    CustomKey key(key_str, expired_time_s);
    leveldb::Slice value = leveldb::Slice(key.data(), key.size());
    leveldb::Status s = g_db->Put(leveldb::WriteOptions(),
        leveldb::Slice(key.data(), key.size()), value);
    if (!s.ok()) {
        warning("put failed: %s\n", s.ToString().c_str());
    } else {
        notice("put ok. key:%s\n", key_str.c_str());
    }
}

void read(std::string key_str) {
    CustomKey key(key_str, 0);
    std::string value;
    std::string rkey_raw;
    leveldb::Status s = g_db->Get(leveldb::ReadOptions(),
        leveldb::Slice(key.data(), key.size()), &value, &rkey_raw);
    if (!s.ok()) {
        warning("get failed: %s\n", s.ToString().c_str());
    } else {
        CustomKey rkey;
        rkey.DecodeFrom(rkey_raw);
        notice("get ok. key:%s, rkey:%s, expired_time_s:%d, value:%s\n",
            key_str.data(), rkey.user_key().data(), rkey.expired_time_s(), value.data());
    }
}

void test1() {
    read("save");

    printf("test 1\n");
    write("box", time(NULL) + 1);
    read("box");
    sleep(1);
    read("box");

    printf("test 2\n");
    write("box", 0);
    read("box");

    printf("test 3\n");
    write("box", time(NULL) - 1);
    read("box");

    write("save", time(NULL) + 3);
}

void test2() {
    multi_write(100);

    write("box", 0);
    write("box", 0);
    write("box", 0);
    write("box", 0);
    write("box", 0);
    write("box", 0);
    write("box", 0);
    write("box", 0);
}

int main() {
    srand(time(NULL));

    if (init()) {
        warning("init failed");
        return -1;
    }

    test1();

    delete g_db;

    return 0;
}

