/*
 * Copyright (C) 2022 xxxxxx, Inc. or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * File name: speaker_event_handler.c
 * 
 * Description: Provide the APIs of speaker and execute the event that APIs 
 *              cause.
 * 
 * Processing flow:
 * 
 *  1, spk_event_handler_thread() will dispense the coming event into 
 *     corresponding array and manage it orderly, and send the play
 *     voice cmd to spk_event_executor() and wait the finished event
 *     back from it.
 * 
 *  2, spk_event_executor() will play voive and give the finished event
 *     to spk_event_handler_thread().
 * 
 * Version: V0.1
 * 
 * Modifications: .
 * 
 * Note: 1 tab == 4 spaces!
 * 
 */

#include "sdk_common.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "nrf_delay.h"
#include <stdlib.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
#include "mpu_wrappers.h"

#include "led_event_handler.h"
#include "led_driver.h"
#include "boards.h"
#include "gwp_wdt_monitor.h"
#include "wtn6.h"
#include "speaker_event_handler.h"

#if NRF_MODULE_ENABLED(SPEAKER_EVENT_HANDLER)
#define NRF_LOG_MODULE_NAME speaker_event
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

/* Event handler instantiation       */
static spk_event_handler_t spk_event_handler = {
    .inited =                      false,
    .lock =                         NULL,
    .handler_queue =                NULL,
    .voice_finish_queue =           NULL,
    .executor_queue =               NULL,
    .busy_detection_queue =         NULL,
    .handler_thread =               NULL,
    .executor_thread =              NULL,
    .busy_detection_thread =        NULL,
};

/* Event array instantiation         */
static spk_event_array_t g_spk_event_array = { NULL };

#if 1

/**
 * @brief Function for sorting the action array according to the descending
 *        priority.
 *
 * @param[in] arg : Pointer to the event sent by other function call.
 * 
 * @return  void.
 * 
 * */
void spk_demo(void)
{
    ret_code_t ret = NRF_SUCCESS;
    int i,j,k;
    /* 1&2, Init the array */
    spk_array_init( &g_spk_event_array );

    /* 3, Sort the array                  */
    #if 0
    spk_array_sort( &g_spk_event_array );

    NRF_LOG_INFO( "After sorting: \r\n" );
    spk_array_display_by_log();
    #endif

    /* 4, Remove the member 3             */
    #if 0
    spk_array_remove_one_member( &g_spk_event_array , 3 );

    NRF_LOG_INFO( "After removing 3: \r\n" );
    spk_array_display_by_log();
    #endif


    /* 5, Remove the member 2             */
    #if 0
    spk_array_remove_one_member( &g_spk_event_array , 2 );
    if ( ret != NRF_SUCCESS ){
        NRF_LOG_ERROR("ret = : %d", ret);
    }
    
    NRF_LOG_INFO( "After removing 2: \r\n" );
    spk_array_display_by_log();
    #endif

    /* 6, Remove the member 2    error test     */
    #if 0
    ret = spk_array_remove_one_member( &g_spk_event_array , 2 );
    if ( ret != NRF_SUCCESS ){
        NRF_LOG_ERROR("Func: %s line: %d ret = : %d,",
        __FUNCTION__, __LINE__,ret);
    }
    
    NRF_LOG_INFO( "After removing 2: \r\n" );
    spk_array_display_by_log();
    #endif

    /* 7, Add the member 8       */

    /* 8, Send the first one in array         Thread B */
    {
        //wtn6_play_once(2);
    }

    /* 9, Execute the highest priority event  Thread C */
    {
        //wtn6_play_once(5);
    }
    
    /* 10, Send a event to handler_queue */
    spk_event_t event_tmp6 = {
        .spk_action = 1,
        .spk_voice = 1,
        .spk_priority = 1,
        .spk_sequence = 6
    };
    spk_event_t event_tmp7 = {
        .spk_action = 2,
        .spk_voice = 2,
        .spk_priority = 2,
        .spk_sequence = 4
    };
    spk_event_t event_tmp8 = {
        .spk_action = 2,
        .spk_voice = 3,
        .spk_priority = 1,
        .spk_sequence = 4
    };
    spk_event_t event_tmp9 = {
        .spk_action = 2,
        .spk_voice = 4,
        .spk_priority = 2,
        .spk_sequence = 9
    };
    /*spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9);
    spk_event_handler_event_send(event_tmp9); */
    //nrf_delay_ms(10000);
    //vTaskDelay( pdMS_TO_TICKS(100) );
    //spk_array_display_by_log();

    return;


}
#endif

