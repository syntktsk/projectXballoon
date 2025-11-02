#ifndef GNSSforISAS_H
#define GNSSforISAS_H 1

#include <iostream>
#include <vector>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include "TelemetryDefinition.hh"

namespace balloon {

class GNSSforISAS
{
public:
  GNSSforISAS();
  ~GNSSforISAS() = default;

  bool setbinary(const std::vector<uint8_t>& v);
  void interpretISAS();
  void generateHK();
  bsoncxx::document::value getTLMdoc(){return tlm_doc_;}
  bsoncxx::document::value getTLMdoc2(){return tlm_doc2_;}


private:
  TelemetryDefinition TLMdef_;
  bsoncxx::document::value tlm_doc_;
  bsoncxx::document::value tlm_doc2_;

  // GNSS
  int32_t seqNo_ = 0;
  std::vector<float> ypr_;
  std::vector<float> angrate_;
  std::vector<double> posLla_;
  std::vector<float> GNSSvelocity_;
  uint16_t insStatus_ = 0;
  float GNSStemp_ = 0;
  float GNSSpres_ = 0;
  TUTC currutc_;
  uint16_t IMUsensSat_ = 0;
  uint8_t GNSSnumSats_ = 0;


  //HK
  float Temp1_;
  float Temp2_;
  float Temp3_;
  float Current1_;
  float Current2_;
  float Current3_;
};

}/*namespace balloon*/ 

#endif /* GNSSforISAS_H */