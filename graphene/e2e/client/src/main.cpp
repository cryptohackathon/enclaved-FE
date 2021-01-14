#include "EncryptorClient.hpp"


//#define RESTINCURL_USE_SYSLOG 0
#define RESTINCURL_ENABLE_DEFAULT_LOGGER 1
#include "restincurl/restincurl.h"


using namespace std;
using namespace restincurl;

int test( int argc, char * argv[]) {

    restincurl::Client client;

    client.Build()->Get("http://localhost:3000/posts/1")
        .AcceptJson()
        .Header("X-Client", "restincurl")
        .Trace()
        .WithCompletion([&](const Result& result) {
            clog << "In callback! HTTP result code was " << result.http_response_code << endl;
            clog << "Data was " << result.body.size() << " bytes." << endl;
        })
        .Execute();

    // If the client goes out of scope, it will terminate all ongoing
    // requests, so we need to wait here for the request to finish.

    // Tell client that we want to close when the request is finish
    client.CloseWhenFinished();

    // Wait for the worker-thread in the client to quit
    client.WaitForFinish();
}

int main( int argc, char * argv[]) {

    EncryptorClient encryptor;

    return 0;
}
