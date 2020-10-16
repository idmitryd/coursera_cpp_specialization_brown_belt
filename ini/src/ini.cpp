#include <ini.h>
#include <iostream>
#include <string>

using namespace std;

namespace Ini {

bool isSection(string_view line) {
  return (!line.empty() && line[0] == '[' && line[line.size() - 1] == ']');
}

pair<string_view, string_view> Split(string_view line, char by) {
  size_t pos = line.find(by);
  string_view left = line.substr(0, pos);

  if (pos < line.size() && pos + 1 < line.size()) {
    return {left, line.substr(pos + 1)};
  } else {
    return {left, string_view()};
  }
}

Section& Document::AddSection(string name) {
  return sections[name];
}

const Section& Document::GetSection(const string& name) const {
  return sections.at(name);
}

size_t Document::SectionCount() const {
  return sections.size();
}

Document Load(istream& input) {
  Document doc;
  for (string line; getline(input, line); ) {
    if(!isSection(line)) continue;
    Section& sec = doc.AddSection(line.substr(1, line.size() - 2));
    if (input.peek() == '[') continue;
    for (string field; getline(input, field); ) {
      if(field.empty() && input.peek() == '[') break;
      if(field.empty()) continue;
      auto [category, amount] = Split(field, '=');
      sec.insert({string(category), string(amount)});
      if (input.peek() == '[') break;
    }
  }
  return doc;
}
} // namespace Ini