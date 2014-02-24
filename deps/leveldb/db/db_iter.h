// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_DB_ITER_H_
#define STORAGE_LEVELDB_DB_DB_ITER_H_

#include <stdint.h>
#include "leveldb/db.h"
#include "db/dbformat.h"

namespace leveldb {

class DBImpl;
class DeletePolicy;

// Return a new iterator that converts internal keys (yielded by
// "*internal_iter") that were live at the specified "sequence" number
// into appropriate user keys.
extern Iterator* NewDBIterator(
    DBImpl* db,
    const Comparator* user_key_comparator,
    const DeletePolicy* delete_policy,
    Iterator* internal_iter,
    SequenceNumber sequence,
    uint32_t seed);

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_DB_ITER_H_
