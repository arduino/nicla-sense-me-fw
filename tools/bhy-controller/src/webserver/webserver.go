package webserver

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"runtime"

	"github.com/pkg/browser"
)

func errCheck(e error) {
	if e != nil {
		log.Fatal(e)
	}
}

func startServer() {
	fmt.Printf("Starting server at port 8000\n")
	fmt.Printf("Visit webpage at address http://localhost:8000/index.html\n")

	_, filename, _, _ := runtime.Caller(0)
	fmt.Println(filename)

	ex, err := os.Executable()
	if err != nil {
		panic(err)
	}
	exPath := filepath.Dir(ex)
	fmt.Println(exPath)

	fileServer := http.FileServer(http.Dir("./webserver/"))
	http.Handle("/", fileServer)
	http.ListenAndServe(":8000", nil)
}

func Execute() {
	go startServer()

	e := browser.OpenURL("http://localhost:8000/sensor.html")

	errCheck(e)

	select {}
}
