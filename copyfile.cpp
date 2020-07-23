#include <iostream>
#include<unistd.h>//unix stand lib
#include<sys/types.h>
#include<sys/fcntl.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<dirent.h>//file dir
#include <sys/wait.h>//wait func

using namespace std;

int number = 0;

int copyfile(char* sur_file, char* des_file);
int isFileExist(const char* filePath);

int main(int argc, char *argv[])
{
	//输入判断
	if (argc != 3)//判断必须输入两个参数，否则若不是--help则判断为非法输入
	{
		if (argc == 2&&(!strcmp(argv[1], "--help")))
		{
			cout << "This is copyFile's help document" << endl
				<< " You need to enter two parameters," << endl
				<< "the first is the path of the source file " << endl
				<< "and the second is the path of the destination file" << endl
				<< "example:" << endl
				<< " ./2020.7.18copymovies /mnt/hgfs/ShareWithUbuntu/ /mnt/hgfs/ShareWithUbuntu/copymov/" << endl << endl
				<< "The system will report an error if you enter the wrong source file path or if you enter no more than two input parameters" << endl
				<< "I hope you enjoy it :)" << endl;
			
			return -1;
		}
		else
		{
			cout << "illegal input!" << endl;
			return -1;
		}

	}
	
	
	
	//文件路径用
	char sur_file[500] = { 0 };//记录源文件目录+文件名
	char des_file[500] = { 0 };//目标文件夹+文件名
	//opendir readdir
	DIR* source_dir;
	DIR* target_dir;
	struct dirent* sur_ent;
	struct dirent* tar_ent;
	
	struct stat st;//必须定义结构体，取地址传参，而不是定义*
	
	//检查源目录和目标目录都存在，可以打开
	if ((source_dir = opendir(argv[1])) == NULL)
	{
		perror("open source dir error:");
		return -1;
	}
	if ((target_dir = opendir(argv[2])) == NULL)
	{
		perror("open target dir error:");
		return -1;
	}
	//================================================================================================
	//分进程复制
	pid_t pid_id;
	//pid_id = fork();
	if (pid_id == 0)//==0表示这个进程是子进程
	{
		pid_id = fork();
		//cout << "fork1" << endl;
		if (pid_id == 0)
		{
			pid_id = fork();
			//cout << "fork2" << endl;
		}
		else if (pid_id > 0)
		{
			while ((sur_ent = readdir(source_dir)) != NULL)//第n次调用该函数，返回该目录下第n个文件信息，直到全部读完返回空
			{
				//合成源文件和目标文件路径
				strcpy(sur_file, argv[1]); strcpy(des_file, argv[2]);
				strcat(sur_file, sur_ent->d_name); strcat(des_file, sur_ent->d_name);
		
			//检查源文件路径，如果是文件夹则跳过不复制
				stat(sur_file, &st);
				if (S_ISDIR(st.st_mode))//是文件夹
				{
					//cout << sur_file << "is dir,skip" << endl;
				}
				if (S_ISREG(st.st_mode))//是普通文件
				{
					number++;//记录拷贝的文件数目
					cout << "copying No." << number << "  file." << "filename is: " << sur_ent->d_name << endl;

									//cout << "copying form :" << sur_file << endl;
									//cout << "copying to :" << des_file << endl << endl << endl << endl;
	
					cout << "copying please wait..." << endl;
					//pid_id = fork();
					//复制文件函数
					copyfile(sur_file, des_file);
					//清空两个路径
					memset(sur_file, 0, sizeof(sur_file)); memset(des_file, 0, sizeof(des_file));
				}	
			}
			cout << "copy complete!" << endl;
			number = 0;
		}
	}
	else if (pid_id > 0)//父进程
	{
		while ((sur_ent = readdir(source_dir)) != NULL)//第n次调用该函数，返回该目录下第n个文件信息，直到全部读完返回空
		{
			//合成源文件和目标文件路径
			strcpy(sur_file, argv[1]); strcpy(des_file, argv[2]);
			strcat(sur_file, sur_ent->d_name); strcat(des_file, sur_ent->d_name);
		
		//检查源文件路径，如果是文件夹则跳过不复制
			stat(sur_file, &st);
			if (S_ISDIR(st.st_mode))//是文件夹
			{
				//cout << sur_file << "is dir,skip" << endl;
			}
			if (S_ISREG(st.st_mode))//是普通文件
			{
				number++;//记录拷贝的文件数目
				cout << "copying No." << number << "  file." << "filename is: " << sur_ent->d_name << endl;

													//cout << "copying form :" << sur_file << endl;
													//cout << "copying to :" << des_file << endl << endl << endl << endl;
	
				cout << "copying please wait..." << endl;
				//pid_id = fork();
				//复制文件函数
				copyfile(sur_file, des_file);
				//清空两个路径
				memset(sur_file, 0, sizeof(sur_file)); memset(des_file, 0, sizeof(des_file));
			}	
		}
		return 0;
	}
	
	

	return 0;
}
/*
man stat;
stat.h中有判断是什么文件的宏：
S _ I S R E G ( ) 普通文件
S _ I S D I R ( ) 目录文件
S _ I S C H R ( ) 字符特殊文件
S _ I S B L K ( ) 块特殊文件
S _ I S F I F O ( ) 管道或F I F O
S _ I S L N K ( ) 符号连接（ P O S I X . 1或S V R 4无此类型）
 
S _ I S S O C K ( ) 套接字（P O S I X . 1或S V R 4无此类型）

具体可以参照APUE文件目录相关章节。
*/

int copyfile(char* sur_file, char* des_file)
{
	//文件打开，获取fd
	int sur_fd;
	int des_fd;
	sur_fd = open(sur_file, O_RDONLY);//源文件路径
	if (sur_fd < 0)
	{		
		perror("open source file error:");
		return -1;
	}
	des_fd = open(des_file, O_CREAT | O_WRONLY | O_TRUNC);//拷贝文件路径,创建，写入，清空
	if (des_fd < 0)
	{
		perror("open target file error:");
		return -1;
	}
	
	//读出来再写进去的循环实现拷贝
	int r_size = 0;
	double d_r_size = 0;
	char buffer[5000] = { 0 };//拷贝速度和这个大小成正比
	
	while ((r_size = read(sur_fd, buffer, sizeof(buffer))) > 0)
	{
		//如果read函数返回值为0，表示文件读到末尾处

		if (write(des_fd, buffer, r_size) == -1)//实际读到的是r_size的长度，把它写入要拷贝的文件地址
		{
			perror("write error:");	
			return -1;
		}
		else
		{
			//cout << "copying..." << endl;
		}
		memset(buffer, 0, sizeof(buffer));
			
	}
	
	return 0;//拷贝成功	
}

/*
int isFileExist(const char* filePath)
{
	if (access(filePath, F_OK) == 0)
	{
		return 0;
	}
	return 1;//没有文件存在，不会重名
}
*/
