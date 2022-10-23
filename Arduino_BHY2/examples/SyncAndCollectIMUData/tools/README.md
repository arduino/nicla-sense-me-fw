# process_nicla_bhy2_log_base64.py
This script is used to convert the log encoded in base64 to ascii
## Usage
```
./process_nicla_bhy2_log_base64.py [log_file_name]
```
## Example
./process_nicla_bhy2_log_base64.py log_nicla_bhy2.txt


# check_for_data_loss.sh
This script is used to check for any potential data loss during the transfer,
and it reports some errors if it does find any data loss

## Usage
```
./check_for_data_loss.sh [OPTION] [log_file_name]
```

## Example
- Example 1
    ```
    ./check_for_data_loss.sh -b ./minicom.log
    ```
    The above command check for the data loss using the log "./minicom.log" which is base on base64 encoding
- Example 2
    ```
    ./check_for_data_loss.sh -a ./minicom.log
    ```
    The above command check for the data loss using the log "./minicom.log" which is base on ascii encoding

