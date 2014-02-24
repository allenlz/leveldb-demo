/**
 * @author allenlee.lz@gmail.com
 **/

#ifndef __DELETE_POLICY_H__
#define __DELETE_POLICY_H__

namespace leveldb {

class Slice;

// (added in allenlz/leveldb)
// DeletePolicy used to delete keys in customized way together 
// with Comparator.
// i.e. expire keys depending on user-defined key format.
// Just as the comparator, a DeletePolicy implementation
// must be thread-safe since leveldb may invoke its methods concurrently
// from multiple threads.
class DeletePolicy {
 public:
  virtual ~DeletePolicy() {}

  // The name of the delete policy.
  // Not used inside leveldb right now.
  virtual const char* Name() const = 0;

  // Used to determine whether the key should be deleted.
  // If yes, this key will regarded as kTypeDeletion inside leveldb.
  // Therefore, it will be removed during compaction or skipped in get,
  // seek or iterator.
  virtual bool ShouldDelete(const Slice& key) const = 0;
};

}  // namespace leveldb

#endif //__DELETE_POLICY_H__

