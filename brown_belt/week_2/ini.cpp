#include "ini.h"
#include <iostream>

using namespace std;

namespace Ini {

Section& Document::AddSection(string name)
{
    return sections[name];
}
const Section& Document::GetSection(const string& name) const
{
    return sections.at(name);
}
size_t Document::SectionCount() const
{
    return sections.size();
}

Document Load(istream& input)
{
    Document doc;
    Section* section = nullptr;
    for (string line; getline(input, line); ) {
        if (line.front() == '[') {
            section = &doc.AddSection(line.substr(1, line.size() - 2));
        } else if (!line.empty()) {
            size_t eq_pos = line.find('=');
            section->insert({line.substr(0, eq_pos), line.substr(eq_pos + 1)});
        }
    }

    return doc;
}

}