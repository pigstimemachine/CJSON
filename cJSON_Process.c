#include "../pub/msg_queue.h"
#include "../pub/pub.h"
#include <dirent.h>
#include "math.h"
#include <XG_log.h>

#include <XG_AS4.h>

#include <XG_NetMsg.h>

#include <sys/select.h>

#include <XG_trace.h>
#include <XG_priority.h>

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "cJSON.h"
#include "cJSON_Process.h"

#include <stdio.h>

/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	get_process_para_online 从服务器获取工艺规程参数
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2021/06/ Create
  * @Version 	1.0 2021/06/23
  *
*/
int get_process_para_online ( struct _AS4_comm_result *as4_comm_result )
{
    cJSON* json = cJSON_CreateObject();

    cJSON_AddStringToObject ( json,"projectId", as4_comm_result->PROJECT_ID ); //项目编码
    cJSON_AddStringToObject ( json,"StandardName", as4_comm_result->PROCESS_Code ); //工艺规程文件名称
    XGStandardGet ( json );

}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	JSON
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2021/06/ Create
  * @Version 	1.0 2021/06/15
  *		     	1.1 2021/06/15 change sth
  * @NoteDY16120107B
  	当有新工艺规程出现时
  	先判断程序原有的工艺规程是否符合，不符合则
  	遍历ProcessJSON文件夹下所有工艺规程的json文件，
  	然后遍历每一个文件查看里面的内容是否有包含该工艺规程格式
  	 	通过查阅不同的工艺规程，对其中的参数进行解析和使用
		find /log/ProcessJSON/ .|xargs grep -ri "WDB" 查找某文件夹下的字符串

  	 	as4_comm_result->as_final_data.process 工艺规程

1.2 2021/06/23 设置从服务器获取工艺规程的函数
void XGStandardGet(cJSON* StandardJson);
/*获取工艺规程文件请求*/
/*
	如果服务器找到文件:
	通过 set_param_down_callback()设置的回调函数返回调用
*
Json内容:
{
	cJSON* json = cJSON_CreateObject();

	cJSON_AddStringToObject( json ,"projectId", projectId );//项目编码
	cJSON_AddStringToObject( json ,"StandardName", Name );//工艺规程文件名称
}

*/



