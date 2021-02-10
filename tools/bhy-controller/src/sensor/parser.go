package sensor

import (
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
	"path"
	"path/filepath"
	"strconv"
)

type SensorType struct {
	name     string `json:"name"`
	typeId   int    `json:"type"`
	typeName string `json:"type-name"`
}

type ParseField struct {
	name        int     `json:"name"`
	valueType   string  `json:"type"`
	size        int     `json:"size"`
	scaleFactor float32 `json:"scale-factor"`
}

func loadScheme() map[string]interface{} {
	// Retrieve directory of bhy executable to look for json files
	exe, err := os.Executable()
	errCheck(err)
	exePath := filepath.Dir(exe)

	jsonFile, err := os.Open(path.Join(exePath, "/webserver/parse-scheme.json"))
	errCheck(err)
	defer jsonFile.Close()

	byteValue, err := ioutil.ReadAll(jsonFile)
	errCheck(err)
	var result map[string]interface{}
	json.Unmarshal([]byte(byteValue), &result)
	return result
}

func loadTypes() map[string]interface{} {
	// Retrieve directory of bhy executable to look for json files
	exe, err := os.Executable()
	errCheck(err)
	exePath := filepath.Dir(exe)

	jsonFile, err := os.Open(path.Join(exePath, "/webserver/sensor-type-map.json"))
	errCheck(err)
	defer jsonFile.Close()

	byteValue, err := ioutil.ReadAll(jsonFile)
	errCheck(err)
	var result map[string]interface{}
	json.Unmarshal([]byte(byteValue), &result)
	return result
}

func parseData(data *SensorData) {
	scheme := loadScheme()
	types := loadTypes()

	sensorMap := types[strconv.Itoa(int(data.id))].(map[string]interface{})
	sensorType := int(sensorMap["type"].(float64))
	sensorName := sensorMap["name"].(string)

	typeMap := scheme["types"].([]interface{})
	typeScheme := typeMap[sensorType].(map[string]interface{})
	fields := typeScheme["parse-scheme"].([]interface{})

	fmt.Printf("Sensor id: %d   name: %s   values:", data.id, sensorName)

	index := 0
	for i := 0; i < len(fields); i++ {
		field := fields[i].(map[string]interface{})
		fieldName := field["name"].(string)
		fieldType := field["type"].(string)
		fieldFactor := float32(field["scale-factor"].(float64))
		var value float32
		if fieldType == "int16" {
			value = float32(int16(binary.LittleEndian.Uint16(data.data[index:index+2]))) * fieldFactor
		}

		index += int(field["size"].(float64))
		fmt.Printf(" %s : %f  ", fieldName, value)
	}

	fmt.Println()

}
