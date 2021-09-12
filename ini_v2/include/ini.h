#include <string>
#include <unordered_map>
#include <utility>

using namespace std;

namespace Ini {

using Section = unordered_map<string, string>;

string GetSectionName(const string& str);

pair<string, string> GetKeyAndValue(string& line, char by);

class Document {
public:
  Section& AddSection(string name);
  const Section& GetSection(const string& name) const;
  size_t SectionCount() const;

private:
  unordered_map<string, Section> sections;
};

Document Load(istream& input);

}