int cjson_read_process ( struct _AS4_comm_result *as4_comm_result )
{
    FILE 		   *fp = NULL;
    cJSON		   *json;
    char 		   *out;
    int len,i;
    char* data;
    char process_menu[100];

    sprintf ( process_menu, "%s%s", Process_menu, as4_comm_result->PROCESS_Code );
    PX ( "process_menu=%s\n",process_menu );
    if ( ( access ( process_menu, F_OK ) ) == -1 )
    {
        PX ( "can not find the local process menu \n" );
        get_process_para_online ( as4_comm_result );
        return -1;
    }

    fp = fopen ( process_menu, "r" ); //as4_comm_result->_file_menu.process_menu
    fseek ( fp, 0,SEEK_END );
    len = ftell ( fp );
    fseek ( fp, 0,SEEK_SET );
    data = ( char* ) malloc ( len+1 );
    fread ( data,1,len,fp ) ;
    fclose ( fp );
    if ( NULL != ( fp ) )
    {
        //PX("data=%s\n",data );

        json=cJSON_Parse ( data ); //获取整个大的句柄
        if ( ( NULL != json ) && ( json->type == cJSON_Object ) )
        {
            out=cJSON_Print ( json ); //这个是可以输出的。为获取的整个json的值
#if 0
            PX ( "(cjson_read_process)json pack into cjson success!!! \n" ); ////一串数字也会被解析为json格式
#endif 
		}
        else
        {
            PX ( "(cjson_read_process)json pack into cjson error \n" );
            return -2;
        }

        cJSON * process = cJSON_GetObjectItem ( json, as4_comm_result->PROCESS_Code )  ;

        if ( NULL != process )
        {
#if 0        
            PX ( "(cjson_read_process)txt file get the process data =%s\n", as4_comm_result->PROCESS_Code );
#endif

            // cJSON *process_data = process->child;
            // if (process_data != NULL )


            /*获取焊层几层几道如0308*/
            cJSON * item_layer_type = cJSON_GetObjectItem ( process, as4_comm_result->as_final_data.layerType )  ;
            if ( NULL != item_layer_type )
            {
                PX ( "(cjson_read_process)layerType =%s\n", as4_comm_result->as_final_data.layerType );
                //PX("(cjson_read_process)layerType=%s\n", item_layer_type->valuestring);
#if 0
                cJSON * ITEM_layer_type= cJSON_GetObjectItem ( item_layer_type, json_layer_type )  ;
                int size_layer_type = cJSON_GetArraySize ( ITEM_layer_type )  ;
                PX ( "size_layer_typ=%d\n", size_layer_type );
                for ( i = 0; i < size_layer_type; i++ )
                {
                    cJSON * data_layer_type = cJSON_GetArrayItem ( ITEM_layer_type, i )	;
                    cJSON * item_layer_type = cJSON_GetObjectItem ( data_layer_type, as4_comm_result->Calc_Data.Now_LAYER )	;
                    if ( NULL != item_layer_type )
                    {
                        PX ( "item_layer_type=%s--i=%d\n",item_layer_type->valuestring, i );

                        break;
                    }
                }

#endif
                cJSON * item_cur_layer= cJSON_GetObjectItem ( item_layer_type, as4_comm_result->Calc_Data.Now_LAYER )  ;
                if ( NULL != item_cur_layer )
                {
                    PX ( "(cjson_read_process)Now_LAYER =%s\n", as4_comm_result->Calc_Data.Now_LAYER );
                    cJSON * item_cur_layer_name= cJSON_GetObjectItem ( item_cur_layer, json_layer_name )  ;
                    if ( NULL != item_cur_layer_name )
                    {
                        memcpy ( as4_comm_result->Calc_Data.LAYER, item_cur_layer_name->valuestring, sizeof ( as4_comm_result->Calc_Data.LAYER ) ) ;
                        PX ( "item_cur_layer_name=%s\n", as4_comm_result->Calc_Data.LAYER );
                    }

                    cJSON * item_cur_layer_weld_process= cJSON_GetObjectItem ( item_cur_layer, json_layer_weld_process )  ;
                    if ( NULL != item_cur_layer_weld_process )
                    {
                        memcpy ( as4_comm_result->HJ_PROCESS,item_cur_layer_weld_process->valuestring,sizeof ( as4_comm_result->HJ_PROCESS ) );
                        PX ( "item_cur_layer_weld_process=%s\n", as4_comm_result->HJ_PROCESS );

                    }


                    cJSON * item_cur_layer_vol_para= cJSON_GetObjectItem ( item_cur_layer, json_layer_voltage )	;
                    if ( NULL !=item_cur_layer_vol_para )
                    {
                        cJSON * item_cur_layer_vol_min= cJSON_GetObjectItem ( item_cur_layer_vol_para, json_data_min )	;
                        if ( NULL != item_cur_layer_vol_min )
                        {
                            as4_comm_result->json_process.voltage_min =  item_cur_layer_vol_min->valueint;
                            PX ( "item_cur_layer_vol_min=%d\n", as4_comm_result->json_process.voltage_min );
                        }

                        cJSON * item_cur_layer_vol_max= cJSON_GetObjectItem ( item_cur_layer_vol_para, json_data_max )	;
                        if ( NULL !=item_cur_layer_vol_max )
                        {
                            as4_comm_result->json_process.voltage_max =  item_cur_layer_vol_max->valueint;
                            PX ( "item_cur_layer_vol_max=%d\n", as4_comm_result->json_process.voltage_max );
                        }
                    }

                    cJSON * item_cur_layer_cur_para= cJSON_GetObjectItem ( item_cur_layer,json_layer_current )	;
                    if ( NULL !=item_cur_layer_cur_para )
                    {
                        cJSON * item_cur_layer_cur_min= cJSON_GetObjectItem ( item_cur_layer_cur_para, json_data_min )	;
                        if ( NULL != item_cur_layer_cur_min )
                        {
                            as4_comm_result->json_process.current_min =  item_cur_layer_cur_min->valueint;
                            PX ( "item_cur_layer_cur_min=%d\n", as4_comm_result->json_process.current_min );
                        }

                        cJSON * item_cur_layer_cur_max= cJSON_GetObjectItem ( item_cur_layer_cur_para, json_data_max )	;
                        if ( NULL !=item_cur_layer_cur_max )
                        {
                            as4_comm_result->json_process.current_max =  item_cur_layer_cur_max->valueint;
                            PX ( "item_cur_layer_cur_max=%d\n", as4_comm_result->json_process.current_max );
                        }
                    }

                    cJSON * item_cur_layer_wire_speed= cJSON_GetObjectItem ( item_cur_layer, json_layer_wire_speed )	;
                    if ( NULL !=item_cur_layer_wire_speed )
                    {
                        cJSON * item_wire_speed_min= cJSON_GetObjectItem ( item_cur_layer_wire_speed, json_data_min )	;
                        if ( NULL != item_wire_speed_min )
                        {
                            as4_comm_result->json_process.wire_speed_min =  item_wire_speed_min->valueint;
                            PX ( "item_wire_speed_min=%d\n", as4_comm_result->json_process.wire_speed_min );
                        }

                        cJSON * item_wire_speed_max= cJSON_GetObjectItem ( item_cur_layer_wire_speed, json_data_max )	;
                        if ( NULL !=item_wire_speed_max )
                        {
                            as4_comm_result->json_process.wire_speed_max =  item_wire_speed_max->valueint;
                            PX ( "item_wire_speed_max=%d\n", as4_comm_result->json_process.wire_speed_max );
                        }
                    }

                    cJSON * item_cur_layer_weld_speed= cJSON_GetObjectItem ( item_cur_layer, json_layer_weld_speed )	;
                    if ( NULL !=item_cur_layer_weld_speed )
                    {
                        cJSON * item_weld_speed_min= cJSON_GetObjectItem ( item_cur_layer_weld_speed, json_data_min )	;
                        if ( NULL != item_weld_speed_min )
                        {
                            as4_comm_result->json_process.weld_speed_min =  item_weld_speed_min->valueint;
                            PX ( "item_weld_speed_min=%d\n", as4_comm_result->json_process.weld_speed_min );
                        }

                        cJSON * item_weld_speed_max= cJSON_GetObjectItem ( item_cur_layer_weld_speed, json_data_max )	;
                        if ( NULL !=item_weld_speed_max )
                        {
                            as4_comm_result->json_process.weld_speed_max =  item_weld_speed_max->valueint;
                            PX ( "item_weld_speed_max=%d\n", as4_comm_result->json_process.weld_speed_max );
                        }
                    }

                    cJSON * item_cur_layer_pre_temp= cJSON_GetObjectItem ( item_cur_layer, json_layer_pre_temp )	;
                    if ( NULL !=item_cur_layer_pre_temp )
                    {
                        cJSON * item_pre_temp_min= cJSON_GetObjectItem ( item_cur_layer_pre_temp, json_data_min )	;
                        if ( NULL != item_pre_temp_min )
                        {
                            as4_comm_result->json_process.pre_temp_min =  item_pre_temp_min->valueint;
                            PX ( "item_pre_temp_min=%d\n", as4_comm_result->json_process.pre_temp_min );
                        }

                        cJSON * item_pre_temp_max= cJSON_GetObjectItem ( item_cur_layer_pre_temp, json_data_max )	;
                        if ( NULL !=item_pre_temp_max )
                        {
                            as4_comm_result->json_process.pre_temp_max =  item_pre_temp_max->valueint;
                            PX ( "item_pre_temp_max=%d\n", as4_comm_result->json_process.pre_temp_max );

                        }
                    }


                    cJSON * item_cur_layer_temp= cJSON_GetObjectItem ( item_cur_layer, json_layer_temp ) ;
                    if ( NULL !=item_cur_layer_temp )
                    {
                        cJSON * item_layer_temp_min= cJSON_GetObjectItem ( item_cur_layer_temp, json_data_min )	;
                        if ( NULL != item_layer_temp_min )
                        {
                            as4_comm_result->json_process.layer_temp_min =  item_layer_temp_min->valueint;
                            PX ( "item_layer_temp_min=%d\n", as4_comm_result->json_process.layer_temp_min );
                        }

                        cJSON * item_layer_temp_max= cJSON_GetObjectItem ( item_cur_layer_temp, json_data_max )	;
                        if ( NULL !=item_layer_temp_max )
                        {
                            as4_comm_result->json_process.layer_temp_max =  item_layer_temp_max->valueint;
                            PX ( "item_layer_temp_max=%d\n", as4_comm_result->json_process.layer_temp_max );

                        }
                    }








                }
            }
        }
    }


    cJSON_Delete ( json );
    free ( data );
    return 0;

}

