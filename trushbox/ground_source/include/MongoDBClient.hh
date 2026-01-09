#ifndef MongoDBClient_HH
#define MongoDBClient_HH 1

#include <memory>
#include <vector>
#include <anlnext/BasicModule.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <bsoncxx/document/value.hpp>
#include "TelemetryDefinition.hh"

namespace balloon {

class MongoDBClient : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(MongoDBClient, 3.1);
public:
  MongoDBClient();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;

  void createCappedCollection(const std::string& name, int size);
  void createCollection(const std::string& name);
  bsoncxx::document::value createDocument(const std::vector<uint8_t>& telemetry_binary);
  void push(const std::string& collection, const bsoncxx::document::value& doc);
  void push_many(const std::string& collection, std::vector<bsoncxx::document::value>& docs);
  bsoncxx::stdx::optional<bsoncxx::document::value> find_one(const std::string& collection, const bsoncxx::document::value& constraints);
  mongocxx::cursor find(const std::string& collection, const bsoncxx::document::value& constraints);
  mongocxx::cursor aggregate(const std::string& collection, const mongocxx::pipeline& pipeline);
  void update(const std::string& collection, const bsoncxx::document::value& constraints, bsoncxx::document::value& doc);
  void update_many(const std::string& collection, const bsoncxx::document::value& constraints, bsoncxx::document::value& doc, bsoncxx::array::value& array_filters);

  mongocxx::database& getDatabase()
  { return m_DB; }
  
private:
  std::unique_ptr<mongocxx::instance> m_MDBInstance;
  std::unique_ptr<mongocxx::client> m_Client;
  mongocxx::database m_DB;
  std::string m_MDBHost;
  int m_MDBPort;
  std::string m_MDBName;
  bool m_MDBInstantiation = true;
};

} /*namespace balloon */

#endif /* MongoDBClient_HH */
