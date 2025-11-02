#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>

#include "GNSSForISAS.hh"
#include "DocumentBuilder.hh"


namespace balloon{

GNSSforISAS::GNSSforISAS()
    : tlm_doc_(bsoncxx::builder::basic::document{}.extract()),
      tlm_doc2_(bsoncxx::builder::basic::document{}.extract())
{
    // 他の初期化処理
}

bool GNSSforISAS::setbinary(const std::vector<uint8_t>& v)
{
  int n = v.size();
  if (n<10) {
    std::cerr << "Telemetry is too short!!: length = " << n << std::endl; 
    return false;
  }
  TLMdef_.setTelemetry(v); 

  return true;
}

void GNSSforISAS::interpretISAS()
{ 

//   96byteで1パケ想定
  seqNo_ = TLMdef_.getValue<int32_t>(0);
  TLMdef_.getVector<float>(4,3,ypr_);
  TLMdef_.getVector<float>(16,3,angrate_);
  TLMdef_.getVector<double>(32,3,posLla_);
  TLMdef_.getVector<float>(56,3,GNSSvelocity_);
  insStatus_ = TLMdef_.getValue<uint16_t>(70);
  GNSStemp_ = TLMdef_.getValue<float>(72);
  GNSSpres_ = TLMdef_.getValue<float>(76);
  currutc_.year = TLMdef_.getValue<int8_t>(80);
  currutc_.month = TLMdef_.getValue<uint8_t>(81);
  currutc_.day = TLMdef_.getValue<uint8_t>(82);
  currutc_.hour = TLMdef_.getValue<uint8_t>(83);
  currutc_.minute = TLMdef_.getValue<uint8_t>(84);
  currutc_.second = TLMdef_.getValue<uint8_t>(85);
  currutc_.fracsec = TLMdef_.getValue<uint16_t>(86);
  IMUsensSat_ = TLMdef_.getValue<uint16_t>(88);
  GNSSnumSats_ = TLMdef_.getValue<uint8_t>(90);  

  DocumentBuilder builder("GNSS", "Status");
  time_t t(0); time(&t);
  const int64_t ti = static_cast<int64_t>(t)*64;
  builder.setTI(ti);
  builder.setTimeNow();

    // フォーマットされた文字列を格納するための文字配列を準備
  char ypr_str[64]; // バッファサイズは十分な大きさに
  char ang_str[64];
  char pos_str[64];
  char vel_str[64];
  char temp_str[16];
  char press_str[16];
    
  // snprintfを使って、小数点以下6桁で文字列をフォーマット
  snprintf(ypr_str, sizeof(ypr_str), "%.6f %.6f %.6f", ypr_[0], ypr_[1], ypr_[2]);
  snprintf(ang_str, sizeof(ang_str), "%.6f %.6f %.6f", angrate_[0], angrate_[1], angrate_[2]);
  snprintf(pos_str, sizeof(pos_str), "%.6f %.6f %.6f", posLla_[0], posLla_[1], posLla_[2]);
  snprintf(vel_str, sizeof(vel_str), "%.6f %.6f %.6f", GNSSvelocity_[0], GNSSvelocity_[1], GNSSvelocity_[2]);
  snprintf(temp_str, sizeof(temp_str), "%.6f", GNSStemp_);
  snprintf(press_str, sizeof(press_str), "%.6f", GNSSpres_);

  {
    const std::string section_name = "GNSSData";
    auto section = bsoncxx::builder::stream::document{}
      << "SeqNo" << seqNo_
      << "Ypr" << bsoncxx::builder::stream::open_array
        << ypr_str
      << bsoncxx::builder::stream::close_array
      << "Angrate" << bsoncxx::builder::stream::open_array
        << ang_str
      << bsoncxx::builder::stream::close_array
      << "PosLla" << bsoncxx::builder::stream::open_array
        << pos_str
      << bsoncxx::builder::stream::close_array
      << "Velocity" << bsoncxx::builder::stream::open_array
        << vel_str
      << bsoncxx::builder::stream::close_array
      << "InsStatus" << insStatus_
      << "GNSStemp" << temp_str
      << "GNSSpres" << press_str
      << "IMUsensSat" << IMUsensSat_
      << "GNSSnumSats" << GNSSnumSats_
      << "TUTC"<< bsoncxx::builder::stream::open_array
        << currutc_.year <<"/"<< currutc_.month <<"/"<< currutc_.day<<"-"<<currutc_.hour<<":"<<currutc_.minute<<":"<<currutc_.second<<"."<<currutc_.fracsec
      << bsoncxx::builder::stream::close_array
      << bsoncxx::builder::stream::finalize;

    builder.addSection(section_name, section);
  }
  // {
  //   const std::string section_name = "TUTC"; // <- ここを新しいセクション名にする
  //   auto section = bsoncxx::builder::stream::document{}
  //     << "year" << static_cast<int>(currutc_.year)
  //     << "month" << static_cast<int>(currutc_.month)
  //     << "day" << static_cast<int>(currutc_.day)
  //     << "hour" << static_cast<int>(currutc_.hour)
  //     << "minute" << static_cast<int>(currutc_.minute)
  //     << "second" << static_cast<int>(currutc_.second)
  //     << "fracsec" << static_cast<int>(currutc_.fracsec)
  //   << bsoncxx::builder::stream::finalize;
  //   builder.addSection(section_name, section);
  // }
   tlm_doc_ = builder.generate();

}

void GNSSforISAS::generateHK()
{
  DocumentBuilder builder("HK", "Status");
  time_t t(0); time(&t);
  const int64_t ti = static_cast<int64_t>(t)*64;
  builder.setTI(ti);
  builder.setTimeNow();
  // ダミーデータ
  Temp1_ = 1*GNSStemp_;
  Temp2_ = 2*GNSStemp_;
  Temp3_ = 3*GNSStemp_;
  Current1_ = ypr_[0];
  Current2_ = ypr_[1];
  Current3_ = ypr_[2];

{
  const std::string section_name = "HKData";
  auto section = bsoncxx::builder::stream::document{}
    << "Temp1" << Temp1_
    << "Temp2" << Temp2_
    << "Temp3" << Temp3_
    << "Current1" << Current1_
    << "Current2" << Current2_
    << "Current3" << Current3_
  << bsoncxx::builder::stream::finalize;
  builder.addSection(section_name, section);  
}

   tlm_doc2_ = builder.generate();
}

}