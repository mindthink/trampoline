/**
 * @file tpl_as_counter.c
 *
 * @internal
 *
 * @section desc File description
 *
 * Trampoline autosar extension software counters functions
 *
 * @section copyright Copyright
 *
 * Trampoline OS
 *
 * Trampoline is copyright (c) IRCCyN 2005-2007
 * Autosar extension is copyright (c) IRCCyN and ESEO 2007
 * Trampoline and its Autosar extension are protected by the
 * French intellectual property law.
 *
 * This software is distributed under the Lesser GNU Public Licence
 *
 * @section infos File informations
 *
 * $Date$
 * $Rev$
 * $Author$
 * $URL$
 */

#include "tpl_as_counter.h"
#include "tpl_os_alarm_kernel.h"
#include "tpl_os_kernel.h"
#include "tpl_as_error.h"

#ifndef NO_COUNTER
extern P2VAR(tpl_counter, OS_APPL_DATA, OS_VAR) tpl_counter_table[COUNTER_COUNT];
#endif

#define OS_START_SEC_CODE
#include "tpl_memmap.h"

/*
 * Increment a counter.
 *
 * counter_id is the identifier of the counter to be incremented.
 * The function returns E_OK if everything was ok or E_OS_ID
 * (extended error only) if counter_id is invalid
 *
 * see paragraph 8.4.12, page 70 of
 * AUTOSAR/Specification of the Operating System v2.1.0
 */
FUNC(StatusType, OS_CODE) IncrementCounter(VAR(CounterType, AUTOMATIC) counter_id)
{
    VAR(StatusType, AUTOMATIC)  result = E_OK;

#ifndef NO_COUNTER
    P2VAR(tpl_counter, OS_APPL_DATA, AUTOMATIC) counter = NULL;
    VAR(tpl_status, AUTOMATIC)  need_rescheduling = NO_SPECIAL_CODE;
#endif

    /*  lock the task structures                    */
    LOCK_WHEN_TASK_OR_ISR()

    /*  store information for error hook routine    */
    STORE_SERVICE(OSServiceId_IncrementCounter)
    STORE_COUNTER_ID(counter_id)

    /*  check a counter_id error                    */
    CHECK_COUNTER_ID_ERROR(counter_id,result)

#ifndef NO_COUNTER
    IF_NO_EXTENDED_ERROR(result)

        /*  get the counter descriptor              */
        counter = tpl_counter_table[counter_id];

        /*  increment the counter                   */
        need_rescheduling |= tpl_counter_tick(counter);

        if (need_rescheduling == NEED_RESCHEDULING) {
            tpl_schedule(FROM_TASK_LEVEL);
        }

    IF_NO_EXTENDED_ERROR_END()
#endif

    PROCESS_ERROR(result)

    /*  unlock the task structures                  */
    UNLOCK_WHEN_TASK_OR_ISR();

    return result;
}


/**
 * Get the current value of a counter.
 *
 * counter_id is the identifier of the counter and value a reference (pointer)
 * to the variable where the value of the counter is stored.
 *
 * see paragraph 8.4.12, page 70 of
 * AUTOSAR/Specification of the Operating System v2.1.0
 */
FUNC(StatusType, OS_CODE) GetCounterValue(
    VAR(CounterType, AUTOMATIC) counter_id,
    VAR(TickRefType, AUTOMATIC) value)
{
    VAR(StatusType, AUTOMATIC)  result = E_OK;

#ifndef NO_COUNTER
    P2VAR(tpl_counter, OS_APPL_DATA, AUTOMATIC) counter = NULL;
#endif

    LOCK_WHEN_HOOK()

    /*  store information for error hook routine    */
    STORE_SERVICE(OSServiceId_GetCounterValue)
    STORE_COUNTER_ID(counter_id)
    STORE_TICK_REF(value)

    /*  check a counter_id error                    */
    CHECK_COUNTER_ID_ERROR(counter_id,result)

#ifndef NO_COUNTER
    IF_NO_EXTENDED_ERROR(result)

        /*  get the counter descriptor              */
        counter = tpl_counter_table[counter_id];

        /*  copy its value in value ref             */
        *value = counter->current_date;

    IF_NO_EXTENDED_ERROR_END()
#endif

    PROCESS_ERROR(result)

    /*  unlock the task structures                  */
    UNLOCK_WHEN_HOOK()

    return result;
}


/**
 * Get the elapsed value of a counter.
 *
 * counter_id is the identifier of the counter and value a reference (pointer)
 * to the variable where the value of the counter is stored.
 *
 * see paragraph 8.4.12, page 70 of
 * AUTOSAR/Specification of the Operating System v2.1.0
 */
FUNC(StatusType, OS_CODE) GetElapsedCounterValue(
    VAR(CounterType, AUTOMATIC) counter_id,
    VAR(TickType, AUTOMATIC)    previous_value,
    VAR(TickRefType, AUTOMATIC) value)
{
    VAR(StatusType, AUTOMATIC)  result = E_OK;

#ifndef NO_COUNTER
    P2VAR(tpl_counter, OS_APPL_DATA, AUTOMATIC) counter = NULL;
    VAR(TickType, AUTOMATIC)                    cpt_val;
#endif

    LOCK_WHEN_HOOK()

    /*  store information for error hook routine    */
    STORE_SERVICE(OSServiceId_GetElapsedCounterValue)
    STORE_COUNTER_ID(counter_id)
    STORE_TICK_2(previous_value)
    STORE_TICK_REF(value)

    /*  check a counter_id error                    */
    CHECK_COUNTER_ID_ERROR(counter_id,result)
    /*  check the previous value does not exceed
        the maxallowedvalue of the counter          */
    CHECK_COUNTER_MAX_ALLOWED_VALUE_ERROR(counter_id,previous_value,result)

#ifndef NO_COUNTER
    IF_NO_EXTENDED_ERROR(result)

        /*  get the counter descriptor              */
        counter = tpl_counter_table[counter_id];

        /*  get the current counter value           */
        cpt_val = counter->current_date;
        if (cpt_val < previous_value) {
            cpt_val += tpl_counter_table[counter_id]->max_allowed_value + 1;
        }
        *value = cpt_val - previous_value;

    IF_NO_EXTENDED_ERROR_END()
#endif

    PROCESS_ERROR(result)

    /*  unlock the task structures                  */
    UNLOCK_WHEN_HOOK()

    return result;
}

#define OS_STOP_SEC_CODE
#include "tpl_memmap.h"

/* End of file tpl_as_counter.c */