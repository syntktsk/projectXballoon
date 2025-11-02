#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <iostream>
#include <chrono>
#include "MongoDBClient.hh"
#include <boost/format.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "TelemetryDefinition.hh"
#include "ReadTelemetry.hh"
using namespace anlnext;


namespace balloon
{

MongoDBClient::MongoDBClient()
  : m_MDBHost("localhost"),
    m_MDBPort(27017),
    m_MDBName("ProjectX"),
    m_MDBInstantiation(true)
{
  // ここは空にしてある
}

ANLStatus MongoDBClient::mod_define()
{
  define_parameter("host", &mod_class::m_MDBHost);
  define_parameter("port", &mod_class::m_MDBPort);
  define_parameter("database", &mod_class::m_MDBName);
  define_parameter("instantiation", &mod_class::m_MDBInstantiation);

  return AS_OK;
}

ANLStatus MongoDBClient::mod_initialize()
{
  if (m_MDBInstantiation) {
    m_MDBInstance = std::make_unique<mongocxx::instance>();
  }
  mongocxx::uri uri((boost::format("mongodb://%s:%d") % m_MDBHost % m_MDBPort).str());
  m_Client.reset(new mongocxx::client(uri));
  m_DB = m_Client->database(m_MDBName);

  return AS_OK;
}

ANLStatus MongoDBClient::mod_finalize()
{
  if (m_MDBInstantiation) {
    m_MDBInstance.reset();
  }

  return AS_OK;
}

void MongoDBClient::createCappedCollection(const std::string& name, const int size)
{
  using bsoncxx::builder::stream::document;
  using bsoncxx::builder::stream::finalize;

  if (!m_DB.has_collection(name)) {
    auto doc = bsoncxx::builder::stream::document{};
    m_DB.create_collection(name,
                           doc <<
                           "capped" << true <<
                           "size" << size << finalize);
  }
}

void MongoDBClient::createCollection(const std::string& name)
{
  using bsoncxx::builder::stream::document;
  using bsoncxx::builder::stream::finalize;

  if (!m_DB.has_collection(name)) {
    auto doc = bsoncxx::builder::stream::document{};
    m_DB.create_collection(name,
                           doc <<
                           "capped" << false << finalize);
  }
}

bsoncxx::document::value MongoDBClient::createDocument(const std::vector<uint8_t>& telemetry_binary) {
  using namespace bsoncxx::builder::basic;
  using namespace std::chrono;

  document doc{};
  
  // タイムスタンプを追加（データが生成された時刻として）
  doc.append(kvp("timestamp", bsoncxx::types::b_date(std::chrono::system_clock::now())));
  
  // バイナリデータをBSONドキュメントに追加。1個目はらずぱい用説あるかなと思って残してある
  // doc.append(kvp("data", bsoncxx::binary(bsoncxx::binary::bytes, telemetry_binary.data(), telemetry_binary.size())));
  doc.append(kvp("data", bsoncxx::types::b_binary{
    bsoncxx::binary_sub_type::k_binary,
    static_cast<uint32_t>(telemetry_binary.size()),
    telemetry_binary.data()
  }));

  return doc.extract();
}

anlnext::ANLStatus MongoDBClient::mod_analyze(){
  // TelementryDefinitionのインスタンスを生成
  TelemetryDefinition telemetry;

  // // ダミーデータの準備と設定
  // telemetry.setTelemetryType(static_cast<uint16_t>(TelemetryType::GNSS));
  // telemetry.setSeqNo(12345);
  // telemetry.setYpr({25.5f, 5.2f, 1.8f});
  // telemetry.setAngrate({0.12f, 0.05f, 0.01f});
  // telemetry.setPosLla({34.852f, 137.915f, 15000.0f});
  // telemetry.setVelocity({2.5f, -1.8f, 0.5f});
  // telemetry.setInsStatus(1);
  // telemetry.setGNSStemp(-35.5f);
  // telemetry.setGNSSpres(150.8f);

  // TUTC utc_time;
  // utc_time.year = 25;       
  // utc_time.month = 8;      
  // utc_time.day = 20;      
  // utc_time.hour = 11;    
  // utc_time.minute = 25; 
  // utc_time.second = 35; 
  // utc_time.fracsec = 12345; 
  // telemetry.setCurrutc(utc_time);

  // telemetry.setIMUsensSat(4);
  // telemetry.setGNSSnumSats(12);

  // // テレメトリをバイナリ化
  // telemetry.generateTelemetry();
  // const std::vector<uint8_t>& binary_data = telemetry.Telemetry();

  // バイナリ取得
  ReadTelemetry readTelemetry;
  std::vector<uint8_t> binary_data = readTelemetry.readBinaryData(); 
  telemetry.setTelemetry(binary_data);

  // テレメトリをバイナリ→数値
  telemetry.interpret();
  const std::vector<uint8_t>& data = telemetry.Telemetry();

  // // バイナリデータをドキュメントに変換
  // bsoncxx::document::value doc = createDocument(data);
  // std::cout << "Telemetry binary size: " << binary_data.size() << " bytes" << std::endl;

  // MongoDBに送信する構造体を作る
  

  // MongoDBに送信
  push("gnss_telemetry", doc);

  std::cout << "テレメトリをMongoDBに送信しました。" << std::endl;
  return AS_OK;
}

void MongoDBClient::push(const std::string& collection, const bsoncxx::document::value& doc)
{
  mongocxx::collection col = m_DB[collection];
  col.insert_one(doc.view());
}

void MongoDBClient::push_many(const std::string& collection, std::vector<bsoncxx::document::value>& docs)
{
  mongocxx::collection col = m_DB[collection];
  if (docs.size() > 0) {
    col.insert_many(docs);
  }
}

bsoncxx::stdx::optional<bsoncxx::document::value> MongoDBClient::find_one(const std::string& collection, const bsoncxx::document::value& constraints)
{
  mongocxx::collection col = m_DB[collection];
  return col.find_one(constraints.view());
}

mongocxx::cursor MongoDBClient::find(const std::string& collection, const bsoncxx::document::value& constraints)
{
  mongocxx::collection col = m_DB[collection];
  return col.find(constraints.view());
}

mongocxx::cursor MongoDBClient::aggregate(const std::string& collection, const mongocxx::pipeline& pipeline)
{
  mongocxx::collection col = m_DB[collection];
  return col.aggregate(pipeline, mongocxx::options::aggregate{});
}

void MongoDBClient::update(const std::string& collection, const bsoncxx::document::value& constraints, bsoncxx::document::value& doc)
{
  mongocxx::collection col = m_DB[collection];
  col.update_one(constraints.view(), doc.view());
}

void MongoDBClient::update_many(const std::string& collection, const bsoncxx::document::value& constraints, bsoncxx::document::value& doc, bsoncxx::array::value& array_filters)
{
  mongocxx::collection col = m_DB[collection];
  mongocxx::options::update options;
  options.array_filters(array_filters.view());
  col.update_many(constraints.view(), doc.view(), options);
}

} /* namespace balloon */