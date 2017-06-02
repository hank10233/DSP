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
#define man_ini {principal_ini,tea_1_ini,tea_2_ini,tea_3_ini,tea_4_ini,stud_1_ini,stud_2_ini,stud_3_ini,stud_4_ini}
#define man_number 9
//===========================CLASS_ENG======================================
#define class_name_1 "english"
#define class_name_2 "MATH"
#define class_name_3 "PHYSICAL"
#define class_name_4 "CHEMISTRY"
//=================================================================
#define school_name "the school"
#define teacher_num 4
#define student_num 4
//=================================================================
typedef struct member_information{
    char *name;
    char *cel_num;
	int identity;
	int cLass;
}mem_info;
//=================================================================
typedef struct data_struct{
	mem_info *data[20];
	int data_num;
}data_str;
//=================================================================
typedef struct class_information{
    char *name;
    int mem_num;
	mem_info *p_teacher;
	mem_info *p_student[3];

}class_info;
//=================================================================
typedef struct school_information{
    char *name;
    int mem_num;
	mem_info *p_principal;
	class_info *p_class[4];
}school_info;
//=================================================================

int main(void)
{
	ASA_M128_set();
	
	data_str *p_data,data_x;
	p_data=&data_x;
	mem_info arry[man_number]=man_ini;
	for(int i=0;i<20;i++){
		p_data->data[i]=NULL;
	}
	p_data->data_num=0;
	for(int i=0;i<man_number;i++){
		p_data->data[i]=arry+i;
		p_data->data_num++;
	}

	//=================================================

	school_info *p_school,the_school;
	p_school=&the_school;
	p_school->name=school_name;
	p_school->mem_num=p_data->data_num;
	class_info eng_class;
	class_info math_class;
	class_info phy_class;
	class_info chemi_class;
	p_school->p_class[0]=&eng_class;
	p_school->p_class[1]=&math_class;
	p_school->p_class[2]=&phy_class;
	p_school->p_class[3]=&chemi_class;
	p_school->p_class[0]->name=class_name_1;
	p_school->p_class[1]->name=class_name_2;
	p_school->p_class[2]->name=class_name_3;
	p_school->p_class[3]->name=class_name_4;
	for(int i=0;i<4;i++){
		p_school->p_class[i]->mem_num=0;
		for(int j=0;j<3;j++){
			p_school->p_class[i]->p_student[j]=NULL;
		}
	}
	for(int i=0;i<p_data->data_num;i++){
		if(p_data->data[i]->identity==0){
			p_school->p_principal=p_data->data[i];
		}
		else if(p_data->data[i]->identity==1){
			if(p_data->data[i]->cLass/1000==1)
				p_school->p_class[0]->p_teacher=p_data->data[i];
			if(p_data->data[i]->cLass%1000/100==1) 
				p_school->p_class[1]->p_teacher=p_data->data[i];
			if(p_data->data[i]->cLass%100/10==1) 
				p_school->p_class[2]->p_teacher=p_data->data[i];
			if(p_data->data[i]->cLass%10==1) 
				p_school->p_class[3]->p_teacher=p_data->data[i];
		}
		else if(p_data->data[i]->identity==2){
			if(p_data->data[i]->cLass/1000==1){
				for(int j=0;j<3;j++){
					if(p_school->p_class[0]->p_student[j]==NULL){
						p_school->p_class[0]->p_student[j]=p_data->data[i];
						p_school->p_class[0]->mem_num++;
						break;
					}
					else if (p_school->p_class[0]->p_student[2]!=NULL){
						printf("out of limit1  ");
					}
				}
			}
			if(p_data->data[i]->cLass%1000/100==1){
				for(int j=0;j<3;j++){
					if(p_school->p_class[1]->p_student[j]==NULL){
						p_school->p_class[1]->p_student[j]=p_data->data[i];
						p_school->p_class[1]->mem_num++;
						break;
					}
					else if (p_school->p_class[1]->p_student[2]!=NULL) {
						printf("out of limit2  ");
					}
				}
			}
			if(p_data->data[i]->cLass%100/10==1){
				for(int j=0;j<3;j++){
					if(p_school->p_class[2]->p_student[j]==NULL){
						p_school->p_class[2]->p_student[j]=p_data->data[i];
						p_school->p_class[2]->mem_num++;
						break;
					}
					else if (p_school->p_class[2]->p_student[2]!=NULL) {
						printf("out of limit3  ");
					}
				}
			}
			if(p_data->data[i]->cLass%10==1){
				for(int j=0;j<3;j++){
					if(p_school->p_class[3]->p_student[j]==NULL){
						p_school->p_class[3]->p_student[j]=p_data->data[i];
						p_school->p_class[3]->mem_num++;
						break;
					}
					else if (p_school->p_class[3]->p_student[2]!=NULL) {
						printf("out of limit4  ");
					}
				}
			}
		}
	}
	
	printf("%s\n",p_school->name);
	printf("principal: %s\n",p_school->p_principal->name);
	for(int i=0;i<4;i++){
		printf("calss: %s  teacher: %s\n",p_school->p_class[i]->name,p_school->p_class[i]->p_teacher->name);
		printf("student:");
		for(int j=0;j<3;j++){
			printf("  %s",p_school->p_class[i]->p_student[j]->name);
		}
		printf("\n");
	}
	return 0;
}