/**
 * @brief Function for displaying all array info by log printer.
 * 
 * @param[in] event_array: The array linked to the memory allcated by malloc().
 * 
 * @retval NRF_SUCCESS                                   Initiate successfully.
 * @retval NRF_ERROR_NO_MEM                                The malloc() failed.
 * 
 * */
static ret_code_t spk_array_init( spk_event_array_t * event_array )
{
    int i;

    /* 1, Allocate the memory              */
    spk_event_t * temp_pointer  = NULL;
    temp_pointer = \
        ( spk_event_t * )malloc( SPK_EVENT_ARRAY_MAX * sizeof( spk_event_t ) );

    if ( NULL == temp_pointer ) {
        NRF_LOG_ERROR( "%s memory alloc failed \t"
                       " error_code: %d \n"  , 
                       __FUNCTION__, 
                       NRF_ERROR_NO_MEM );
        return NRF_ERROR_NO_MEM;
    }

    memset( temp_pointer, 0, SPK_EVENT_ARRAY_MAX*sizeof( spk_event_t ) );

    /* 2,Link the memory with the array    */
    for( i = 0; i < SPK_EVENT_ARRAY_MAX; i++ ) {
        (*event_array)[i] = (temp_pointer+i);
    }

    #if 0
    NRF_LOG_INFO( "After array init : \r\n" );
    spk_array_display_by_log();
    #endif
    
    return NRF_SUCCESS;
}

/**
 * @brief Function for displaying all array info by log printer.
 * 
 * @param[\] void.
 * 
 * @return  void.
 * 
 * */
static void spk_array_display_by_log( void )
{
    int i;
    uint32_t valid_event_num = 0;
    spk_array_valid_element_number( &g_spk_event_array,
                                    &valid_event_num  );
    for( i = 0; i < valid_event_num; i++ ) {
        NRF_LOG_INFO( 
                      "array [%d]:"
                      "spk_priority: %d.    "
                      "spk_sequence: %d.    "
                      "spk_action: %d.    "
                      "spk_voice:  %d.    \r\n",
                                            i,
                                            g_spk_event_array[i]->spk_priority,
                                            g_spk_event_array[i]->spk_sequence,
                                            g_spk_event_array[i]->spk_action,
                                            g_spk_event_array[i]->spk_voice );
    }
}

/**
 * @brief Function for figuring out how many members in this event array.
 * 
 * Steps:
 *  1, Figure out how many events are in this array, if empty, will return.
 *  2, Sort out the array with descending 
 * 
 * @param[in] event_array : The pointer to the event array needed to 
 *                          figure out.
 * @param[out] array_vaild_number : The member number of this array.
 * 
 * @return  void.
 * 
 * */
static void spk_array_valid_element_number( spk_event_array_t * event_array,
                                            uint32_t *  array_vaild_number )
{
    int i,j,temp;
    uint32_t valid_event_num = 0;
    spk_event_t * event_temp;
    
    for( i = 0; i < SPK_EVENT_ARRAY_MAX; i++ ) {
        if( 0 != (*event_array)[i]->spk_sequence ) {
            valid_event_num ++;
        }
    }

    * array_vaild_number = valid_event_num ;  

    #if 0
    NRF_LOG_INFO( "valid_event_num: %d \r\n", valid_event_num );
    #endif
}

/**
 * @brief Function for sorting the action array according to the priority.
 * 
 * Steps:
 *  1, Figure out how many events are in this array, if empty, will return.
 *  2, Sort out the array with descending 
 * 
 * @param[in] event_array : Pointer to the event array needed to be sorted.
 * 
 * @retval NRF_SUCCESS               If the array is not empty and sort it out.
 * @retval NRF_ERROR_NOT_SUPPORTED                       If the array is empty.
 * @retval NRF_ERROR_NULL                          If the array pinter is NULL.
 * 
 * */
