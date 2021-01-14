#include "EncryptorClient.hpp"

#include "logfault/logfault.h"
//#define RESTINCURL_USE_SYSLOG 0
#define RESTINCURL_ENABLE_DEFAULT_LOGGER 1
#include "restincurl/restincurl.h"
#include "nlohmann/json.hpp"


using namespace std;
using namespace restincurl;
using namespace logfault;
using json = nlohmann::json;

string clientId = "";
int bound = 0;
int dataVectorLength = 0;
int modulusLength = 0;
string publicKey = "";

int getFEPublicParamsFromTrustedServer() {

    restincurl::Client client;

    client.Build()->Get("http://localhost:3000/publicParams")
    
        // Tell the server that we accept Json payloads
        .AcceptJson()
        .WithCompletion([&](const Result& result) {
            if (result.curl_code == 0 && result.http_response_code == 200) {
                
                try {
                    
                    // Parse the response body we got as Json.
                    LFLOG_DEBUG << "Returned body was " << result.body;
                    const auto j = json::parse(result.body);
                    
                    clientId = j["ClientId"].get<string>();
                    bound = j["Bound"].get<int>();
                    dataVectorLength = j["L"].get<int>();
                    modulusLength = j["modulusLength"].get<int>();
                    publicKey = j["mpk"].get<string>();
                    
                } catch (const std::exception& ex) {
                    LFLOG_ERROR << "Caught exception: " << ex.what();
                    return -1;
                }
                
            } else {
                LFLOG_ERROR << "Request failed: " << result.msg << endl
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

int postEncryptedDataToAnalyticsServer(const string& encryptedData) {

    restincurl::Client client;

     // Create some data
    json j;
    j["ClientId"] = clientId;
    j["encData"] = encryptedData;
    
    client.Build()->Post("http://localhost:3000/data")
    
        // Tell the server that we accept Json payloads
        .AcceptJson()
        
        // Tell the server that we have a Json body, and 
        // hand that body to the request.
        // json::dump() will serialize the data in j as a Json string.
        .WithJson(j.dump())
        
        // Call this lambda when the request is done
        .WithCompletion([&](const Result& result) {
            
            // Check if the request was successful
            if (result.isOk()) {
                LFLOG_DEBUG << "Returned body was " << result.body;
                try {
                    
                    // Parse the response body we got as Json.
                    const auto j = json::parse(result.body);
                    
                    // We expect to get an id for the object we just sent
                    LFLOG_DEBUG << "The object was assigned id " << j["id"].get<int>();
                    
                } catch (const std::exception& ex) {
                    LFLOG_ERROR << "Caught exception: " << ex.what();
                    return;
                }
                
            } else {
                LFLOG_ERROR << "Request failed: " << result.msg << endl
                    << "HTTP code: " << result.http_response_code << endl;
            }
        })
        .Execute();

        
    client.CloseWhenFinished();
    client.WaitForFinish();
    // Done
    return 0;
}

int main( int argc, char * argv[]) {

     // Use logfault for logging and log to std::clog on DEBUG level
    LogManager::Instance().AddHandler(
        make_unique<StreamHandler>(clog, logfault::LogLevel::DEBUGGING));

    // get fe scheme public params from server
    if(getFEPublicParamsFromTrustedServer()) {
        LFLOG_ERROR << "retrieving public data \
            from trusted entity failed" << endl;
    }

    // init encryptor client
    //EncryptorClient encryptor(clientId, dataVectorLength, modulusLength, bound);
    // set pub key
    //encryptor.setPublicKey(publicKey);
    // encrypt data
    //encryptor.encryptData();

    // post encrypted data to analytics engine
    if(postEncryptedDataToAnalyticsServer("hello")) {
        LFLOG_ERROR << "posting encrypted data \
            to analytics engined failed" << endl;
    }

    return 0;
}
