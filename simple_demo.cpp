#include <stdlib.h>
#include <leveldb/db.h>
#include "util.h"

leveldb::DB *g_db;
std::string g_data_dir = "./data/simple_demo";

int init() {
    leveldb::Options init_options;
    init_options.create_if_missing = true;
    init_options.write_buffer_size = 1;
    leveldb::Status s = leveldb::DB::Open(init_options, g_data_dir.c_str(), &g_db);
    if (!s.ok()) {
        warning("db open failed:%s\n", s.ToString().c_str());
        return -1;
    }
    return 0;
}

void write_random() {
    std::string key = "box";
    string_random_append(&key, 1, 5);
    leveldb::Slice value = leveldb::Slice(key.data(), key.size());
    leveldb::Status s = g_db->Put(leveldb::WriteOptions(),
        leveldb::Slice(key.data(), key.size()), value);
    if (!s.ok()) {
        warning("put failed: %s\n", s.ToString().c_str());
    } else {
        notice("put ok. key:%s, key_len:%lu, key_len:%lu\n",
            key.c_str(), key.size(), key.size());
    }
}

void multi_write(int n) {
    for (int i = 0; i < n; ++i) {
        write_random();
    }
}

void write(std::string key) {
    leveldb::Slice value = leveldb::Slice(key.data(), key.size());
    leveldb::Status s = g_db->Put(leveldb::WriteOptions(),
        leveldb::Slice(key.data(), key.size()), value);
    if (!s.ok()) {
        warning("put failed: %s\n", s.ToString().c_str());
    } else {
        notice("put ok. key:%s\n", key.c_str());
    }
}

void read(std::string key) {
    std::string value;
    leveldb::Status s = g_db->Get(leveldb::ReadOptions(),
        leveldb::Slice(key.data(), key.size()), &value);
    if (!s.ok()) {
        warning("get failed: %s\n", s.ToString().c_str());
    } else {
        notice("get ok. key:%s, value:%s\n", key.data(), value.data());
    }
}

void test1() {
    read("save");

    printf("test 1\n");
    write("box");
    read("box");
    sleep(1);
    read("box");

    printf("test 2\n");
    write("box");
    read("box");

    printf("test 3\n");
    write("box");
    read("box");

    write("save");
}

void test2() {
    multi_write(1);

    write("box");
}

void test3() {
    read("box");
    write("box");
    read("box");
}

int main() {
    srand(time(NULL));

    if (init()) {
        warning("init failed");
        return -1;
    }

    test3();

    delete g_db;

    return 0;
}