static ret_code_t spk_array_sort( spk_event_array_t * event_array )
{
    int i,j,temp;
    uint32_t valid_event_num = 0;
    spk_event_t * event_temp;
    
    /* 1, Check pointer                                                      */
    if ( NULL == event_array )
        return NRF_ERROR_NULL;
    
    /* 2, Figure out how many events are in this array, if empty,will return */
    spk_array_valid_element_number( event_array, (&valid_event_num) );
    if( 0 == valid_event_num ) 
        return NRF_ERROR_NOT_SUPPORTED;
    
    /* 3, Sort out                                                           */
    /* 3.1, Gather all valid member at front                                 */
    #if 1
    for( i = 0; i < valid_event_num; i++ ) {
        
        #if 0
        NRF_LOG_INFO(" i: %d \r\n", i);
        #endif

        if( 0 == (*event_array)[i]->spk_sequence ) {
            
            /* Find out the first one that is valid                          */
            j = i;
            while((*event_array)[++j]->spk_sequence == 0);
            
            #if 0
            NRF_LOG_INFO(" (++j): %d \r\n", j);
            #endif
            
            /* Exchange the current(unvalid) i with the i+j (valid)member    */
            event_temp = (*event_array)[i];  /* unvalid one to event_temp    */
            (*event_array)[i] = (*event_array)[j]; /* valid to unvalid       */
            (*event_array)[j] = event_temp;
        }
    }
    #endif
    
    /* 3.2, Sort out the array with descending per priority                  */
    #if 1
    for( i = 0; i < valid_event_num; i++ ) {
        
        for( j = 0; j < (valid_event_num - 1 -i); j++ ) {
            
          if( (*event_array)[j]->spk_priority <
              (*event_array)[j+1]->spk_priority ) {

                    event_temp = (*event_array)[j];
                    (*event_array)[j] = (*event_array)[j+1];
                    (*event_array)[j+1] = event_temp;

                }
        }
    }
    #endif
    
    /* 3.3, Sort out the array with descending per sequence                  */
    #if 1
    for( i = 0; i < valid_event_num; i++ ) {
        
        for( j = 0; j < (valid_event_num - 1 -i); j++ ) {
            
          if( ( (*event_array)[j]->spk_priority ==
               (*event_array)[j+1]->spk_priority ) &&
              ( (*event_array)[j]->spk_sequence >
               (*event_array)[j+1]->spk_sequence )) {

                    event_temp = (*event_array)[j];
                    (*event_array)[j] = (*event_array)[j+1];
                    (*event_array)[j+1] = event_temp;
                }
        }
    }
    #endif
    
    #if 0
    NRF_LOG_INFO("After sorting: \r\n");
    spk_array_display_by_log();
    #endif

    return NRF_SUCCESS;
}

/**
 * @brief Function for removing certain event specified by sequence number.
 * 
 * Steps:
 *  1, Figure out how many events are in this array, if empty, will return.
 *  2, Find out the specified element and remove it by setting the sequence  
 *     number as 0.
 *  3, Sort out the array with descending 
 * 
 * @param[in] event_array : The pointer to the event array sent by other 
 *                          function call.
 * @param[in] sequence_specified : The sequence number the function will remove
 *                                 in the array.
 * 
 * @retval NRF_SUCCESS               If the array is not empty and sort it out.
 * @retval NRF_ERROR_NOT_SUPPORTED                       If the array is empty.
 * @retval NRF_ERROR_NOT_FOUND                      If the member is not found.
 * @retval NRF_ERROR_NULL                          If the array pinter is NULL.
 * 
 * */
static ret_code_t spk_array_remove_one_member( spk_event_array_t * event_array,
                                               uint32_t    sequence_specified )
{
    ret_code_t ret = NRF_SUCCESS;
    bool member_found = false; /* if the specified number is found, be true. */

    int i,j,temp;
    uint32_t valid_event_num = 0;
    spk_event_t * event_temp;
    
    /* 1, Check pointer                                                      */
    if ( NULL == event_array ) {

        return NRF_ERROR_NULL;
    }

    /* 2, Figure out how many events are in this array, if empty, will return*/
    spk_array_valid_element_number( event_array, (&valid_event_num) );
    if( 0 == valid_event_num ) {
        return NRF_ERROR_NOT_SUPPORTED;
    }

    /* 3, Find out the specified element and remove it by setting the sequence 
          number as 0.                                                       */
    for( i = 0; i < valid_event_num; i++ ) {

        #if 0
        NRF_LOG_INFO(" Found cycle :i: %d \r\n", i);
        #endif

        if( sequence_specified == (*event_array)[i]->spk_sequence ) {

                    (*event_array)[i]->spk_sequence = 0;
                    (*event_array)[i]->spk_action   = 0;
                    (*event_array)[i]->spk_priority = 0;
                    (*event_array)[i]->spk_voice    = 0;
                    
                    member_found = true;
        }
    }

    if ( false == member_found ) {

        return NRF_ERROR_NOT_FOUND;
    }
    
    return NRF_SUCCESS;
}

