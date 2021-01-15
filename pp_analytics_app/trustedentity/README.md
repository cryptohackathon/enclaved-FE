# Trusted Entity

The trusted entity serves as the Key Distribution Server (KDS) and is responsible for generating and providing keys. It currently works with the simple DDH scene, implements a REST API interface and exposes two endpoints:
- /generateKeys
	> This GET API generates a ClientID for each request, and returns the mpk and simple.DDHParams to the requestor.
- /fetchKey 
	> This POST API expects a ClientID and returns the corresponding y, FeKey and simple.DDHParams.
# Files Structure & Build
## Files
The server is implemented in a single golang file, main.go!

# Building & Running the TrustedEntity 
## Pre-Requisites
- golang development environment
- Postman (optional) for validation

## Build & Execution
Running a server doesn't get simpler than this. Simply do,
```
go run main.go
```
## Validating the server
Hit the /generateKeys API using a HTTP GET request on Postman as shown below:
![Alt text](docs/generateKeys.png?raw=true "Generate Keys")

Hit the /fetchKey API using a HTTP POST request with the ClientID in the body using Postman as shown below:
![Alt text](docs/fetchKey.png?raw=true "Fetch Key")