#include <leveldb/db.h>
#include <leveldb/comparator.h>
#include <leveldb/delete_policy.h>

namespace store {

class CustomKey {
public:
    typedef struct { 
        uint32_t reserved;
        uint32_t expired_time_s;
    } Footer;

    CustomKey() {
        _footer.reserved = 0;
        _footer.expired_time_s = 0;
    }

    CustomKey(const leveldb::Slice &user_key, uint32_t expired_time_s)
            : _user_key(user_key), _raw(_user_key.data(), _user_key.size()) {
        _footer.reserved = 0;
        _footer.expired_time_s = expired_time_s;
        _raw.append((char*)&_footer, sizeof(Footer));
    }

    void DecodeFrom(const leveldb::Slice &raw_key) {
        _raw.clear();
        _raw.append(raw_key.data(), raw_key.size());

        _user_key = leveldb::Slice(_raw.data(), _raw.size() - sizeof(CustomKey::Footer));
        _footer = *(CustomKey::Footer*)(_raw.data() + _raw.size() - sizeof(CustomKey::Footer));
    }

    ~CustomKey() {}

    leveldb::Slice to_slice()   { return leveldb::Slice(_raw.data(), _raw.size()); }

    const char *data()          { return _raw.data(); }
    size_t size()               { return _raw.size(); }

    leveldb::Slice user_key()   { return _user_key; }
    uint32_t expired_time_s()   { return _footer.expired_time_s; }

private:
    leveldb::Slice _user_key;
    Footer _footer;
    std::string _raw;
};

static inline leveldb::Slice ExtractUserKey(const leveldb::Slice &key) {
    return leveldb::Slice(key.data(), key.size() - sizeof(CustomKey::Footer));
}

class CustomComparator : public leveldb::Comparator {
public:
    virtual const char *Name() const {
        return "monsterdb.CustomComparator";
    }

    // based from BytewiseComparatorImpl
    virtual int Compare(const leveldb::Slice &a, const leveldb::Slice &b) const {
        leveldb::Slice a_user_key = ExtractUserKey(a);
        leveldb::Slice b_user_key = ExtractUserKey(b);
        return a_user_key.compare(b_user_key);
    }

    // the same with BytewiseComparatorImpl
    virtual void FindShortestSeparator(
            std::string *start,
            const leveldb::Slice &limit) const {
        // Find length of common prefix
        size_t min_length = std::min(start->size(), limit.size());
        size_t diff_index = 0;
        while ((diff_index < min_length) &&
                ((*start)[diff_index] == limit[diff_index])) {
            diff_index++;
        }

        if (diff_index >= min_length) {
            // Do not shorten if one string is a prefix of the other
        } else {
            uint8_t diff_byte = static_cast<uint8_t>((*start)[diff_index]);
            if (diff_byte < static_cast<uint8_t>(0xff) &&
                    diff_byte + 1 < static_cast<uint8_t>(limit[diff_index])) {
                (*start)[diff_index]++;
                start->resize(diff_index + 1);
                assert(Compare(*start, limit) < 0);
            }
        }
    }

    // the same with BytewiseComparatorImpl
    virtual void FindShortSuccessor(std::string *key) const {
        // Find first character that can be incremented
        size_t n = key->size();
        for (size_t i = 0; i < n; i++) {
            const uint8_t byte = (*key)[i];
            if (byte != static_cast<uint8_t>(0xff)) {
                (*key)[i] = byte + 1;
                key->resize(i + 1);
                return;
            }
        }
        // *key is a run of 0xffs.  Leave it alone.
    }
};

class CustomDeletePolicy : public leveldb::DeletePolicy {
public:
    virtual const char *Name() const {
        return "monsterdb.CustomDeletePolicy";
    }

    virtual bool ShouldDelete(const leveldb::Slice &key) const {
        CustomKey custom_key;
        custom_key.DecodeFrom(key);
        uint32_t now_s = time(NULL);
        if (custom_key.expired_time_s() != 0 
            && custom_key.expired_time_s() <= now_s) {
            return true;
        } else {
            return false;
        }
    }
};

}
