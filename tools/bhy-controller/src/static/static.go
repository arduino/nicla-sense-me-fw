package static

import "embed"

// Content holds all the static file necessary
// to serve our local webserver.
// Since we embed them in the final binary we won't need to
// distribute the static file.
//go:embed *.html
//go:embed *.json
var Content embed.FS
