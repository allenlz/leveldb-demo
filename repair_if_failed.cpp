#include <stdlib.h>
#include <leveldb/db.h>
#include "util.h"

leveldb::DB *g_db;
std::string g_data_dir = "./data/simple_demo";

int init() {
    leveldb::Options init_options;
    init_options.create_if_missing = true;
    leveldb::Status s = leveldb::DB::Open(init_options, g_data_dir.c_str(), &g_db);
    if (s.ok()) {
        notice("db open succeed:%s\n", g_data_dir.c_str());
        return 0;
    }
    warning("db open failed:%s\n", s.ToString().c_str());

    s = leveldb::RepairDB(g_data_dir.c_str(), init_options);
    if (!s.ok()) {
        warning("db repair failed:%s\n", s.ToString().c_str());
        return -1;
    }

    s = leveldb::DB::Open(init_options, g_data_dir.c_str(), &g_db);
    if (s.ok()) {
        notice("db open succeed after repair:%s\n", g_data_dir.c_str());
        return 0;
    }
    warning("db open failed after repair:%s\n", s.ToString().c_str());

    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        g_data_dir = argv[1];
    }

    if (init()) {
        warning("init failed");
        return -1;
    }

    delete g_db;

    return 0;
}