//char str[1024] = "{\"byExposureModeSet\" : \"2\", \"byFocusMode\" : \"2\", \"wMinFocusDistance\" : \"10\"}";

/***********************************************************************
 *函数名称:init_Scanner_data()
 *输入参数:
 *返回参数:
 *函数功能：初始化存储数据
 *函数说明：
 *备	注:20210621 ZCJ 新数据写入接口，json格式写入
 ************************************************************************/

int init_Scanner_data ( struct _AS4_comm_result *as4_comm_result )
{

#if 1
    pthread_mutex_init ( ( &mutex_scannerdata ),NULL );
    pthread_mutex_init ( ( &mutex_write_scannerdata ),NULL );

    pthread_mutex_lock ( &mutex_scannerdata );
#endif

    FILE 		   *fp = NULL;
    cJSON		   *json;
    char 		   *out;
    int len;
    char* data;
    fp = fopen ( as4_comm_result->_file_menu.scanner_data, "r" );
    fseek ( fp, 0,SEEK_END );
    len = ftell ( fp );
    fseek ( fp, 0,SEEK_SET );
    data = ( char* ) malloc ( len+1 );
    fread ( data,1,len,fp ) ;
    fclose ( fp );
    if ( NULL != ( fp ) )
    {
        //while (NULL != (fgets(QRcode, 1024, fp)  )   )
        // fread(QRcode,strlen(QRcode),1,fp) ;
        //read(fp,QRcode,strlen(QRcode)) ;
        PX ( "QRcode=%s\n",data );
#if 1
        json=cJSON_Parse ( data ); //获取整个大的句柄
        if ( ( NULL != json ) && ( json->type == cJSON_Object ) )
        {
            out=cJSON_Print ( json ); //这个是可以输出的。为获取的整个json的值
            PX ( "(init_Scanner_data)json pack into cjson success!!! \n" ); ////一串数字也会被解析为json格式
        }
        else
        {
            PX ( "(init_Scanner_data)json pack into cjson error \n" );
            return 0;
        }

        cJSON * welder_code = cJSON_GetObjectItem ( json, json_welder_code )  ;
        if ( NULL != welder_code )
        {
            memcpy ( as4_comm_result->Welder_code, welder_code->valuestring, sizeof ( as4_comm_result->Welder_code ) );
            PX ( "(init_Scanner_data)weldCode:%s\n", as4_comm_result->Welder_code );
        }

        cJSON * pipe_code = cJSON_GetObjectItem ( json, json_pipe_code )  ;
        if ( NULL != pipe_code )
        {
            memcpy ( as4_comm_result->PipeCode, pipe_code->valuestring, sizeof ( as4_comm_result->PipeCode ) );
            PX ( "(init_Scanner_data)pipe_code:%s\n", as4_comm_result->PipeCode );
        }

        cJSON * ALL_weld_time = cJSON_GetObjectItem ( json, json_ALL_weld_time )  ;
        if ( NULL != ALL_weld_time )
        {
            as4_comm_result->Calc_Data.His_ALL_weld_time = ALL_weld_time->valueint;
            PX ( "(init_Scanner_data)ALL_weld_time:%d\n",as4_comm_result->Calc_Data.His_ALL_weld_time );
        }

        cJSON * Now_LAYER = cJSON_GetObjectItem ( json, json_Now_LAYER )  ;
        if ( NULL != Now_LAYER )
        {
            memcpy ( as4_comm_result->Calc_Data.His_LAYER, Now_LAYER->valuestring, sizeof ( as4_comm_result->Calc_Data.His_LAYER ) );
            PX ( "(init_Scanner_data)Now_LAYER:%s\n", as4_comm_result->Calc_Data.His_LAYER );
        }
        as4_comm_result->Calc_Data.ALL_weld_time = as4_comm_result->Calc_Data.His_ALL_weld_time;
        memcpy ( as4_comm_result->Calc_Data.Now_LAYER,as4_comm_result->Calc_Data.His_LAYER,sizeof ( as4_comm_result->Calc_Data.His_LAYER ) );
        if ( NULL == strstr ( as4_comm_result->Calc_Data.His_LAYER,"error" ) )
        {
            as4_comm_result->Calc_Data.ALL_Start_weld = P_On;//开始计算焊层的标志
        }




        cJSON * _0clock_welding = cJSON_GetObjectItem ( json, json_0clock_welding )  ;
        if ( NULL != _0clock_welding )
        {
            as4_comm_result->welding_position._0clock_welding = _0clock_welding->valueint;
            PX ( "(init_Scanner_data)_0clock_welding:%d\n",as4_comm_result->welding_position._0clock_welding );
        }

        cJSON * _3clock_welding = cJSON_GetObjectItem ( json, json_3clock_welding )  ;
        if ( NULL != _3clock_welding )
        {
            as4_comm_result->welding_position._3clock_welding = _3clock_welding->valueint;
            PX ( "(init_Scanner_data)_3clock_welding:%d\n",as4_comm_result->welding_position._3clock_welding );
        }

        cJSON * _6clock_welding = cJSON_GetObjectItem ( json, json_6clock_welding )  ;
        if ( NULL != _6clock_welding )
        {
            as4_comm_result->welding_position._6clock_welding = _6clock_welding->valueint;
            PX ( "(init_Scanner_data)_6clock_welding:%d\n",as4_comm_result->welding_position._6clock_welding );
        }

        cJSON * _all_welding = cJSON_GetObjectItem ( json, json_all_welding )  ;
        if ( NULL != _all_welding )
        {
            as4_comm_result->welding_position._all_welding = _all_welding->valueint;
            PX ( "(init_Scanner_data)_all_welding:%d\n",as4_comm_result->welding_position._all_welding );
        }

        cJSON * _finish_sts = cJSON_GetObjectItem ( json, json_finish_sts )  ;
        if ( NULL != _finish_sts )
        {
            as4_comm_result->welding_position._finish_sts = _finish_sts->valueint;
            PX ( "(init_Scanner_data)_finish_sts:%d\n",as4_comm_result->welding_position._finish_sts );
        }

        cJSON * PROCESS_Code = cJSON_GetObjectItem ( json, json_PROCESS_Code )  ;
        if ( NULL != PROCESS_Code )
        {
            memcpy ( as4_comm_result->PROCESS_Code, PROCESS_Code->valuestring, sizeof ( as4_comm_result->PROCESS_Code ) );
            PX ( "(init_Scanner_data)PROCESS_Code:%s\n", as4_comm_result->PROCESS_Code );
        }

        cJSON * TEAM_CODE = cJSON_GetObjectItem ( json, json_TEAM_CODE )	;
        if ( NULL != TEAM_CODE )
        {
            memcpy ( as4_comm_result->parsePipeCode_data.TEAM_CODE, TEAM_CODE->valuestring, sizeof ( as4_comm_result->parsePipeCode_data.TEAM_CODE ) );
            PX ( "(init_Scanner_data)TEAM_CODE:%s\n", as4_comm_result->parsePipeCode_data.TEAM_CODE );
        }

        cJSON * PROJECT_ID = cJSON_GetObjectItem ( json, json_PROJECT_ID )	;
        if ( NULL != PROJECT_ID )
        {
            memcpy ( as4_comm_result->PROJECT_ID, PROJECT_ID->valuestring, sizeof ( as4_comm_result->PROJECT_ID ) );
            PX ( "(init_Scanner_data)PROJECT_ID:%s\n", as4_comm_result->PROJECT_ID );
        }

        cJSON * In_EquCode = cJSON_GetObjectItem ( json, json_In_EquCode )	;
        if ( NULL != In_EquCode )
        {
            memcpy ( as4_comm_result->In_EquCode, In_EquCode->valuestring, sizeof ( as4_comm_result->In_EquCode ) );
            PX ( "(init_Scanner_data)In_EquCode:%s\n", as4_comm_result->In_EquCode );
        }

        cJSON * WifiName = cJSON_GetObjectItem ( json, json_WifiName )	;
        if ( NULL != WifiName )
        {
            memcpy ( as4_comm_result->WifiConnect.name, WifiName->valuestring, sizeof ( as4_comm_result->WifiConnect.name ) );
            PX ( "(init_Scanner_data)WifiConnect.name:%s\n", as4_comm_result->WifiConnect.name );
        }

        cJSON * WifiPassword = cJSON_GetObjectItem ( json, json_WifiPassword )	;
        if ( NULL != WifiPassword )
        {
            memcpy ( as4_comm_result->WifiConnect.password, WifiPassword->valuestring, sizeof ( as4_comm_result->WifiConnect.password ) );
            PX ( "(init_Scanner_data)WifiConnect.password:%s\n", as4_comm_result->WifiConnect.password );
        }

        cJSON * layer_example = cJSON_GetObjectItem ( json, json_layer_example )  ;
        if ( NULL != layer_example )
        {
            as4_comm_result->welding_position.layer_example = layer_example->valueint;
            PX ( "(init_Scanner_data)layer_example:%d\n", as4_comm_result->welding_position.layer_example );
        }

        cJSON * total_weld_time_sec = cJSON_GetObjectItem ( json, json_total_weld_time_sec )  ;
        if ( NULL != total_weld_time_sec )
        {
            as4_comm_result->as_final_data.total_weld_time_sec = total_weld_time_sec->valueint;
            PX ( "(init_Scanner_data)total_weld_time_sec:%d\n", as4_comm_result->as_final_data.total_weld_time_sec );
        }
        as4_comm_result->as_final_data.total_weld_time =  ( ( float ) as4_comm_result->as_final_data.total_weld_time_sec /3600.0 ) ;


        cJSON * oneday_weld_time_sec = cJSON_GetObjectItem ( json, json_oneday_weld_time_sec )  ;
        if ( NULL != oneday_weld_time_sec )
        {
            as4_comm_result->as_final_data.oneday_weld_time_sec = oneday_weld_time_sec->valueint;
            PX ( "(init_Scanner_data)oneday_weld_time_sec:%d\n", as4_comm_result->as_final_data.oneday_weld_time_sec );
        }
        as4_comm_result->as_final_data.oneday_weld_time =  ( ( float ) as4_comm_result->as_final_data.oneday_weld_time_sec /3600.0 ) ;


        cJSON * total_power_consumption = cJSON_GetObjectItem ( json, json_total_power_consumption )  ;
        if ( NULL != total_power_consumption )
        {
            as4_comm_result->as_final_data.total_power_consumption = total_power_consumption->valuedouble;
            PX ( "(init_Scanner_data)total_power_consumption:%f\n", as4_comm_result->as_final_data.total_power_consumption );
        }


        cJSON * single_power_consumption = cJSON_GetObjectItem ( json, json_single_power_consumption )  ;
        if ( NULL != single_power_consumption )
        {
            as4_comm_result->as_final_data.single_power_consumption = single_power_consumption->valuedouble;
            PX ( "(init_Scanner_data)single_power_consumption:%f\n", as4_comm_result->as_final_data.single_power_consumption );
        }


        cJSON * weld_clear_date = cJSON_GetObjectItem ( json, json_weld_clear_date )	;
        if ( NULL != weld_clear_date )
        {
            memcpy ( as4_comm_result->as_final_data.weld_clear_date, weld_clear_date->valuestring, sizeof ( as4_comm_result->as_final_data.weld_clear_date ) );
            PX ( "(init_Scanner_data)weld_clear_date,:%s\n", as4_comm_result->as_final_data.weld_clear_date );
        }

        cJSON * sitecode = cJSON_GetObjectItem ( json, json_sitecode )	;
        if ( NULL != sitecode )
        {
            memcpy ( as4_comm_result->as_final_data.sitecode, sitecode->valuestring, sizeof ( as4_comm_result->as_final_data.sitecode ) );
            PX ( "(init_Scanner_data)sitecode,:%s\n", as4_comm_result->as_final_data.sitecode );
        }


        cJSON * layerType = cJSON_GetObjectItem ( json, json_layerType )	;
        if ( NULL != layerType )
        {
            memcpy ( as4_comm_result->as_final_data.layerType, layerType->valuestring, sizeof ( as4_comm_result->as_final_data.layerType ) );
            PX ( "(init_Scanner_data)layerType,:%s\n", as4_comm_result->as_final_data.layerType );
        }


#endif
    }
#if 1
    IntiCalc_DataLAYER ( as4_comm_result );
//parse_PipeCode_Buffer(as4_comm_result);//解析焊口编码中的数据
//DiscernPROCESS_Code(as4_comm_result);	//解析工艺规程
//Discern_HJPROCESS(as4_comm_result);		//解析焊接类型
    DiscernHJPROCESS ( as4_comm_result );

//	parse_Weldercode_Buffer(as4_comm_result);//解析焊工二维码数据
    parse_Weldercode_IMEI ( as4_comm_result );


//CalcLAYER(as4_comm_result);//将计算的焊层转换为工艺英文字符
    ParaseLayerMode ( as4_comm_result );
#endif

    cJSON_Delete ( json );
    free ( data );


#if 1
    pthread_mutex_unlock ( &mutex_scannerdata );
#endif
    return 0;
}


