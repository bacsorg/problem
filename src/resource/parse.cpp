#include <bacs/problem/resource/parse.hpp>

#include <boost/cast.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>

#include <algorithm>
#include <string>

#include <cmath>

namespace bacs {
namespace problem {
namespace resource {
namespace parse {

namespace qi = boost::spirit::qi;

struct si_multiple : qi::symbols<char, double> {
  si_multiple() {
    add("da", 1e1);
    add("h", 1e2);
    add("k", 1e3);
    add("M", 1e6);
    add("G", 1e9);
    add("T", 1e12);
    add("P", 1e15);
    add("E", 1e18);
    add("Z", 1e21);
    add("Y", 1e24);
  }
};

struct si_submultiple : qi::symbols<char, double> {
  si_submultiple() {
    add("d", 1e-1);
    add("c", 1e-2);
    add("m", 1e-3);
    add("u", 1e-6);
    add("n", 1e-9);
    add("p", 1e-12);
    add("f", 1e-15);
    add("a", 1e-18);
    add("z", 1e-21);
    add("y", 1e-24);
  }
};

struct binary_multiple : qi::symbols<char, double> {
  binary_multiple() {
    constexpr double ten = 1 << 10;
    double mult = 1;
    add("Ki", mult *= ten);
    add("Mi", mult *= ten);
    add("Gi", mult *= ten);
    add("Ti", mult *= ten);
    add("Pi", mult *= ten);
    add("Ei", mult *= ten);
    add("Zi", mult *= ten);
    add("Yi", mult *= ten);
  }
};

template <typename Iterator>
struct time_parser : qi::grammar<Iterator, double()> {
  time_parser() : time_parser::base_type(start) {
    multiple = (si_multiple_[qi::_val = qi::_1] |
                si_submultiple_[qi::_val = qi::_1] | qi::eps[qi::_val = 1]) >>
               qi::lit('s');
    multiple_unit = multiple[qi::_val = qi::_1] | qi::eps[qi::_val = 1];
    start = (qi::double_ >> multiple_unit)[qi::_val = qi::_1 * qi::_2];
  }

  qi::rule<Iterator, double()> start, multiple, multiple_unit;
  si_multiple si_multiple_;
  si_submultiple si_submultiple_;
};

template <typename Iterator>
struct memory_parser : qi::grammar<Iterator, double()> {
  memory_parser() : memory_parser::base_type(start) {
    multiple = (binary_multiple_[qi::_val = qi::_1] | qi::eps[qi::_val = 1]) >>
               qi::lit('B');
    multiple_unit = multiple[qi::_val = qi::_1] | qi::eps[qi::_val = 1];
    start = (qi::double_ >> multiple_unit)[qi::_val = qi::_1 * qi::_2];
  }

  qi::rule<Iterator, double()> start, multiple, multiple_unit;
  binary_multiple binary_multiple_;
};

std::uint64_t time_millis(const std::string &time) {
  std::string::const_iterator begin = time.begin();
  double result;
  if (!qi::parse(begin, time.end(), time_parser<std::string::const_iterator>(),
                 result) ||
      begin != time.end()) {
    BOOST_THROW_EXCEPTION(time_error()
                          << time_error::value(time)
                          << time_error::pos(begin - time.begin()));
  }
  return boost::numeric_cast<std::uint64_t>(std::round(result * 1000));
}

std::uint64_t memory_bytes(const std::string &memory) {
  std::string::const_iterator begin = memory.begin();
  double result;
  if (!qi::parse(begin, memory.end(),
                 memory_parser<std::string::const_iterator>(), result) ||
      begin != memory.end()) {
    BOOST_THROW_EXCEPTION(memory_error()
                          << memory_error::value(memory)
                          << memory_error::pos(begin - memory.begin()));
  }
  return boost::numeric_cast<std::uint64_t>(std::round(result));
}

}  // namespace parse
}  // namespace resource
}  // namespace problem
}  // namespace bacs
