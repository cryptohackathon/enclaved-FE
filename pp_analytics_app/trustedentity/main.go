package main

import (
	"encoding/json"
	"fmt"
	"log"
	"math/big"
	"math/rand"
	"net/http"
	"strings"

	"github.com/fentec-project/gofe/data"
	"github.com/fentec-project/gofe/innerprod/simple"
)

type ddhParams struct {
	L             int      `json:"L"`
	ModulusLength int      `json:"modulusLength"`
	Bound         []string `json:"Bound"`
}

type keyStruct struct {
	ClientID int         `json:"ClientID"`
	Mpk      []string    `json:"mpk"`
	Y        data.Vector `json:"y"`
	FeKey    *big.Int    `json:"feKey"`
	Params   *ddhParams  `json:"Params"`
}

type keygenResponse struct {
	ClientID int        `json:"ClientID"`
	Mpk      []string   `json:"mpk"`
	Params   *ddhParams `json:"Params"`
}

type fetchKeyRequest struct {
	ClientID int `json:"ClientID"`
}

type fetchKeyResponse struct {
	Y      data.Vector `json:"y"`
	FeKey  *big.Int    `json:"feKey"`
	Params *ddhParams  `json:"Params"`
}

var keyList = map[int]keyStruct{}

func homePage(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Welcome to the HomePage of the TrustedEntity!")
	log.Println("Endpoint Hit: homePage")
}

func generateKeys(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Endpoint Hit: Generate Keys")
	var key keyStruct
	var msk data.Vector
	var response keygenResponse

	key.ClientID = rand.Int()

	len := 2
	bound := big.NewInt(10)
	modulusLength := 2048
	trustedEnt, _ := simple.NewDDHPrecomp(len, modulusLength, bound)

	msk, mpk, _ := trustedEnt.GenerateMasterKeys()
	key.Mpk = strings.Fields(mpk.String())

	key.Y = data.NewVector([]*big.Int{big.NewInt(1), big.NewInt(2)})
	key.FeKey, _ = trustedEnt.DeriveKey(msk, key.Y)
	key.Params = new(ddhParams)
	key.Params.L = trustedEnt.Params.L
	key.Params.Bound = strings.Fields(trustedEnt.Params.Bound.String())
	key.Params.ModulusLength = modulusLength

	keyList[key.ClientID] = key

	response.ClientID = key.ClientID
	response.Mpk = key.Mpk
	response.Params = key.Params

	json.NewEncoder(w).Encode(response)
}

func provideFEKey(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	var req fetchKeyRequest
	decoder.Decode(&req)

	fmt.Println("Endpoint Hit: Returning FE Keys for ClientID ", req.ClientID)
	key := keyList[req.ClientID]

	response := fetchKeyResponse{key.Y, key.FeKey, key.Params}
	json.NewEncoder(w).Encode(response)
}

func initAndStartHTTPServer() {
	http.HandleFunc("/", homePage)
	http.HandleFunc("/generateKeys", generateKeys)
	http.HandleFunc("/fetchKey", provideFEKey)
	log.Fatal(http.ListenAndServe(":10000", nil))
}

func main() {
	initAndStartHTTPServer()
}
