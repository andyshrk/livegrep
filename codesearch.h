#ifndef CODESEARCH_H
#define CODESEARCH_H

#include <vector>
#include <string>

#include <google/dense_hash_set>
#include <re2/re2.h>
#include <locale>

#include "smart_git.h"
#include "mutex.h"

using google::dense_hash_set;

class searcher;
class chunk_allocator;
struct match_result;

using re2::RE2;
using re2::StringPiece;

using std::string;
using std::locale;


/*
 * We special-case data() == NULL to provide an "empty" element for
 * dense_hash_set.
 *
 * StringPiece::operator== will consider a zero-length string equal to a
 * zero-length string with a NULL data().
 */
struct eqstr {
    bool operator()(const StringPiece& lhs, const StringPiece& rhs) const;
};

struct hashstr {
    locale loc;
    size_t operator()(const StringPiece &str) const;
};


typedef dense_hash_set<StringPiece, hashstr, eqstr> string_hash;

class code_searcher {
public:
    code_searcher(git_repository *repo);
    ~code_searcher();
    void walk_ref(const char *ref);
    void dump_stats();
    bool match(RE2& pat);
protected:
    void print_match(const match_result *m);
    void walk_tree(const char *ref, const string& pfx, git_tree *tree);
    void update_stats(const char *ref, const string& path, git_blob *blob);
    void resolve_ref(smart_object<git_commit> &out, const char *refname);

    mutex repo_lock_;
    git_repository *repo_;
    string_hash lines_;
    struct {
        unsigned long bytes, dedup_bytes;
        unsigned long lines, dedup_lines;
    } stats_;
    chunk_allocator *alloc_;

    friend class searcher;
};


#endif /* CODESEARCH_H */