/**
 * @brief Function for adding certain event into the array.
 * 
 * Steps:
 *  1, Pointer check null.
 *  2, Find a unvaild member.
 *  3, Fill the event into the pointer.
 *  
 * 
 * @param[in] event_array : The pointer to the event array sent by other 
 *                          function call.
 * @param[in] event_to_add : The sequence number the function will remove
 *                                 in the array.
 * 
 * @retval NRF_SUCCESS              If the array is not empty and sort it out.
 * @retval NRF_ERROR_NO_MEM                              If the array is FULL.
 * @retval NRF_ERROR_NULL                         If the array pinter is NULL.
 * 
 * */
static ret_code_t spk_array_add_one_member( spk_event_array_t *  event_array,
                                            spk_event_t *        event_to_add )
{
    int i,j,temp;
    uint32_t valid_event_num = 0;
    spk_event_t * event_temp;

    /* 1, Pointer check null.                  */
    if ( NULL == event_array ) {

        return NRF_ERROR_NULL;
    }

    /* 2, Find a unvaild member.               */
    for( i = 0; i < 10; i++ ) {

        if( 0 == (*event_array)[i]->spk_sequence ) {

            break; /* Found the unvaild number, so break to put 'i' */
        }  
    }

    #if 0
    NRF_LOG_INFO(" i: %d \r\n", i);
    #endif
    
    /* 2.1 if the array is full                    */
    if( SPK_EVENT_QUEUE_MAX <= i ) {

        NRF_LOG_ERROR(  "Func: %s     "
                        "line: %d     "
                        "error = : %d,",
                                        __FUNCTION__,
                                        __LINE__,
                                        NRF_ERROR_NO_MEM);
        return NRF_ERROR_NO_MEM;
    }

    /* 3, Fill the event into the pointer.     */
    (*event_array)[i]->spk_sequence = event_to_add->spk_sequence;
    (*event_array)[i]->spk_priority = event_to_add->spk_priority;
    (*event_array)[i]->spk_sequence = event_to_add->spk_sequence;
    (*event_array)[i]->spk_voice    =    event_to_add->spk_voice;

    #if 0
    /* 4, Sort out the array.                  */
    spk_array_sort(event_array);
    #endif

    return NRF_SUCCESS;
}

/**
 * @brief Function for sorting the action array and send the action.
 *
 * @param[in] arg : Pointer to the event sent by other function call.
 * 
 * @return  void.
 * 
 * */
