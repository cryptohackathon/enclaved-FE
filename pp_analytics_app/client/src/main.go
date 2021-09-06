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

type keygenResponse struct {
	ClientID int       `json:"ClientID"`
	Mpk      []string  `json:"mpk"`
	Params   ddhParams `json:"Params"`
}

type dataRequest struct {
	ClientID   int      `json:"ClientID"`
	Ciphertext []string `json:"encData"`
}

func main() {
	r, err := http.Get("http://localhost:10000/generateKeys")
	if err != nil {
		log.Fatalln(err)
	}

	decoder := json.NewDecoder(r.Body)
	var key keygenResponse
	decoder.Decode(&key)

	fmt.Println("Received keys: ", key)
	bound := new(big.Int)
	bound.SetString(key.Params.Bound, 10)
	enc, _ := simple.NewDDHPrecomp(key.Params.L, key.Params.ModulusLength, bound)
	x := data.NewVector([]*big.Int{big.NewInt(234), big.NewInt(256)})

	var mpk = []*big.Int{}
	for _, elem := range key.Mpk {
		val := new(big.Int)
		val.SetString(elem, 10) //sets elem as decimal (10) big.Int
		mpk = append(mpk, val)
	}
	fmt.Println("mpk: ", mpk)
	cipher, _ := enc.Encrypt(x, mpk)
	fmt.Println("cipher: ", cipher)

	var ciphertext []string
	for _, elem := range cipher {
		ciphertext = append(ciphertext, elem.String())
	}

	fmt.Println("Ciphertext: ", ciphertext)
	dataReq := dataRequest{key.ClientID, ciphertext}

	jsonReq, _ := json.Marshal(dataReq)
	fmt.Println(string(jsonReq))
	dataRes, err := http.Post("http://localhost:10008/data", "application/json; charset=utf-8", bytes.NewBuffer(jsonReq))
	fmt.Println("AnalyticsEngine response: ", dataRes)
}
