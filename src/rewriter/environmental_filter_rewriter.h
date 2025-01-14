// Copyright 2010-2021, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// This rewriter is used for environment specific filtering.
// There are roughly three major roles of this rewriter.
//
// 1. Normalization
// There were characters which should be rewriten in some platforms. For
// example, in Windows environment, U+FF0D is preferred than U+2212 for the
// glyph of 'full-width minus', due to historical reason. This rewriter rewrites
// candidate containing U+2212 if the environment is Windows.
//
// 2. Validation
// This rewriter checks validity as UTF-8 string for each candidate. If
// unacceptable candidates were to be found, this rewriter removes such
// candidates.
//
// 3. Unavailable glyph removal
// There are some glyphs that can be in candidates but not always available
// among environments. For example, newer emojis tend to be unavailable in old
// OSes. In order to prevend such glyphs appearing as candidates, this rewriter
// removes candidates containing unavailable glyphs. Information about font
// availability in environments are sent by clients.
#ifndef MOZC_REWRITER_ENVIRONMENTAL_FILTER_REWRITER_H_
#define MOZC_REWRITER_ENVIRONMENTAL_FILTER_REWRITER_H_

#include <memory>
#include <utility>
#include <vector>

#include "base/text_normalizer.h"
#include "data_manager/data_manager_interface.h"
#include "rewriter/rewriter_interface.h"

namespace mozc {

class ConversionRequest;
class Segments;

class CharacterGroupFinder {
 public:
  CharacterGroupFinder() = default;
  ~CharacterGroupFinder() = default;

  // Sets target_codepoints, which represents target group.
  void Initialize(const std::vector<std::vector<char32_t>> &target_codepoints);
  // Finds targeted character in given target codepoints. If found, returns
  // true. If not found, returns false.
  bool FindMatch(const std::vector<char32_t> &target) const;

 private:
  // Closed range of single codepoints, like {{U+1F000, U+1F100}, {U+1F202,
  // U+1F202}}
  std::vector<std::pair<char32_t, char32_t>> single_codepoint_ranges_;
  // Vector of Emoji which requires multiple codepoints, like {{U+1Fxxx, U+200D,
  // U+1Fyyy}, {U+1Fzzz, U+200D, U+1Fwww}}.
  std::vector<std::vector<char32_t>> multiple_codepoints_;
};

class EnvironmentalFilterRewriter : public RewriterInterface {
 public:
  // This class does not take an ownership of |emoji_data_list|, |token_list|
  // and |value_list|.  If NULL pointer is passed to it, Mozc process
  // terminates with an error.
  explicit EnvironmentalFilterRewriter(
      const DataManagerInterface &data_manager);
  ~EnvironmentalFilterRewriter() override = default;

  int capability(const ConversionRequest &request) const override;

  bool Rewrite(const ConversionRequest &request,
               Segments *segments) const override;
  void SetNormalizationFlag(TextNormalizer::Flag flag) { flag_ = flag; }

 private:
  // Controls the normalization behavior.
  TextNormalizer::Flag flag_ = TextNormalizer::kDefault;

  // Filters for filtering target Emoji versions.
  CharacterGroupFinder finder_e12_1_;
  CharacterGroupFinder finder_e13_0_;
  CharacterGroupFinder finder_e13_1_;
  CharacterGroupFinder finder_e14_0_;
  CharacterGroupFinder finder_e15_0_;
};
}  // namespace mozc
#endif  // MOZC_REWRITER_ENVIRONMENTAL_FILTER_REWRITER_H_
