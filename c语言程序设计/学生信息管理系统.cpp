/**
 * 项目名称：学生信息管理系统
 * 功能介绍：用文件存储学生成绩并计算总分平均分（完整的增、删、改、查功能）
 * 开发日期：2023.12
 * 作    者：杨思敏，张彤，余芃稼，林芙巧
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct student{      //定义一个名为student的结构体，用来表示学生的各种信息
	char name[20];
	char id[20];
	int math;
	int english;
	int cyy;//c语言
	int sum;//总分
	int average;//平均分
};
struct Node{             //定义了一个名为node的结构体，这个结构体是用于构建一个单向链表的节点。
	struct student data; //这个结构体是用于构建一个单向链表的节点。每个节点存储一个学生的信息并指向下一个节点（通过 next 指针）。
	struct Node*next;
};
struct Node* list;
struct Node* creatList()//创建链表 //杨思敏
{
	struct Node* head= (struct Node*)malloc(sizeof(Node));//创建头节点 //malloc()动态内存分配函数，它分配了足够存储一个 Node 结构体的内存，并返回这块内存的地址。
	head->next=NULL;//在创建空链表时，没有下一个节点，所以将 next 设置为 NULL。                 
	return head;  //返回头节点的地址
}
struct Node*createNode(struct student data)//创建一个新的链表节点 //杨思敏   
{
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->data=data;  //将传入的 data 参数的值赋给新节点中的 data 成员
	newNode->next=NULL;
	return newNode;
}
void push_back(struct Node* headNode,struct student data)//余芃稼//向链表中添加一个新的节点
{                                                        
	struct Node* newNode = createNode(data);  //首先调用 createNode 函数创建一个新节点
	while(headNode->next!=NULL)  //通过循环找到链表的最后一个节点
	{
	headNode=headNode->next;  
	}
	//把新节点连接到尾部 
	headNode->next=newNode;
}
void print(struct Node*head)     //遍历链表中的所有节点，并打印每个节点的数据
{
	struct Node* x=head->next;
	while(x!=NULL)
	{
	printf("%s\t%s\t%d\t%d\t%d\t%d\t%d\n", x->data.name,x->data.id, x->data.math, x->data.english, x->data.cyy, x->data.sum, x->data.average);
	x=x->next;
	}
}
struct Node* search(struct Node* headNode,const char *name)      //张彤//遍历链表，搜索具有特定名字的节点
{
 struct Node* pmove = headNode->next;
 while (pmove != NULL && strcmp(pmove->data.name,name)) 
 {
  //pmove = pmove->next;
pmove = pmove->next;
 }
 return pmove;
}
void saveToFile(struct Node* headNode) //将链表中的数据写入到文件中   //余芃稼
{
 struct Node* pmove = headNode->next;
 FILE* fp = fopen("student.txt", "w");//以写的形式打开    //余芃稼
 while (pmove != NULL) 
 {
 fprintf(fp, "%s\t%s\t%d\t%d\t%d\t%d\t%d\n", pmove->data.name,pmove->data.id, pmove->data.math, pmove->data.english, pmove->data.cyy,pmove->data.sum, pmove->data.average);
 pmove = pmove->next;
 }
 fclose(fp);
}

void read(struct Node* headNode ) //读文件
{
FILE* fp = fopen("student.txt", "r");//以读的形式打开   
 struct student temp;
 while (fscanf(fp, "%s%s%d%d%d%d%d\n",temp.name,temp.id,&temp.math,&temp.english,&temp.cyy,&temp.sum,&temp.average) != EOF) 
 {
  push_back(list,temp);
 }
fclose(fp);
}
void  rdelete(struct Node* headNode,char*name)      //张彤//删除链表中指定名称的节点
{
 struct Node* preNode = headNode;
 struct Node* curNode = headNode->next;
 while (curNode != NULL && strcmp(curNode->data.name,name)) 
 {
  preNode = curNode;
  curNode = preNode->next;
 }
 if (curNode==NULL) 
 {
  printf("删除失败!,没有找到指定数据！\n");
 }
 else 
 {
  preNode->next = curNode->next;
  free(curNode);  //释放当前节点的内存，以避免内存泄漏。
 }
}
void makeMenu()   //制作菜单    // 林芙巧
{

 //排序+文件

 printf("---------【链式学生信息管理系统】---------\n");

 printf("\t0.退出功能\n");

 printf("\t1.录入功能\n");

 printf("\t2.浏览功能\n");

 printf("\t3.查找功能\n");

 printf("\t4.修改功能\n");

 printf("\t5.删除功能\n");

 printf("------------------------------------------\n");

 printf("请输入(0~5):");

}
void key()//林芙巧
{
	struct student temp;//定义输入操作的信息
	struct Node* result;//定义查找函数结果
	int a=0;
	scanf("%d",&a);
	switch(a)
    {
    	case 0:
    		printf("----【退出模块】----\n");
    		system("pause");
    		exit(0);
    		break;
    	case 1:
	        printf("----【录入模块】----\n");
            printf("请输入学生信息(学生名字,学号,高数,英语,C语言):");
            scanf("%s%s%d%d%d", temp.name, temp.id, &temp.math, &temp.english, &temp.cyy);
            temp.sum = temp.math + temp.english + temp.cyy;//计算总分
            temp.average = temp.sum / 3;//计算平均分
	        push_back(list,temp);//将创建的链表插入 
	        saveToFile(list);
	        break;
	    case 2:
	    	printf("----【浏览功能】----\n");
			printf("姓名\t学号\t高数\t英语\tc语言\t总分\t平均分\n");
	    	print(list);
	    	break;
	    case 3:
	    	printf("----【查找功能】----\n");
            printf("请输入要查找学生姓名:");
            scanf("%s",temp.name); 
            result = search(list, temp.name);
            if (result == NULL) 
			{
			 printf("未找到结果！\n");
			}
			 else 
			{
                printf("姓名\t编号\t高数\t英语\tc语言\t总分\t平均分\n");
                printf("%s\t%s\t%d\t%d\t%d\t%d\t%d\n",result->data.name,result->data.id, result->data.math, result->data.english,result->data.cyy,result->data.sum, result->data.average);
			}
	    	break;
	    case 4:
	    	printf("----【修改功能】----\n");
		    printf("请输入要修改学生姓名:");
			scanf("%s", temp.name); 
			result=search(list, temp.name);
			if (result == NULL) 
			{
			 printf("未找到结果！\n");
			}
			 else 
			{
            printf("请输入新的学生信息(学生名字,学号,高数,英语,C语言):");
            scanf("%s%s%d%d%d", result->data.name, result->data.id, &result->data.math, &result->data.english, &result->data.cyy);
            result->data.sum = result->data.math + result->data.english + result->data.cyy;
			result->data.average = result->data.sum / 3;
			saveToFile(list);
			}
	    	break;
	    	case 5:
			printf("----【删除功能】----\n");
			printf("请输入要删除学生姓名:");
			scanf("%s", temp.name,20); 
			rdelete(list, temp.name);
			saveToFile(list);
			break;
		default:
		    printf("输入错误!,请重新输入...!\n");
		    break;
	}
}
int main ()     //杨思敏
{               //调用函数，执行程序
	list =creatList();
	read(list);
	while(1)
	{
	makeMenu();
	key();
	system("pause");
    system("cls");
	}
	return 0;
}