static void spk_event_handler_thread(void * arg)
{
    int i;
    BaseType_t xResult;
    ret_code_t ret;

    const spk_event_handler_t *handler = (spk_event_handler_t *)arg;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10);

    static UBaseType_t sequence_number = 0;
    UBaseType_t avaliable_msg;
    spk_event_t event;
    uint32_t last_event_to_executor_seq = 0;

    
    GWP_WDT_MONITOR_TASK_REGISTER( spk_event_handler_thread, 4000 );
    
    for (;;) {
        
        #if 0
            NRF_LOG_INFO( "spk_event_handler_thread working \r\n");
        #endif
        /* 1, Make the other thread has opportunity to run                   */
        vTaskDelay( pdMS_TO_TICKS(100) );
        
        /* 2, Keep the task feeding here to avoid skipping feeding the dog  
              when do `continue` operation                                   */
        GWP_WDT_MONITOR_TASK_FEED( spk_event_handler_thread );
        
        /* 3, Queue receive events from APIs' call and process               */
        xResult = pdFAIL;
        avaliable_msg = 0;

        /* 3.1, Figure out how many messages waitting in the queue           */
        avaliable_msg = uxQueueMessagesWaiting(handler->handler_queue);
        if( avaliable_msg != 0 ) {

            #if 1
            NRF_LOG_INFO( "avaliable_msg from handler_queue: %d  \r\n", 
                           avaliable_msg);
            #endif
        }

        /* 3.2, Get messages && set sequece num && stroe && sort the array   */
        for( i = 0; i < avaliable_msg; i++ ) {
            
            /* 3.2.1, Get the message                                        */
            xResult = xQueueReceive( handler->handler_queue,
                                     &(event), 
                                     xMaxBlockTime );
            if ( pdPASS == xResult ) {

                #if 0
                NRF_LOG_INFO("spk_event_handler_thread: "
                             "handler->handler_queue : " 
                             "spk_action:%d  "
                             "spk_voice: %d  "
                             "spk_priority: %d "
                             "spk_sequence: %d \r\n", 
                                                    event.spk_action, 
                                                    event.spk_voice,
                                                    event.spk_priority,
                                                    event.spk_sequence);
                #endif

                /* 3.2.2, Set the sequence number                            */
                event.spk_sequence = ++sequence_number;
                /* 3.2.3, Store the message into the array                   */
                ret = spk_array_add_one_member( &g_spk_event_array , 
                                                &(event)            );
                if( NRF_SUCCESS != ret) {

                    NRF_LOG_ERROR( "Func: %s     "
                                   "line: %d     "
                                   "error = : %d,",
                                   __FUNCTION__,
                                   __LINE__,
                                   ret);
                    break;
                }
            }
        }
        /* 3.3, Sort the the array                                           */
        if( avaliable_msg != 0 ) {
            
            ret = spk_array_sort( &g_spk_event_array );
            if( NRF_SUCCESS != ret) {
                
                NRF_LOG_ERROR( "Func: %s     "
                                "line: %d     "
                                "error = : %d,",
                                __FUNCTION__,
                                __LINE__,
                                ret);
                continue;
            }

            #if 1
                NRF_LOG_INFO( "After stored the new "
                                               "events into the array: \r\n" );
                spk_array_display_by_log();
            #endif
        }
        
        /* 4, Queue receive events from spk_busy_detection and process       */
        xResult = pdFAIL;
        avaliable_msg = 0;
        
        /* 4.1, Figure out how many messages waitting in the queue           */
        avaliable_msg = uxQueueMessagesWaiting( handler->voice_finish_queue );
        
        if( avaliable_msg != 0 ) {

            #if 1
            NRF_LOG_INFO( "avaliable_msg from voice_finish_queue: %d  \r\n", 
                           avaliable_msg);
            #endif
        }
        
        /* 4.2, Get messages && set sequece num && remove && sort the array  */
        for( i = 0; i < avaliable_msg; i++ ) {
            
            /* 4.2.1, Get the message                                        */
            xResult = xQueueReceive( handler->voice_finish_queue,
                                     &(event), 
                                     xMaxBlockTime );
            if ( pdPASS == xResult ) {

                #if 0
                NRF_LOG_INFO("spk_event_handler_thread: "
                             "voice_finish_queue : " 
                             "spk_action:%d  "
                             "spk_voice: %d  "
                             "spk_priority: %d "
                             "spk_sequence: %d \r\n", 
                                                    event.spk_action, 
                                                    event.spk_voice,
                                                    event.spk_priority,
                                                    event.spk_sequence);
                #endif

                /* 4.2.2, Remove the message from the array                  */
                ret = spk_array_remove_one_member( &g_spk_event_array ,
                                                   event.spk_sequence );
                if( NRF_SUCCESS != ret ) {
                    NRF_LOG_ERROR(  "Func: %s     "
                                    "line: %d     "
                                    "error = : %d,",
                                    __FUNCTION__,
                                    __LINE__,
                                    ret);
                    break;
                }
            }
        }

        /* 4.3, Sort the the array                                           */
        if( avaliable_msg != 0 ) {
            
            #if 0
            NRF_LOG_INFO( "avaliable_msg from "
                          "handler->voice_finish_queue: %d  \r\n", 
                           avaliable_msg);
            #endif
            
            ret = spk_array_sort( &g_spk_event_array );
            if( NRF_SUCCESS != ret) {
                NRF_LOG_ERROR( "Func: %s     "
                                "line: %d     "
                                "error = : %d,",
                                __FUNCTION__,
                                __LINE__,
                                ret);
                continue;
            }

            #if 1
                NRF_LOG_INFO( "After remove the new "
                                               "events into the array: \r\n" );
                spk_array_display_by_log();
            #endif
        }
    
        /* 5, Send the higest priority event to spk_event_executor_thread    */
        /* 5.1, if there is a event needed to send                           */
        if( 0 != g_spk_event_array[0]->spk_sequence ) {
            
            /* 5.2, if the event is the same as the last time                */
            if ( last_event_to_executor_seq != 
                 g_spk_event_array[0]->spk_sequence ) {
                    
                    #if 1
                    NRF_LOG_INFO( "New play event will be pushed\r\n");
                    #endif

                    /* 5.3, register the new event sequece number            */
                    last_event_to_executor_seq = 
                                            g_spk_event_array[0]->spk_sequence;

                    /* 5.4, send the new event sequece number                */
                    spk_event_to_executor_send( *(g_spk_event_array[0]) );
                    
                    #if 1
                    NRF_LOG_INFO( " higest priority event "
                                  " sent to spk_event_executor"
                                  " sequence: %d \r\n", 
                                    g_spk_event_array[0]->spk_sequence);
                    #endif
            }
        }
    }
}

