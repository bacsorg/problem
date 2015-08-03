#include <bacs/problem/system_verifier.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/protobuf/binary.hpp>

namespace bacs {
namespace problem {

bool system_verifier::make_package(
    const boost::filesystem::path &destination,
    const bunsan::pm::entry & /*package*/) const {
  bunsan::filesystem::reset_dir(destination);
  bunsan::pm::index index;
  index.source.import.source.insert(
      std::make_pair(".", "bacs/system/system_verifier"));
  index.source.self.insert(std::make_pair(".", "src"));
  boost::filesystem::create_directory(destination / "src");
  bunsan::filesystem::ofstream fout(destination / "src" /
                                    "system_verifier.cpp");
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
    fout << "#include <bacs/system/system_verifier.hpp>\n"
         << "\n"
         << "#include <bunsan/protobuf/binary.hpp>\n"
         << "\n"
         << "namespace bacs {\n"
         << "namespace problem {\n"
         << "\n"
         << "class proto_system_verifier : public system_verifier {\n"
         << " public:\n"
         << "  proto_system_verifier();\n"
         << "  bool verify(const problem::System &request,\n"
         << "              problem::SystemResult &result) override;\n"
         << "\n"
         << " private:\n"
         << "   const problem::System m_system;\n"
         << "   static const char system_encoded[];\n"
         << "};\n"
         << "\n"
         << "proto_system_verifier::proto_system_verifier()\n"
         << "    : m_system(bunsan::protobuf::binary::parse_make<\n"
         << "          problem::System>(system_encoded)) {}\n"
         << "\n"
         << "bool proto_system_verifier::verify(\n"
         << "    const problem::System &request,\n"
         << "    problem::SystemResult &result) {\n"
         << "  if (request.revision() != m_system.revision()) {\n"
         << "    result.set_status(problem::SystemResult::INVALID_REVISION);\n"
         << "    return false;\n"
         << "  }\n"
         << "  return true;\n"
         << "}\n"
         << "\n"
         << "const char proto_system_verifier::system_encoded[] = {\n";
    const std::string system_encoded =
        bunsan::protobuf::binary::to_string(m_system);
    for (const int c : system_encoded) {
      fout << "  " << c << ",\n";
    }
    fout << "};\n"
         << "\n"
         << "\n"
         << "}  // namespace problem\n"
         << "}  // namespace bacs\n";
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
  fout.close();
  return true;
}

}  // namespace problem
}  // namespace bacs