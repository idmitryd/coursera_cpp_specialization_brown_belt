#pragma once

#include <map>
#include <unordered_map>
#include <string>

using namespace std;

namespace Ini {

pair<string_view, string_view> Split(string_view line, char by);

bool isSection(string_view line);
  
using Section = unordered_map<string, string>;

class Document {
public:
  Section& AddSection(string name);
  const Section& GetSection(const string& name) const;
  size_t SectionCount() const;

private:
  unordered_map<string, Section> sections;
};

Document Load(istream& input);

} // namespace Ini