/**
 * @brief Function for calling the driver(play_one_voice/in circle) and start 
 *        the voice busy detect.
 *
 * @param[in] arg : Pointer to the event sent by other function call.
 * 
 * @return  void.
 * 
 * */
static void spk_event_executor_thread(void * arg)
{
    uint32_t i = 0;
    const spk_event_handler_t *handler = (spk_event_handler_t *)arg;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10);
    spk_event_t event;
    BaseType_t xResult;
    bool b_busy_check = false;

    GWP_WDT_MONITOR_TASK_REGISTER( spk_event_executor_thread, 4000 );

    for (;;)
    {       
        #if 0
            NRF_LOG_INFO( "spk_event_executor_thread working \r\n");
        #endif
        
        vTaskDelay( pdMS_TO_TICKS(100) );

        //while(1);

        /* 1, Keep the task feeding here to avoid skipping feeding the dog  
              when do `continue` operation                                   */
        GWP_WDT_MONITOR_TASK_FEED( spk_event_executor_thread );
        
        /* 2, Queue receive from spk_event_handler_thread call               */
        xResult = xQueueReceive( handler->executor_queue, 
                                 &(event), 
                                 xMaxBlockTime );
        if (xResult == pdPASS) {
            #if 0
            NRF_LOG_INFO( "spk_event_executor_thread: "
                          "handler->executor_queue : " 
                          "spk_action:%d  "
                          "spk_voice: %d  "
                          "spk_priority: %d "
                          "spk_sequence: %d \r\n", 
                                                  event.spk_action, 
                                                  event.spk_voice,
                                                  event.spk_priority,
                                                  event.spk_sequence);
            #endif
            
            /* 2.1, Check if b_busy_check is working                         */
            if ( false == b_busy_check ) {

                b_busy_check = true;
            }

            wtn6_speaker_enable();
            wtn6_play_once(event.spk_voice);
            vTaskDelay( pdMS_TO_TICKS(300) );
            //while(!wtn6_busy_check());

            #if 1
            NRF_LOG_INFO( "Play voice \r\n");
            #endif
        }

        /* 3, if b_busy_check                */
        if( (true == b_busy_check) ) {
            
            /* while(1) {
                vTaskDelay( pdMS_TO_TICKS(1));
            } */

            /* i = 0;
            while( (true == wtn6_busy_check()) && (i < 100000) )
            {   
                i++;
                vTaskDelay( pdMS_TO_TICKS(10));
            } */


            /* if(true == wtn6_busy_check()) {
                
                continue;
            } */

            i = 0;
            while( (true == wtn6_busy_check()) && (i < 100000) )
            {   
                i++;
                vTaskDelay( pdMS_TO_TICKS(10));
                GWP_WDT_MONITOR_TASK_FEED( spk_event_executor_thread );
            }

            b_busy_check = false;
            
            #if 1
            NRF_LOG_INFO( "Play voice completely\r\n");
            #endif

            wtn6_speaker_disable();

            spk_event_finished_event_send(event);
            
            #if 1
            NRF_LOG_INFO( "Finish event sent %d \r\n", event.spk_sequence );
            #endif
        }
    }
}

/**
 * @brief Function for calling driver to detect the busy or not.
 *
 * @param[in] arg : Pointer to the event sent by other function call.
 * 
 * @return  void.
 * 
 * */
static void spk_busy_detection_thread(void * arg)
{
    const spk_event_handler_t *handler = (spk_event_handler_t *)arg;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500);
    spk_event_t event;
    BaseType_t xResult;
    
    GWP_WDT_MONITOR_TASK_REGISTER( spk_busy_detection_thread, 4000 );
    
    for (;;)
    {
        //Make the other thread has opportunity to run
        vTaskDelay( pdMS_TO_TICKS(100) );
        
        //Keep the task feeding here to avoid skipping feeding the dog 
        //when do `continue` operation
        GWP_WDT_MONITOR_TASK_FEED( spk_busy_detection_thread );
        
        xResult = xQueueReceive( handler->busy_detection_queue, &(event), 
                                 xMaxBlockTime );
        if (xResult == pdPASS) {
            
            #if 0
            NRF_LOG_INFO("led_event_handler_thread : ledcode:%x action:%d\n", 
                                                               event.ledcode, 
                                                             event.ledaction);
            #endif
            
            /* 1, Sort the array
               
               2, Send the action_busy finished or action_busy failed.

             */
         
        }
    }
}

