#include <fstream>
#include <streambuf>

#include "xfunc.hpp"

namespace xfift {

    void resolve_includes(std::string& expr) {
        std::regex include_re("^#include\\s\"([^\"]+)\"");
        std::smatch match;
        std::stringstream ss;
        bool resolved = false;

        while (std::regex_search(expr, match, include_re)) {
            assert(match.size() == 2);
            expr = match.suffix();
           
            std::ifstream include(match.str(1));
            if (include.fail()) {
                throw "failed to load file `" + match.str(1) + "`";
            }
            ss << include.rdbuf() << "\n";
            resolved = true;
        }

        if (resolved) {
            ss << expr;
            expr = ss.str();
        }
    }

    void parse_expression(std::string& expr, std::vector<std::string>& func_names) {
        std::regex func_name_re("\\s~?([_\\w:#]+)\??\\s*\\([_,\\w\\s]*\\)\\s*(?:[a-z]|\\{)");
       
        for (std::sregex_iterator it = std::sregex_iterator(expr.begin(), expr.end(), func_name_re);
                                  it != std::sregex_iterator();
                                ++it)
        {
            std::smatch match = *it;
            assert(match.size() == 2);
            // TODO: exclude keywords
            func_names.push_back(match.str(1));
        }

        if (func_names.empty()) {
            func_names.push_back("main");

            std::stringstream ss;
            ss << "_ main() {\n" << expr;
            std::size_t ending = expr.find_last_not_of(" \r\n\t");
            if (ending != -1 && expr[ending] != ';') {
                ss << ';';
            }
            ss << "\n}";
            expr = ss.str();
        }
    }
}