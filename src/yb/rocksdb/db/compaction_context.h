// Copyright (c) YugaByte, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.
//

#pragma once

#include <vector>

#include "yb/rocksdb/metadata.h"
#include "yb/rocksdb/status.h"

namespace rocksdb {

class CompactionFeed {
 public:
  // Process key-value pair during compaction.
  virtual Status Feed(const Slice& key, const Slice& value) = 0;

  // Invoked after last key-value pair in compaction input.
  virtual Status Flush() = 0;

  virtual ~CompactionFeed() = default;
};

// CompactionFeed could be used to alter key-value pairs stream generated by the compaction.
// Such feeds could be combined into chain, providing various modifications on each step.
class CompactionContext {
 public:
  virtual CompactionFeed* Feed() = 0;

  // Gives the compaction feed an opportunity to return a "user frontier" that will be used to
  // update the frontier stored in the version edit metadata when the compaction result is
  // installed.
  //
  // As a concrete use case, we use this to pass the history cutoff timestamp from the DocDB
  // compaction feed into the version edit metadata. See DocDBCompactionFeed.
  virtual UserFrontierPtr GetLargestUserFrontier() const = 0;

  // Returns a list of the ranges which should be considered "live" on this tablet. Returns an empty
  // list if the whole key range of the tablet should be considered live. Returned ranges are
  // represented as pairs of Slices denoting the beginning and end of the range in user space.
  virtual std::vector<std::pair<Slice, Slice>> GetLiveRanges() const = 0;

  virtual ~CompactionContext() = default;
};

struct CompactionContextOptions {
  bool is_full_compaction;
};

}  // namespace rocksdb
