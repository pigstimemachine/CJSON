#ifndef _CJSON_PROCESS_H_
#define _CJSON_PROCESS_H_

#define	json_ScannerMenu		"scanner_data" 

#define json_ProcessMenu 	"process_menu"

#define  json_welder_code 		"welder_code" 
#define  json_pipe_code			"pipe_code"
#define  json_ALL_weld_time		"ALL_weld_time"
#define  json_Now_LAYER			"Now_LAYER"
#define  json_0clock_welding		"_0clock_welding" 
#define  json_3clock_welding		"_3clock_welding"
#define  json_6clock_welding		"_6clock_welding"
#define  json_all_welding			"_all_welding"
#define  json_finish_sts			"_finish_sts"
#define  json_PROCESS_Code			"PROCESS_Code"
#define  json_TEAM_CODE				"TEAM_CODE"
#define  json_PROJECT_ID			"PROJECT_ID"
#define  json_In_EquCode			"In_EquCode" 
#define  json_WifiName				"WifiName"
#define  json_WifiPassword			"WifiPassword"
#define  json_layer_example			"layer_example"
#define  json_total_weld_time_sec	"total_weld_time_sec" 
#define  json_oneday_weld_time_sec	"oneday_weld_time_sec"
#define  json_total_power_consumption	"total_power_consumption"
#define  json_single_power_consumption	"single_power_consumption"
#define  json_weld_clear_date			"weld_clear_date"
#define  json_sitecode					"sitecode"
#define  json_layerType					"layerType"


#define  json_layer_num					"layer_num"
#define  json_layer_type				"layer_type"

#define  json_layer_name		"name"
#define  json_layer_weld_process		"weld_process"
#define  json_layer_voltage		"voltage" 
#define  json_layer_current		"current"
#define  json_data_min		"min"
#define  json_data_max		"max"
#define  json_layer_wire_speed	"wire_speed"
#define  json_layer_weld_speed	"weld_speed"
#define  json_layer_pre_temp	"pre_temp"
#define	 json_layer_temp			"layer_temp"

#define Process_menu	"/log/ProcessJSON/"







int cjson_read_process(struct _AS4_comm_result *as4_comm_result);

int get_file_menu(struct _AS4_comm_result *as4_comm_result);

int get_process_para_online(struct _AS4_comm_result *as4_comm_result);

#endif
