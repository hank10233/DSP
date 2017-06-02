#include "ASA_Lib.h"
//==========================JK=======================================
#define principal_name "Jk"
#define principal_cel "0955558449"
#define principal_identi 0 //p:0 t:1 s:2
#define principal_class 0000 //empc
#define principal_ini {principal_name,principal_cel,principal_identi,principal_class} 
//==========================GAY=======================================
#define tea_1_name "Gay"
#define tea_1_cel "0923564812"
#define tea_1_identi 1 //p:0 t:1 s:2
#define tea_1_class 1000 //empc
#define tea_1_ini {tea_1_name,tea_1_cel,tea_1_identi,tea_1_class} 
//==========================BAN=======================================
#define tea_2_name "Ban"
#define tea_2_cel "0958296409"
#define tea_2_identi 1 //p:0 t:1 s:2
#define tea_2_class 100 //empc
#define tea_2_ini {tea_2_name,tea_2_cel,tea_2_identi,tea_2_class} 
//==========================TARY=======================================
#define tea_3_name "Tary"
#define tea_3_cel "0957644483"
#define tea_3_identi 1 //p:0 t:1 s:2
#define tea_3_class 10 //empc
#define tea_3_ini {tea_3_name,tea_3_cel,tea_3_identi,tea_3_class} 
//==========================YAHI=======================================
#define tea_4_name "Yahi"
#define tea_4_cel "0979800685"
#define tea_4_identi 1 //p:0 t:1 s:2
#define tea_4_class 1 //empc
#define tea_4_ini {tea_4_name,tea_4_cel,tea_4_identi,tea_4_class} 
//=========================GABE========================================
#define stud_1_name "Gabe"
#define stud_1_cel "0912346832"
#define stud_1_identi 2 //p:0 t:1 s:2
#define stud_1_class 111 //empc
#define stud_1_ini {stud_1_name,stud_1_cel,stud_1_identi,stud_1_class} 
//===========================WTFG======================================
#define stud_2_name "Wtfg"
#define stud_2_cel "0967500422"
#define stud_2_identi 2 //p:0 t:1 s:2
#define stud_2_class 1011 //empc
#define stud_2_ini {stud_2_name,stud_2_cel,stud_2_identi,stud_2_class} 
//===========================Holy======================================
#define stud_3_name "Holy"
#define stud_3_cel "0901277576"
#define stud_3_identi 2 //p:0 t:1 s:2
#define stud_3_class 1101 //empc
#define stud_3_ini {stud_3_name,stud_3_cel,stud_3_identi,stud_3_class} 
//===========================HKP======================================
#define stud_4_name "Hkp"
#define stud_4_cel "0923564328"
#define stud_4_identi 2 //p:0 t:1 s:2
#define stud_4_class 1110 //empc
#define stud_4_ini {stud_4_name,stud_4_cel,stud_4_identi,stud_4_class} 
//=================================================================
#define man_number 9
//===========================CLASS_ENG======================================
#define class_name_1 "english"
#define class_name_2 "MATH"
#define class_name_3 "PHYSICAL"
#define class_name_4 "CHEMISTRY"
#define cla_1 {class_name_1,3,tea_1_ini,{stud_2_ini,stud_3_ini,stud_4_ini}}
#define cla_2 {class_name_2,3,tea_2_ini,{stud_1_ini,stud_3_ini,stud_4_ini}}
#define cla_3 {class_name_3,3,tea_3_ini,{stud_1_ini,stud_4_ini,stud_4_ini}}
#define cla_4 {class_name_4,3,tea_4_ini,{stud_1_ini,stud_2_ini,stud_3_ini}}
//=================================================================
#define school_name "the school"
#define school_ini {school_name,man_number,principal_ini,{cla_1,cla_2,cla_3,cla_4}}
//=================================================================
typedef struct member_information{
    char *name;
    char *cel_num;
	int identity;
	int cLass;
}mem_info;
//=================================================================

typedef struct class_information{
    char *name;
    int mem_num;
	mem_info teacher;
	mem_info student[3];

}class_info;
//=================================================================
typedef struct school_information{
    char *name;
    int mem_num;
	mem_info principal;
	class_info class[4];
}school_info;
//=================================================================

int main(void)
{
	ASA_M128_set();
	
	school_info new_school=school_ini;
	
	printf("%s\n",new_school.name);
	printf("principal: %s\n",new_school.principal.name);
	for(int i=0;i<4;i++){
		printf("calss: %s  teacher: %s  cell phone: %s\n",new_school.class[i].name,new_school.class[i].teacher.name,new_school.class[i].teacher.cel_num);
		printf("student:");
		for(int j=0;j<3;j++){
			printf("  %s",new_school.class[i].student[j].name);
		}
		printf("\n");
	}
	return 0;
}
