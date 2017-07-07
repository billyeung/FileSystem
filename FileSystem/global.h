#ifndef  _GLOBAL_H
#define _GLOBAL_H
#define BLKSIZE 512 //���ݿ��С
#define BLKNUM 512 //���ݿ���Ŀ
#define INODESIZE 32 //i�ڵ��С
#define INODENUM 32 //i�ڵ���Ŀ
#define NBLOCKS 2 //�ļ���ռ���ݿ���
#define MAXLEN 10 //�ļ�����󳤶�
#define DIRNUM 10 //Ŀ¼����
#define FBLNUM 50 //���п��ջ���
#define FINNUM 50 //����i�ڵ�������
#define UNAMELEN 10 //�û�������
#define UPWDLEN 10 //�û����볤��
#define DNAMELEN 10 //Ŀ¼������
#define FILENUM 10 //���ļ���
#define SYSOFILE 10 //ϵͳ�򿪱���
#define USERNUM 8 //�û�����

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

/**
 * \brief ������
 */
typedef struct
{
	//unsigned short s_size; //���ݿ���ռ�̿���
	unsigned int s_inode_size; //inode����ռ�̿���

	//unsigned short s_inodes_count; //i�ڵ�����
	//unsigned short s_blocks_count; //�̿�����

	unsigned int s_free_blocks_count; //�������ݿ����
	unsigned short s_next_free_block; //�������ݿ�ָ��
	unsigned int s_free_blocks_stack[FBLNUM]; //�������ݿ��ջ

	unsigned int s_free_inodes_count; //����i�ڵ�����
	unsigned short s_next_free_inode; //����i�ڵ�ָ��
	unsigned int s_free_inodes_stack[FINNUM]; //����i�ڵ�����

	unsigned short *s_rinode;

	//unsigned short s_first_data_block; //��һ�����ݿ��ַ
	//unsigned short s_block_size; //���ݿ��С
	//unsigned short s_mtime; //����ʱ��
	//unsigned short s_state; //�ļ�ϵͳ״̬

	char s_flag; //�޸ı�־
}SuperBlock;

/**
 * \brief ����i�ڵ�
 */
typedef struct
{
	//short fi_id; //i�ڵ��
	unsigned short fi_state; //ռ�����
	unsigned short fi_mode; //�ļ����ͣ�Ŀ¼/��ͨ�ļ�
	unsigned short fi_mtime; //��һ���ļ��޸�ʱ��
	unsigned short fi_ctime; //��һ��inode�޸�ʱ��
	unsigned short fi_atime; //��һ���ļ�����ʱ��
	unsigned short fi_uid; //�����û�id
	unsigned short fi_gid; //�����û���id
	unsigned int fi_size; //�ļ��ֽ���
	unsigned int fi_block[NBLOCKS]; //����block�ĵ�ַָ������
	unsigned short fi_links_count; //����������Ϊ0ʱ��ζ�ű�ɾ��
	char fi_acl[3]; //��ȡȨ��
}Finode;

/**
 * \brief �ڴ�i�ڵ�
 * \details �ڴ���������Ǳ������ڴ��������������ݽṹ��
 * ���ļ���һ�α���ʱ���ļ�����������ģ������϶�����
 * ���������ڴ��У�������һ���ļ��Ĵ򿪡� 
 */
typedef struct inode
{
	unsigned short i_ino; //��������
	char i_flag; //i�ڵ�״̬
	unsigned short i_count; //���ü���
	Finode finode; //����i���ṹ������Ӵ��̶�����i�����Ϣ 
	struct inode *prevptr; //��һ���ڴ�i���ָ��
	struct inode *nextptr; //��һ���ڴ�i���ָ��
}Inode;

/**
 * \brief �û�
 */
typedef struct
{
	unsigned short u_uid; //�û�id
	unsigned short u_gid; //�û���id
	char u_name[UNAMELEN]; //�û���
	char u_password[UPWDLEN]; //����
}User;

/**
 * \brief Ŀ¼��
 * \details �ļ�Ŀ¼�����ļ������ļ������������
 */
typedef struct
{
	char d_name[DNAMELEN]; //�ļ���Ŀ¼��
	unsigned int d_ino; //i�ڵ��
}Dentry;

/**
* \brief Ŀ¼
* \details ����Ŀ¼�࣬�������ݶ�����dir�ṹ�����ڴ����еģ�����dir�ṹ��ȡ
*/
typedef struct
{
	Dentry dentry[DIRNUM]; //Ŀ¼������
	unsigned short size; //Ŀ¼����Ŀ
}Dir;

/**
 * \brief ϵͳ�򿪱�
 * \details ���ļ�����ʱ����
 */
typedef struct
{
	char sf_flag; //�ļ�������־
	unsigned short sf_count; //���ü���
	inode *sf_inode; //ָ���ļ����ڴ�i�ڵ�
	unsigned int sf_curpos; //��ǰ��дָ��
}Sys_OFile;

/**
 * \brief �û��򿪱�
 */
typedef struct
{
	unsigned char uf_default_acl[3]; //Ĭ�ϴ�ȡȨ��
	unsigned short uf_uid; //�û�id
	unsigned short uf_gid; //�û���id
	unsigned short uf_ofile[FILENUM]; //�û����ļ���
}User_OFile;

//ȫ�ֱ���
extern Dir dire;
extern Sys_OFile sys_ofile;
extern SuperBlock superblk;
extern User users[USERNUM];
extern FILE *fp;
extern Inode *cur_path_inode;
extern unsigned short user_id;
extern unsigned int file_block;


//��������
extern void login(void);
extern void init(void);
extern int  analyse(char *);
extern void save_inode(int);
extern int	get_blknum(void);
extern void read_blk(int);
extern void write_blk(int);
extern void release_blk(int);
extern void pathset();
extern void delet(int innum);
extern int  check(int i);

//�û��������
extern void help(void);
extern void cd(void);
extern void dir(void);
extern void mkdir(void);
extern void creat(void);
extern void open(void);
extern void read(void);
extern void write(void);
extern void close(void);
extern void del(void);
extern void logout(void);
extern void command(void);
extern void rd();
extern void quit();



#endif