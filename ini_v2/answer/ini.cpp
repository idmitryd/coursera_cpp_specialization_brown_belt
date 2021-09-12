#include <iostream>
#include <ini.h>

using namespace std;


namespace Ini {

string GetSectionName(const string& str) {
  return str.substr(1, str.size() - 2);
}

pair<string, string> GetKeyAndValue(const string& line) {
  size_t pos = line.find('=');
  string left = line.substr(0, pos);

  if (pos < line.size() && pos + 1 < line.size()) {
    return {left, line.substr(pos + 1)};
  } else {
    return {left, ""};
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
    if (line.empty()) {
      continue;
    }
    else if (line.size() > 2 && line[0] == '[' && line[line.size() - 1] == ']') {
      Section* section = &doc.AddSection(GetSectionName(line));
      while (input.peek() != '[' && input.peek() != EOF) {
        string key_val;
        getline(input, key_val);
        if (key_val.empty()) continue;
        auto [key, value] = GetKeyAndValue(key_val);
        section->insert({key, value});
      }
    }
  }
  return doc;
}

}
