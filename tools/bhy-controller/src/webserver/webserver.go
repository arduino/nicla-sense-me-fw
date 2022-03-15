package webserver

import (
	"embed"
	"fmt"
	"log"
	"net/http"

	"github.com/pkg/browser"
)

// content holds all the static file necessary
// to serve our local webserver.
// Since we embed them in the final binary we won't need to
// distribute the static file.
//go:embed *.html
//go:embed *.json
var content embed.FS

func errCheck(e error) {
	if e != nil {
		log.Fatal(e)
	}
}

func startServer() {
	fmt.Printf("Starting server at port 8000\n")
	fmt.Printf("Visit webpage at address http://localhost:8000/index.html\n")

	fileServer := http.FileServer(http.FS(content))
	http.Handle("/", fileServer)
	http.ListenAndServe(":8000", nil)
}

func Execute() {
	go startServer()

	e := browser.OpenURL("http://localhost:8000/index.html")

	errCheck(e)

	select {}
}
