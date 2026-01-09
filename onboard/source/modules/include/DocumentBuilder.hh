/**
 * HSQuickLook document builder class
 *
 * @author Hirokazu Odaka
 * @date 2019-10-25
 * @date 2022-10-19 | rename black to section
 *
 */

#ifndef DocumentBuilder_HH
#define DocumentBuilder_HH 1

#include <cstdint>
#include <ctime>
#include <string>
#include <list>
#include <utility>
#include <bsoncxx/document/value.hpp>

namespace balloon {

bsoncxx::document::value make_image_value(uint8_t* buf,
                                          uint32_t size,
                                          int width,
                                          int height,
                                          const std::string& filename);

class DocumentBuilder
{
public:
  DocumentBuilder(const std::string& directory, const std::string& name);

  void setTime(time_t v) { unixtime_ = v; }
  void setTimeNow();
  void setTI(int64_t v) { ti_ = v; }
  void addSection(const std::string& name,
                  const bsoncxx::document::value& contents);
  void addSection(const std::string& name,
                  bsoncxx::document::value&& contents);
  bsoncxx::document::value generate();

private:
  std::string directory_;
  std::string name_;
  time_t unixtime_;
  int64_t ti_;
  std::list<std::pair<std::string, bsoncxx::document::value>> sections_;
};

} /* namespace balloon */

#endif /* DocumentBuilder_HH */
