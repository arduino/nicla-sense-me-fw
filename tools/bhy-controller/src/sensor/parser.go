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
	"math"
)

type SensorType struct {
	name        string     `json:"name"`
	scheme      string     `json:"scheme"`
	parseScheme ParseField `json:"parse-scheme"`
	evtCount    int        `json:"eventCount"`
}

type ParseField struct {
	name        int     `json:"name"`
	valueType   string  `json:"type"`
	scaleFactor float32 `json:"scale-factor"`
}

var scheme map[string]interface{}
var types map[string]interface{}

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
	scheme = result
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
	types = result
	return result
}

func parseData(data *SensorData) {
	var eventcount float64 = 0

	sensorMap := types[strconv.Itoa(int(data.id))].(map[string]interface{})
	sensorScheme := sensorMap["scheme"].(string)
	sensorName := sensorMap["name"].(string)

	//Already mapped for singleRead sensors
	typeScheme := sensorMap

	typeMap := scheme["types"].([]interface{})
	//Map the right type:
  if (sensorScheme == "quaternion") {
		typeScheme = typeMap[0].(map[string]interface{})
  } else if (sensorScheme == "xyz") {
		typeScheme = typeMap[1].(map[string]interface{})
  } else if (sensorScheme == "orientation") {
		typeScheme = typeMap[2].(map[string]interface{})
  } else if (sensorScheme == "event") {
    eventcount = sensorMap["eventcount"].(float64)
		typeScheme = typeMap[3].(map[string]interface{})
  } else if (sensorScheme == "activity") {
		typeScheme = typeMap[4].(map[string]interface{})
  } else if (sensorScheme == "BSECOutput") {
		typeScheme = typeMap[5].(map[string]interface{})
  } else if (sensorScheme == "BSECOutputV2") {
		typeScheme = typeMap[6].(map[string]interface{})
  } else if (sensorScheme == "BSECOutputV2Full") {
		typeScheme = typeMap[6].(map[string]interface{})
  }

	fields := typeScheme["parse-scheme"].([]interface{})

	fmt.Printf("Sensor id: %d   name: %s   values:   ", data.id, sensorName)

	index := 0
	for i := 0; i < len(fields); i++ {
		field := fields[i].(map[string]interface{})
		fieldName := field["name"].(string)
		fieldType := field["type"].(string)
		fieldFactor := float32(field["scale-factor"].(float64))
		var fieldSize int = 0
		var value float32 = 0
		var activityVal uint16 = 0;

		if fieldType == "uint8" {
			value = float32(uint8(data.data[index])) * fieldFactor
			fieldSize = 1
		} else if fieldType == "int8" {
      value = float32(int8(data.data[index])) * fieldFactor; 
      fieldSize = 1;
    } else if fieldType == "uint16" {
			if sensorScheme == "activity" {
				activityVal = binary.LittleEndian.Uint16(data.data[index:index+2])
			} else {
				value = float32(uint16(binary.LittleEndian.Uint16(data.data[index:index+2]))) * fieldFactor
			}
			fieldSize = 2
    } else if fieldType == "int16" {
			value = float32(int16(binary.LittleEndian.Uint16(data.data[index:index+2]))) * fieldFactor
			fieldSize = 2
    } else if fieldType == "uint24" {
			temp := []byte{0, 0, 0, 0}
			copy(temp[0:3], data.data[index:index+3])
			value = float32(binary.LittleEndian.Uint32(temp)) * fieldFactor
			fieldSize = 3
		} else if fieldType == "uint32" {
			value = float32(uint32(binary.LittleEndian.Uint32(data.data[index:index+4]))) * fieldFactor
      fieldSize = 4;
    } else if fieldType == "float" {
			value = float32(math.Float32frombits(binary.LittleEndian.Uint32(data.data[index:index+4]))) * fieldFactor
			fieldSize = 4
		} else if fieldType == "none" {
      eventcount = eventcount + 1
      sensorMap["eventcount"] = eventcount
      fieldSize = 0;
    } else {
			panic("Unknown sensor type")
		}

    if sensorScheme == "activity" {
      act := geActivityString(activityVal)
			fmt.Printf(" %s : %s  ", fieldName, act)
    } else if sensorScheme == "event" {
			index += fieldSize
			fmt.Printf(" %s : %d  ", fieldName, int(eventcount))
		} else {
			index += fieldSize
			fmt.Printf(" %s : %f  ", fieldName, value)
		}

	}

	fmt.Println()

}


func geActivityString(activityVal uint16) string {

	activityMessages := [16]string{
                            "Still activity ended",
                            "Walking activity ended",
                            "Running activity ended",
                            "On bicycle activity ended",
                            "In vehicle activity ended",
                            "Tilting activity ended",
                            "In vehicle still ended",
                            "",
                            "Still activity started",
                            "Walking activity started",
                            "Running activity started",
                            "On bicycle activity started",
                            "In vehicle activity started",
                            "Tilting activity started",
                            "In vehicle still started",
                            ""}

	i := 0;
  for i = 0; i < 16; i++ {
    maskedVal := (activityVal & (0x0001 << i)) >> i
    if (maskedVal == 1) {
			break
    }
  }

	return activityMessages[i]
}