/***********************************************************************
 *函数名称:Write_Scanner_data()
 *输入参数:
 *返回参数:
 *函数功能：写二维码数据到本地
 *函数说明：
 *备	注:20210621 ZCJ 新数据写入接口，json格式写入
 ************************************************************************/

int Write_Scanner_data ( struct _AS4_comm_result *as4_comm_result,int cmd )
{
    struct stat st;
    struct timeval ttm;
    struct tm *timenow;
    char data[500] = {0};

#if 1
    pthread_mutex_lock ( &mutex_write_scannerdata );
#endif

    chmod ( as4_comm_result->_file_menu.scanner_data, 0777 ); //修改文件权限

    gettimeofday ( &ttm,NULL );
    timenow = localtime ( &ttm.tv_sec );
    if ( ( timenow->tm_year+1900 ) <2099 )
    {
        timenow->tm_year+=1900;
        timenow->tm_mon+=1;
    }
    //创建空对象
    cJSON* root = cJSON_CreateObject();
    //在root节点下添加
    if ( cmd == WelderCode_CMD )
    {
        memcpy ( & ( as4_comm_result->weldercode_time ),timenow,sizeof ( as4_comm_result->weldercode_time ) );
    }
    else if ( cmd == PipeCode_CMD )
    {
        memcpy ( & ( as4_comm_result->pipecode_time ),timenow,sizeof ( as4_comm_result->pipecode_time ) );
    }
    //cJSON_AddItemToObject(root,"welder_time",cJSON_CreateString(as4_comm_result->weldercode_time));
    cJSON_AddItemToObject ( root,json_welder_code,cJSON_CreateString ( as4_comm_result->Welder_code ) );
    //cJSON_AddItemToObject(root,"pipe_time",cJSON_CreateString(as4_comm_result->pipecode_time));
    cJSON_AddItemToObject ( root,json_pipe_code,cJSON_CreateString ( as4_comm_result->PipeCode ) );

    cJSON_AddItemToObject ( root,json_ALL_weld_time,cJSON_CreateNumber ( as4_comm_result->Calc_Data.ALL_weld_time ) );
    cJSON_AddItemToObject ( root,json_Now_LAYER,cJSON_CreateString ( as4_comm_result->Calc_Data.Now_LAYER ) );
    //同步历史数据
    as4_comm_result->Calc_Data.His_ALL_weld_time = as4_comm_result->Calc_Data.ALL_weld_time;
    memcpy ( as4_comm_result->Calc_Data.His_LAYER,as4_comm_result->Calc_Data.Now_LAYER,sizeof ( as4_comm_result->Calc_Data.Now_LAYER ) );

    cJSON_AddItemToObject ( root,json_0clock_welding,cJSON_CreateNumber ( as4_comm_result->welding_position._0clock_welding ) );
    cJSON_AddItemToObject ( root,json_3clock_welding,cJSON_CreateNumber ( as4_comm_result->welding_position._3clock_welding ) );
    cJSON_AddItemToObject ( root,json_6clock_welding,cJSON_CreateNumber ( as4_comm_result->welding_position._6clock_welding ) );
    cJSON_AddItemToObject ( root,json_all_welding,cJSON_CreateNumber ( as4_comm_result->welding_position._all_welding ) );
    cJSON_AddItemToObject ( root,json_finish_sts,cJSON_CreateNumber ( as4_comm_result->welding_position._finish_sts ) );


    as4_comm_result->welding_position_His._0clock_welding = as4_comm_result->welding_position._0clock_welding;
    as4_comm_result->welding_position_His._3clock_welding = as4_comm_result->welding_position._3clock_welding;
    as4_comm_result->welding_position_His._6clock_welding = as4_comm_result->welding_position._6clock_welding;
    as4_comm_result->welding_position_His._all_welding = as4_comm_result->welding_position._all_welding;
    as4_comm_result->welding_position_His._finish_sts = as4_comm_result->welding_position._finish_sts;


    cJSON_AddItemToObject ( root,json_PROCESS_Code,cJSON_CreateString ( as4_comm_result->PROCESS_Code ) );
    cJSON_AddItemToObject ( root,json_TEAM_CODE,cJSON_CreateString ( as4_comm_result->parsePipeCode_data.TEAM_CODE ) );
    cJSON_AddItemToObject ( root,json_PROJECT_ID,cJSON_CreateString ( as4_comm_result->PROJECT_ID ) );
    cJSON_AddItemToObject ( root,json_In_EquCode,cJSON_CreateString ( as4_comm_result->In_EquCode ) );
    cJSON_AddItemToObject ( root,json_WifiName,cJSON_CreateString ( as4_comm_result->WifiConnect.name ) );
    cJSON_AddItemToObject ( root,json_WifiPassword,cJSON_CreateString ( as4_comm_result->WifiConnect.password ) );
    cJSON_AddItemToObject ( root,json_layer_example,cJSON_CreateNumber ( as4_comm_result->welding_position.layer_example ) );

    cJSON_AddItemToObject ( root,json_total_weld_time_sec,cJSON_CreateNumber ( as4_comm_result->as_final_data.total_weld_time_sec ) );
    cJSON_AddItemToObject ( root,json_oneday_weld_time_sec,cJSON_CreateNumber ( as4_comm_result->as_final_data.oneday_weld_time_sec ) );


    cJSON_AddItemToObject ( root,json_total_power_consumption,cJSON_CreateNumber ( as4_comm_result->as_final_data.total_power_consumption ) );
    cJSON_AddItemToObject ( root,json_single_power_consumption,cJSON_CreateNumber ( as4_comm_result->as_final_data.single_power_consumption ) );

    cJSON_AddItemToObject ( root,json_weld_clear_date,cJSON_CreateString ( as4_comm_result->as_final_data.weld_clear_date ) );

    cJSON_AddItemToObject ( root,json_sitecode,cJSON_CreateString ( as4_comm_result->as_final_data.sitecode ) );
    cJSON_AddItemToObject ( root,json_layerType,cJSON_CreateString ( as4_comm_result->as_final_data.layerType ) );


    ParaseLayerMode ( as4_comm_result );
#if 0
    printf ( "%s\n", cJSON_Print ( root ) );
#endif
    FILE *fp = fopen ( as4_comm_result->_file_menu.scanner_data,"w+" );
    char *buf = cJSON_Print ( root );
    fwrite ( buf,strlen ( buf ),1,fp );
    fclose ( fp );
    cJSON_Delete ( root );

#if 1
    pthread_mutex_unlock ( &mutex_write_scannerdata );
#endif

    return 0;
}