ret_code_t spk_event_handler_init(void)
{ 
    ret_code_t ret = NRF_SUCCESS;
    
    NRF_LOG_INFO("%s spk_event_hanler_start\n", __FUNCTION__);
    
    if (spk_event_handler.inited != false) {
        NRF_LOG_WARNING("%s \n", __FUNCTION__);
        return NRF_ERROR_INVALID_STATE;
    }

    /* 1,Init mutex                            */
    spk_event_handler.lock = xSemaphoreCreateMutex();
    if (!spk_event_handler.lock) {
        NRF_LOG_ERROR("%s can't create mutex\n", __FUNCTION__);
        return NRF_ERROR_NO_MEM;
    }

    /* 2,Init queue                            */
    spk_event_handler.handler_queue =       xQueueCreate( SPK_EVENT_QUEUE_MAX,
                                                        sizeof( spk_event_t ));
    spk_event_handler.executor_queue =      xQueueCreate( SPK_EVENT_QUEUE_MAX,
                                                        sizeof( spk_event_t ));
    spk_event_handler.busy_detection_queue = xQueueCreate( SPK_EVENT_QUEUE_MAX,
                                                        sizeof( spk_event_t ));
    spk_event_handler.voice_finish_queue  =  xQueueCreate( SPK_EVENT_QUEUE_MAX,
                                                        sizeof( spk_event_t ));
    if ( !spk_event_handler.handler_queue      ||
         !spk_event_handler.executor_queue     ||
         !spk_event_handler.voice_finish_queue ||
         !spk_event_handler.busy_detection_queue ) {
        
        NRF_LOG_ERROR("%s can't create handler_queue\n", __FUNCTION__);

        vSemaphoreDelete(spk_event_handler.lock);
        spk_event_handler.lock = NULL;
        return NRF_ERROR_NO_MEM;
    }

    xSemaphoreTake(spk_event_handler.lock, portMAX_DELAY);
    /* 3,Init array                            */
    if ( NRF_SUCCESS != spk_array_init( &g_spk_event_array )) {
        return NRF_ERROR_NO_MEM;
    }

    /* 4,Init led_handler thread               */
    if (pdPASS != xTaskCreate(  spk_event_handler_thread, 
                                "spk_event_handler", 
                                0x400, 
                                &spk_event_handler, 
                                4, 
                                &spk_event_handler.handler_thread )  ||
        pdPASS != xTaskCreate(  spk_event_executor_thread, 
                                "spk_event_executor", 
                                0x400, 
                                &spk_event_handler, 
                                4, 
                                &spk_event_handler.executor_thread )/* ||
         pdPASS != xTaskCreate(  spk_busy_detection_thread, 
                                "spk_busy_detection", 
                                0x300, 
                                &spk_event_handler, 
                                3, 
                                &spk_event_handler.busy_detection_thread )*/) {

        xSemaphoreGive( spk_event_handler.lock) ;

        vSemaphoreDelete( spk_event_handler.lock );
        spk_event_handler.lock = NULL;
        
        vQueueDelete( spk_event_handler.handler_queue );
        vQueueDelete( spk_event_handler.executor_queue );
        vQueueDelete( spk_event_handler.busy_detection_queue );
        
        spk_event_handler.handler_queue = NULL;
        
        NRF_LOG_INFO("%s Speaker event handler init failed. \n", __FUNCTION__);

        return NRF_ERROR_NO_MEM;
    }
    
    spk_event_handler.inited = true;
    
    xSemaphoreGive(spk_event_handler.lock);
    
    NRF_LOG_INFO("%s Speaker event handler init succeeded. \n", __FUNCTION__);
    
    return ret;
}

/**
 * @brief Used for other application to call, it will send the event to 
 *        handler_queue of spk_event_handler_thread.
 *
 * @param[in] event : Pointer to the event sent by other function call.
 * 
 * @return  void.
 * 
 * */
// TO-DO : change the name per the spk_event_to_executor_send.
ret_code_t spk_event_handler_event_send(spk_event_t event)
{
    ret_code_t ret = NRF_SUCCESS;
  
    if (spk_event_handler.inited == false) {

        NRF_LOG_WARNING("%s not inited\n", __FUNCTION__);

        return NRF_ERROR_INVALID_STATE;
    }
    
    if (spk_event_handler.handler_queue != 0) {

        if (pdPASS != xQueueSend( spk_event_handler.handler_queue, 
                                  (void *)&event, 
                                  (TickType_t)0)) {
            ret = NRF_ERROR_RESOURCES;
        }
    }
    
    return ret;
}

