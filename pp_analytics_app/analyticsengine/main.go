package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"math/big"
	"net/http"

	"github.com/fentec-project/gofe/data"
	"github.com/fentec-project/gofe/innerprod/simple"
)

type ddhParams struct {
	L             int    `json:"L"`
	ModulusLength int    `json:"modulusLength"`
	Bound         string `json:"Bound"`
}

type dataRequest struct {
	ClientID   int      `json:"ClientID"`
	Ciphertext []string `json:"encData"`
}

type fetchKeyRequest struct {
	ClientID int `json:"ClientID"`
}

type fetchKeyResponse struct {
	Y      data.Vector `json:"y"`
	FeKey  *big.Int    `json:"feKey"`
	Params *ddhParams  `json:"Params"`
}

var clientData = []*big.Int{}

func homePage(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Welcome to the HomePage of the AnalyticsEngine!")
	log.Println("Endpoint Hit: homePage")
}

func processData(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Endpoint Hit: Process Data")
	decoder := json.NewDecoder(r.Body)
	var reqData dataRequest
	decoder.Decode(&reqData)
	fmt.Println("Data received from client: ", reqData)

	var keyReq fetchKeyRequest
	keyReq.ClientID = reqData.ClientID
	jsonReq, _ := json.Marshal(keyReq)

	keyRes, err := http.Post("http://localhost:10000/fetchKey", "application/json; charset=utf-8", bytes.NewBuffer(jsonReq))
	if err != nil {
		log.Fatalln(err)
	}

	keyDecoder := json.NewDecoder(keyRes.Body)
	var keys fetchKeyResponse
	keyDecoder.Decode(&keys)

	fmt.Println("Keys received from TrustedEntity: ", keys.Y, keys.FeKey, keys.Params.Bound, keys.Params.L, keys.Params.ModulusLength)

	bound := new(big.Int)
	bound.SetString(keys.Params.Bound, 10)
	dec, _ := simple.NewDDHPrecomp(keys.Params.L, keys.Params.ModulusLength, bound)
	// decrypt to obtain the result: inner prod of x and y
	// we expect xy to be 11 (e.g. <[1,2],[3,4]>)
	var cipher []*big.Int
	for _, elem := range reqData.Ciphertext {
		val := new(big.Int)
		val.SetString(elem, 10) //sets elem as decimal (10) big.Int
		cipher = append(cipher, val)
	}
	fmt.Println("Ciphertext after conversion to data.Vector: ", cipher)
	xy, _ := dec.Decrypt(cipher, keys.FeKey, keys.Y)

	fmt.Println("Decypted data: ", xy)

	clientData = append(clientData, xy)
}

func initAndStartHTTPServer() {
	http.HandleFunc("/", homePage)
	http.HandleFunc("/data", processData)
	log.Fatal(http.ListenAndServe(":10008", nil))
}

func main() {
	initAndStartHTTPServer()
}
