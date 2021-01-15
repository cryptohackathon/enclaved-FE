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
string bound = "";
size_t dataVectorLength = 0;
size_t modulusLength = 0;
vector<string> publicKey;

int getFEPublicParamsFromTrustedServer() {

  restincurl::Client client;

  LFLOG_INFO << "GET public key params from trusted-key-server";

  client.Build()
      ->Get("http://localhost:10000/generateKeys")

      // Tell the server that we accept Json payloads
      .AcceptJson()
      .WithCompletion([&](const Result &result) {
        if (result.curl_code == 0 && result.http_response_code == 200) {

          try {
            LFLOG_INFO << "Got reponse from trusted-key-server";
            // Parse the response body we got as Json.
            LFLOG_DEBUG << "Returned body was " << result.body << endl;
            const auto j = json::parse(result.body);

            clientId = j["ClientID"].get<int64_t>();
            // auto params = j["Params"];
            dataVectorLength = j["Params"]["L"].get<int>();
            bound = j["Params"]["Bound"].get<string>();
            modulusLength = j["Params"]["modulusLength"];
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

  LFLOG_INFO << "POST encrypted data to analytics engine";
  LFLOG_INFO << j.dump();

  client.Build()
      ->Post("http://localhost:10008/data")

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
          LFLOG_INFO << "Posting to analytics server successful";

          LFLOG_DEBUG << "Returned body was " << result.body;
          try {

            // Parse the response body we got as Json.
            // const auto j = json::parse(result.body);

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

  LFLOG_INFO << "FE Demo Client started";

  // get fe scheme public params from server
  if (getFEPublicParamsFromTrustedServer()) {
    LFLOG_ERROR << "retrieving public data \
            from trusted entity failed"
                << endl;
    exit(EXIT_FAILURE);
  }

  // init encryptor client
  EncryptorClient encryptor(clientId, dataVectorLength, modulusLength, bound);
  // set pub key
  encryptor.setPublicKey(publicKey);
  // encrypt data, as of now we use some randome data provided by
  // EncryptorClient class
  LFLOG_INFO << "Encrypt data";

  vector<string> encryptedData = encryptor.encryptData(nullptr);

  // print encrypted data
  LFLOG_TRACE << "Encrypted data" << endl;

  std::for_each(encryptedData.begin(), encryptedData.end(),
                [](const auto &elem) { LFLOG_TRACE << elem << " "; });

  // post encrypted data to analytics engine
  if (postEncryptedDataToAnalyticsServer(encryptedData)) {
    LFLOG_ERROR << "posting encrypted data \
                to analytics engined failed"
                << endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
