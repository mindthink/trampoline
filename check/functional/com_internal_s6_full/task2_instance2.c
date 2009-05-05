/*Instance 2 of task t2*/

#include "embUnit.h"
#include "tpl_os.h"

DeclareMessage(rm_maskednewequalsx);

/*test case:test the reaction of the system called with 
 an activation of a task*/
static void test_t2_instance2(void)
{
	StatusType result_inst_1, result_inst_2;
	StatusType received_char;
	
	SCHEDULING_CHECK_INIT(20);
	result_inst_1 = ReceiveMessage(rm_maskednewequalsx, &received_char);
	SCHEDULING_CHECK_AND_EQUAL_INT_FIRST(20,E_OK, result_inst_1);
	SCHEDULING_CHECK_AND_EQUAL_INT(20,7, (int)received_char);
	
	SCHEDULING_CHECK_INIT(21);
	result_inst_2 = TerminateTask();
	SCHEDULING_CHECK_AND_EQUAL_INT(21,E_OK, result_inst_2);
}

/*create the test suite with all the test cases*/
TestRef COMInternalTest_seq6_t2_instance2(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("test_t2_instance2",test_t2_instance2)
	};
	EMB_UNIT_TESTCALLER(COMInternalTest,"COMInternalTest_sequence6",NULL,NULL,fixtures);
	
	return (TestRef)&COMInternalTest;
}