#pragma once

#ifndef BOOST

#include <regex>

using std::wregex;
using std::wsregex_token_iterator;
using std::wsmatch;

using std::regex_match;
using std::regex_search;
using std::regex_replace;

using RegexFlag = std::regex_constants::syntax_option_type;
using std::regex_constants::icase;
using std::regex_constants::ECMAScript;
using std::regex_constants::optimize;

#else

#include <boost/regex.hpp>

using boost::wregex;
using boost::wsregex_token_iterator;
using boost::wsmatch;

using boost::regex_match;
using boost::regex_search;
using boost::regex_replace;

using RegexFlag = boost::regex_constants::syntax_option_type;
using boost::regex_constants::icase;
using boost::regex_constants::ECMAScript;
using boost::regex_constants::optimize;

#endif