/***********************************************************************
 *函数名称:get file menu()
 *输入参数:
 *返回参数:
 *函数功能：存储数据目录 json格式
 *函数说明：
 *备	注:20210621 ZCJ 新数据写入接口，json格式写入
 ************************************************************************/


int get_file_menu ( struct _AS4_comm_result *as4_comm_result )
{


    FILE 		   *fp = NULL;
    cJSON		   *json;
    char 		   *out;
    int len;
    char* data;
    fp = fopen ( file_menu, "r" );
    fseek ( fp, 0,SEEK_END );
    len = ftell ( fp );
    fseek ( fp, 0,SEEK_SET );
    data = ( char* ) malloc ( len+1 );
    fread ( data,1,len,fp ) ;
    fclose ( fp );
    if ( NULL != ( fp ) )
    {
        PX ( "data=%s\n",data );

        json=cJSON_Parse ( data ); //获取整个大的句柄
        if ( ( NULL != json ) && ( json->type == cJSON_Object ) )
        {
            out=cJSON_Print ( json ); //这个是可以输出的。为获取的整个json的值
            //PX("(init_Scanner_data)json pack into cjson success!!! \n");////一串数字也会被解析为json格式
        }
        else
        {
            //PX("(init_Scanner_data)json pack into cjson error \n");
            return 0;
        }

        cJSON * scanner_data = cJSON_GetObjectItem ( json, json_ScannerMenu )  ;
        if ( NULL != scanner_data )
        {
            memcpy ( as4_comm_result->_file_menu.scanner_data, scanner_data->valuestring, sizeof ( as4_comm_result->_file_menu.scanner_data ) );
            PX ( "scanner_data:%send\n", as4_comm_result->_file_menu.scanner_data );
        }

        cJSON * process_menu = cJSON_GetObjectItem ( json, json_ProcessMenu )	;
        if ( NULL != process_menu )
        {
            memcpy ( as4_comm_result->_file_menu.process_menu, process_menu->valuestring, sizeof ( as4_comm_result->_file_menu.process_menu ) );
            PX ( "process_menu:%send\n", as4_comm_result->_file_menu.process_menu );
        }


    }

    cJSON_Delete ( json );
    return 0;
}

