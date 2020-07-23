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
	//�����ж�
	if (argc != 3)//�жϱ���������������������������--help���ж�Ϊ�Ƿ�����
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
	
	
	
	//�ļ�·����
	char sur_file[500] = { 0 };//��¼Դ�ļ�Ŀ¼+�ļ���
	char des_file[500] = { 0 };//Ŀ���ļ���+�ļ���
	//opendir readdir
	DIR* source_dir;
	DIR* target_dir;
	struct dirent* sur_ent;
	struct dirent* tar_ent;
	
	struct stat st;//���붨��ṹ�壬ȡ��ַ���Σ������Ƕ���*
	
	//���ԴĿ¼��Ŀ��Ŀ¼�����ڣ����Դ�
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
	//�ֽ��̸���
	pid_t pid_id;
	//pid_id = fork();
	if (pid_id == 0)//==0��ʾ����������ӽ���
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
			while ((sur_ent = readdir(source_dir)) != NULL)//��n�ε��øú��������ظ�Ŀ¼�µ�n���ļ���Ϣ��ֱ��ȫ�����귵�ؿ�
			{
				//�ϳ�Դ�ļ���Ŀ���ļ�·��
				strcpy(sur_file, argv[1]); strcpy(des_file, argv[2]);
				strcat(sur_file, sur_ent->d_name); strcat(des_file, sur_ent->d_name);
		
			//���Դ�ļ�·����������ļ���������������
				stat(sur_file, &st);
				if (S_ISDIR(st.st_mode))//���ļ���
				{
					//cout << sur_file << "is dir,skip" << endl;
				}
				if (S_ISREG(st.st_mode))//����ͨ�ļ�
				{
					number++;//��¼�������ļ���Ŀ
					cout << "copying No." << number << "  file." << "filename is: " << sur_ent->d_name << endl;

									//cout << "copying form :" << sur_file << endl;
									//cout << "copying to :" << des_file << endl << endl << endl << endl;
	
					cout << "copying please wait..." << endl;
					//pid_id = fork();
					//�����ļ�����
					copyfile(sur_file, des_file);
					//�������·��
					memset(sur_file, 0, sizeof(sur_file)); memset(des_file, 0, sizeof(des_file));
				}	
			}
			cout << "copy complete!" << endl;
			number = 0;
		}
	}
	else if (pid_id > 0)//������
	{
		while ((sur_ent = readdir(source_dir)) != NULL)//��n�ε��øú��������ظ�Ŀ¼�µ�n���ļ���Ϣ��ֱ��ȫ�����귵�ؿ�
		{
			//�ϳ�Դ�ļ���Ŀ���ļ�·��
			strcpy(sur_file, argv[1]); strcpy(des_file, argv[2]);
			strcat(sur_file, sur_ent->d_name); strcat(des_file, sur_ent->d_name);
		
		//���Դ�ļ�·����������ļ���������������
			stat(sur_file, &st);
			if (S_ISDIR(st.st_mode))//���ļ���
			{
				//cout << sur_file << "is dir,skip" << endl;
			}
			if (S_ISREG(st.st_mode))//����ͨ�ļ�
			{
				number++;//��¼�������ļ���Ŀ
				cout << "copying No." << number << "  file." << "filename is: " << sur_ent->d_name << endl;

													//cout << "copying form :" << sur_file << endl;
													//cout << "copying to :" << des_file << endl << endl << endl << endl;
	
				cout << "copying please wait..." << endl;
				//pid_id = fork();
				//�����ļ�����
				copyfile(sur_file, des_file);
				//�������·��
				memset(sur_file, 0, sizeof(sur_file)); memset(des_file, 0, sizeof(des_file));
			}	
		}
		return 0;
	}
	
	

	return 0;
}
/*
man stat;
stat.h�����ж���ʲô�ļ��ĺ꣺
S _ I S R E G ( ) ��ͨ�ļ�
S _ I S D I R ( ) Ŀ¼�ļ�
S _ I S C H R ( ) �ַ������ļ�
S _ I S B L K ( ) �������ļ�
S _ I S F I F O ( ) �ܵ���F I F O
S _ I S L N K ( ) �������ӣ� P O S I X . 1��S V R 4�޴����ͣ�
 
S _ I S S O C K ( ) �׽��֣�P O S I X . 1��S V R 4�޴����ͣ�

������Բ���APUE�ļ�Ŀ¼����½ڡ�
*/

int copyfile(char* sur_file, char* des_file)
{
	//�ļ��򿪣���ȡfd
	int sur_fd;
	int des_fd;
	sur_fd = open(sur_file, O_RDONLY);//Դ�ļ�·��
	if (sur_fd < 0)
	{		
		perror("open source file error:");
		return -1;
	}
	des_fd = open(des_file, O_CREAT | O_WRONLY | O_TRUNC);//�����ļ�·��,������д�룬���
	if (des_fd < 0)
	{
		perror("open target file error:");
		return -1;
	}
	
	//��������д��ȥ��ѭ��ʵ�ֿ���
	int r_size = 0;
	double d_r_size = 0;
	char buffer[5000] = { 0 };//�����ٶȺ������С������
	
	while ((r_size = read(sur_fd, buffer, sizeof(buffer))) > 0)
	{
		//���read��������ֵΪ0����ʾ�ļ�����ĩβ��

		if (write(des_fd, buffer, r_size) == -1)//ʵ�ʶ�������r_size�ĳ��ȣ�����д��Ҫ�������ļ���ַ
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
	
	return 0;//�����ɹ�	
}

/*
int isFileExist(const char* filePath)
{
	if (access(filePath, F_OK) == 0)
	{
		return 0;
	}
	return 1;//û���ļ����ڣ���������
}
*/
