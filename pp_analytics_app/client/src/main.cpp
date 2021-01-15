#include <string>
#include <vector>

#include "logfault/logfault.h"
//#define RESTINCURL_USE_SYSLOG 0
#define RESTINCURL_ENABLE_DEFAULT_LOGGER 1
#include "nlohmann/json.hpp"
#include "restincurl/restincurl.h"

#include "EncryptorClient.hpp"

using namespace std;
using namespace restincurl;
using namespace logfault;
using json = nlohmann::json;

int64_t clientId = 0;
int32_t bound = 0;
size_t dataVectorLength = 0;
size_t modulusLength = 0;
vector<string> publicKey;

int getFEPublicParamsFromTrustedServer() {

  restincurl::Client client;

  client.Build()
      ->Get("http://localhost:3000/generateKeys")

      // Tell the server that we accept Json payloads
      .AcceptJson()
      .WithCompletion([&](const Result &result) {
        if (result.curl_code == 0 && result.http_response_code == 200) {

          try {

            // Parse the response body we got as Json.
            LFLOG_DEBUG << "Returned body was " << result.body << endl;
            const auto j = json::parse(result.body);

            clientId = j["ClientID"].get<int64_t>();
            // auto params = j["Params"];
            dataVectorLength = j["Params"]["L"].get<int>();
            bound = j["Params"]["Bound"].get<int>();
            auto mpk = j["mpk"];
            for (json::iterator it = mpk.begin(); it != mpk.end(); ++it) {
              publicKey.push_back(*it);
              LFLOG_DEBUG << *it << endl;
            }

          } catch (const std::exception &ex) {
            LFLOG_ERROR << "Caught exception: " << ex.what();
            return -1;
          }

        } else {
          LFLOG_ERROR << "GetKey Request failed: " << result.msg << endl
                      << "HTTP code: " << result.http_response_code << endl;
          return -1;
        }
      })
      .Execute();

  client.CloseWhenFinished();
  client.WaitForFinish();

  // Done
  return 0;
}

int postEncryptedDataToAnalyticsServer(const vector<string> &encryptdData) {

  restincurl::Client client;

  json j;
  j["ClientID"] = clientId;
  j["encData"] = encryptdData;

  client.Build()
      ->Post("http://localhost:3000/data")

      // Tell the server that we accept Json payloads
      .AcceptJson()

      // Tell the server that we have a Json body, and
      // hand that body to the request.
      // json::dump() will serialize the data in j as a Json string.
      .WithJson(j.dump())

      // Call this lambda when the request is done
      .WithCompletion([&](const Result &result) {
        // Check if the request was successful
        if (result.isOk()) {
          LFLOG_DEBUG << "Returned body was " << result.body;
          try {

            // Parse the response body we got as Json.
            const auto j = json::parse(result.body);

            // We expect to get an id for the object we just sent
            // LFLOG_DEBUG << "The object was assigned id " <<
            // j["id"].get<int>();

          } catch (const std::exception &ex) {
            LFLOG_ERROR << "Caught exception: " << ex.what();
            return -1;
          }

        } else {
          LFLOG_ERROR << "PostData Request failed: " << result.msg << endl
                      << "HTTP code: " << result.http_response_code << endl;
          return -1;
        }
      })
      .Execute();

  client.CloseWhenFinished();
  client.WaitForFinish();
  // Done
  return 0;
}

int main(int argc, char *argv[]) {

  // Use logfault for logging and log to std::clog on DEBUG level
  LogManager::Instance().AddHandler(
      make_unique<StreamHandler>(clog, logfault::LogLevel::DEBUGGING));

  // get fe scheme public params from server
  if (getFEPublicParamsFromTrustedServer()) {
    LFLOG_ERROR << "retrieving public data \
            from trusted entity failed"
                << endl;
    exit(EXIT_FAILURE);
  }

  // init encryptor client
  modulusLength = 2048;
  EncryptorClient encryptor(clientId, dataVectorLength, modulusLength, bound);
  // set pub key
  encryptor.setPublicKey(publicKey);
  // encrypt data, as of now we use some randome data provided by
  // EncryptorClient class
  vector<string> encryptedData = encryptor.encryptData(nullptr);

  // print encrypted data
  LFLOG_DEBUG << "encrypted data" << endl;

  std::for_each(encryptedData.begin(), encryptedData.end(),
                [](const auto &elem) { LFLOG_DEBUG << elem << " "; });

  // post encrypted data to analytics engine
  if (postEncryptedDataToAnalyticsServer(encryptedData)) {
    LFLOG_ERROR << "posting encrypted data \
              to analytics engined failed"
                << endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