/**
 * @brief Used for spk_event_handler_thread to call, it will send the event to 
 *        executor_queue of spk_event_executor_thread.
 *
 * @param[in] event : Po the event sent by other function call.
 * 
 * @return  void.
 * 
 * */
ret_code_t spk_event_to_executor_send(spk_event_t event)
{
    ret_code_t ret = NRF_SUCCESS;
  
    if (spk_event_handler.inited == false) {

        NRF_LOG_WARNING("%s not inited\n", __FUNCTION__);

        return NRF_ERROR_INVALID_STATE;
    }
    
    if (spk_event_handler.executor_queue != 0) {

        if (pdPASS != xQueueSend( spk_event_handler.executor_queue, 
                                  (void *)&event, 
                                  (TickType_t)0)) {

            ret = NRF_ERROR_RESOURCES;
        }
    }
    
    return ret;
}

/**
 * @brief Used for internal notification. when spk_busy_detection realize that
 *        the voive has been played completely, spk_busy_detection will send 
 *        the event to spk_event_handler_thread to remove the finished event.
 *
 * @param[in] event : Pointer to the event sent by other function call.
 * 
 * @return  void.
 * 
 * */
static ret_code_t spk_event_finished_event_send(spk_event_t event)
{
    ret_code_t ret = NRF_SUCCESS;
  
    if (spk_event_handler.inited == false) {
        NRF_LOG_WARNING("%s not inited\n", __FUNCTION__);
        return NRF_ERROR_INVALID_STATE;
    }
    
    if (spk_event_handler.voice_finish_queue != 0) {
        if (pdPASS != xQueueSend( spk_event_handler.voice_finish_queue, 
                                  (void *)&event, 
                                  (TickType_t)0)) 
        {
            ret = NRF_ERROR_RESOURCES;
        }
    }
    
    return ret;
}

#if 0
ret_code_t led_event_handler_uninit(void)
{ 
    ret_code_t ret = NRF_SUCCESS;
    
    NRF_LOG_INFO("%s start\n", __FUNCTION__);
    
    if (spk_event_handler.inited == false) {
        NRF_LOG_WARNING("%s \n", __FUNCTION__);
        return NRF_ERROR_INVALID_STATE;
    }
    
    if (pdTRUE != xSemaphoreTake( spk_event_handler.lock, 
                                  portMAX_DELAY) ) {
        return NRF_ERROR_BUSY;
    }
    
    if (pdTRUE != xSemaphoreTake( SOS_event.handler_timer_mutex, 
                                  portMAX_DELAY)) 
    {
        return NRF_ERROR_BUSY;
    }
    
    if (pdTRUE != xSemaphoreTake( LORA_event.handler_timer_mutex, 
                                  portMAX_DELAY)) 
    {
        return NRF_ERROR_BUSY;
    }
    
    if (pdTRUE != xSemaphoreTake( BLE_event.handler_timer_mutex, 
                                  portMAX_DELAY)) 
    {
        return NRF_ERROR_BUSY;
    }
    
    if (pdTRUE != xSemaphoreTake( BATT_event.handler_timer_mutex, 
                                  portMAX_DELAY)) 
    {
        return NRF_ERROR_BUSY;
    }
    
    spk_event_handler.inited = false;

    //uninit thread
    vTaskDelete(spk_event_handler.thread);
    spk_event_handler.thread = NULL;

    //uninit queue
    vQueueDelete(spk_event_handler.queue);
    spk_event_handler.queue = NULL;
    
    xSemaphoreGive(spk_event_handler.lock);

    //uninit mutex
    vSemaphoreDelete(spk_event_handler.lock);
    spk_event_handler.lock = NULL;
    
    //TO-DO: //Uninit array
    
    //TO-DO //Uninit    free() from malloc();
    NRF_LOG_INFO("%s end\n", __FUNCTION__);
    
    return ret;
}

ret_code_t led_event_handler_event_send_ISR(led_event_t event)
{
    ret_code_t ret = NRF_SUCCESS;
  
    if (spk_event_handler.inited == false) {
        NRF_LOG_WARNING("%s not inited\n", __FUNCTION__);
        return NRF_ERROR_INVALID_STATE;
    }
    
    if( spk_event_handler.queue != 0 ) {
        BaseType_t woken = pdFALSE;
        if (pdPASS != xQueueSendFromISR( spk_event_handler.queue, 
                                         (void *)&event, 
                                         &woken)) 
        {
            ret = NRF_ERROR_RESOURCES;
        } else {
            portYIELD_FROM_ISR(woken);
        }
    }
    
    return ret;
}

#endif

#endif 
