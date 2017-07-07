#include "header.h"
char	choice;
int		argc;		// �û�����Ĳ�������
char	*argv[5];		// �û�����Ĳ���
int		inum_cur;		// ��ǰĿ¼
char	temp[2 * BLKSIZE];	// ������
User	user;		// ��ǰ���û�
char	bitmap[BLKNUM];	// λͼ����
Inode	inode_array[INODENUM];	// i�ڵ�����
File_table file_array[FILENUM];	// ���ļ�������
char	image_name[10] = "hd.dat";	// �ļ�ϵͳ����
FILE	*fp;					// ���ļ�ָ��

//����ӳ��hd�����������û����ļ����
void format(void)
{
	FILE  *fp;
	int   i;
	Inode inode;
	printf("Will be to format filesystem...\n");
	printf("WARNING:ALL DATA ON THIS FILESYSTEM WILL BE LOST!\n");
	printf("Proceed with Format(Y/N)?");
	scanf("%c", &choice);
	gets_s(temp);
	if ((choice == 'y') || (choice == 'Y'))
	{
		if ((fp = fopen(image_name, "w+b")) == NULL)
		{
			printf("Can't create file %s\n", image_name);
			exit(-1);
		}
		for (i = 0; i < BLKSIZE; i++)
			fputc('0', fp);
		inode.inum = 0;
		strcpy(inode.file_name, "/");
		inode.type = 'd';
		strcpy(inode.user_name, "all");
		inode.iparent = 0;
		inode.length = 0;
		inode.address[0] = -1;
		inode.address[1] = -1;
		fwrite(&inode, sizeof(Inode), 1, fp);
		inode.inum = -1;
		for (i = 0; i < 31; i++)
			fwrite(&inode, sizeof(Inode), 1, fp);
		for (i = 0; i < BLKNUM*BLKSIZE; i++)
			fputc('\0', fp);
		fclose(fp);
		// ���ļ�user.txt
		if ((fp = fopen("user.txt", "w+")) == NULL)
		{
			printf("Can't create file %s\n", "user.txt");
			exit(-1);
		}
		fclose(fp);
		printf("Filesystem created successful.Please first login!\n");
	}
	return;
}
// ����: �û���½����������û��򴴽��û�
void login(void)
{
	char *p;
	int  flag;
	char user_name[10];
	char password[10];
	char file_name[10] = "user.txt";
	do
	{
		printf("login:");
		gets_s(user_name);
		printf("password:");
		p = password;
		while (*p = getch())
		{
			if (*p == 0x0d) //������س���ʱ��0x0dΪ�س�����ASCII��
			{
				*p = '\0'; //������Ļس���ת���ɿո�
				break;
			}
			printf("*");   //�������������"*"����ʾ
			p++;
		}
		flag = 0;
		if ((fp = fopen(file_name, "r+")) == NULL)
		{
			printf("\nCan't open file %s.\n", file_name);
			printf("This filesystem not exist, it will be create!\n");
			format();
			login();
		}
		while (!feof(fp))
		{
			fread(&user, sizeof(User), 1, fp);
			// �Ѿ����ڵ��û�, ��������ȷ
			if (!strcmp(user.user_name, user_name) &&
				!strcmp(user.password, password))
			{
				fclose(fp);
				printf("\n");
				return;
			}
			// �Ѿ����ڵ��û�, ���������
			else if (!strcmp(user.user_name, user_name))
			{
				printf("\nThis user is exist, but password is incorrect.\n");
				flag = 1;
				fclose(fp);
				break;
			}
		}
		if (flag == 0) break;
	} while (flag);
	// �������û�
	if (flag == 0)
	{
		printf("\nDo you want to creat a new user?(y/n):");
		scanf("%c", &choice);
		gets_s(temp);
		if ((choice == 'y') || (choice == 'Y'))
		{
			strcpy(user.user_name, user_name);
			strcpy(user.password, password);
			fwrite(&user, sizeof(User), 1, fp);
			fclose(fp);
			return;
		}
		if ((choice == 'n') || (choice == 'N'))
			login();
	}
}
// ����: ������i�ڵ�����ڴ�
void init(void)
{
	int   i;
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s.\n", image_name);
		exit(-1);
	}
	// ����λͼ
	for (i = 0; i < BLKNUM; i++)
		bitmap[i] = fgetc(fp);
	// ��ʾλͼ
	// ����i�ڵ���Ϣ
	for (i = 0; i < INODENUM; i++)
		fread(&inode_array[i], sizeof(Inode), 1, fp);
	// ��ʾi�ڵ�
	// ��ǰĿ¼Ϊ��Ŀ¼
	inum_cur = 0;
	// ��ʼ�����ļ���
	for (i = 0; i < FILENUM; i++)
		file_array[i].inum = -1;
}
// ����: �����û�����, ������������argc��argv
// ���: 0-14Ϊϵͳ����, 15Ϊ�������
int analyse(char *str)
{
	int  i;
	char temp[20];
	char *ptr_char;
	char  *syscmd[] = { "help", "cd", "dir", "mkdir", "creat", "open", "read", "write", "close", "delete", "logout", "clear","format","quit","rd" };
	argc = 0;
	for (i = 0, ptr_char = str; *ptr_char != '\0'; ptr_char++)
	{
		if (*ptr_char != ' ')
		{
			while (*ptr_char != ' ' && (*ptr_char != '\0'))
				temp[i++] = *ptr_char++;
			argv[argc] = (char *)malloc(i + 1);
			strncpy(argv[argc], temp, i);
			argv[argc][i] = '\0';
			argc++;
			i = 0;
			if (*ptr_char == '\0') break;
		}
	}
	if (argc != 0)
	{
		for (i = 0; (i < 15) && strcmp(argv[0], syscmd[i]); i++);
		return i;
	}
	else return 15;
}
// ����: ��num��i�ڵ㱣�浽hd.dat
void save_inode(int num)
{
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s\n", image_name);
		exit(-1);
	}
	fseek(fp, 512 + num * sizeof(Inode), SEEK_SET);
	fwrite(&inode_array[num], sizeof(Inode), 1, fp);
	fclose(fp);
}
// ����: ����һ�����ݿ�
int get_blknum(void)
{
	int i;
	for (i = 0; i < BLKNUM; i++)
		if (bitmap[i] == '0') break;
	// δ�ҵ��������ݿ�
	if (i == BLKNUM)
	{
		printf("Data area is full.\n");
		exit(-1);
	}
	bitmap[i] = '1';
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s\n", image_name);
		exit(-1);
	}
	fseek(fp, i, SEEK_SET);
	fputc('1', fp);
	fclose(fp);
	return i;
}
// ����: ��i�ڵ��Ϊnum���ļ�����temp 
void read_blk(int num)
{
	int  i, len;
	char ch;
	int  add0, add1;
	len = inode_array[num].length;
	add0 = inode_array[num].address[0];
	if (len > 512)
		add1 = inode_array[num].address[1];
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s.\n", image_name);
		exit(-1);
	}
	fseek(fp, 1536 + add0*BLKSIZE, SEEK_SET);
	ch = fgetc(fp);
	for (i = 0; (i < len) && (ch != '\0') && (i < 512); i++)
	{
		temp[i] = ch;
		ch = fgetc(fp);
	}
	if (i >= 512)
	{
		fseek(fp, 1536 + add1*BLKSIZE, SEEK_SET);
		ch = fgetc(fp);
		for (; (i < len) && (ch != '\0'); i++)
		{
			temp[i] = ch;
			ch = fgetc(fp);
		}
	}
	temp[i] = '\0';
	fclose(fp);
}
// ����: ��temp����������hd��������
void write_blk(int num)
{
	int  i, len;
	int  add0, add1;
	add0 = inode_array[num].address[0];
	len = inode_array[num].length;
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s.\n", image_name);
		exit(-1);
	}
	fseek(fp, 1536 + add0*BLKSIZE, SEEK_SET);
	for (i = 0; (i<len) && (temp[i] != '\0') && (i < 512); i++)
		fputc(temp[i], fp);
	if (i == 512)
	{
		add1 = inode_array[num].address[1];
		fseek(fp, 1536 + add1*BLKSIZE, SEEK_SET);
		for (; (i < len) && (temp[i] != '\0'); i++)
			fputc(temp[i], fp);
	}
	fputc('\0', fp);
	fclose(fp);
}
// ����: �ͷ��ļ����Ϊnum���ļ�ռ�õĿռ�
void release_blk(int num)
{
	FILE *fp;
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s\n", image_name);
		exit(-1);
	}
	bitmap[num] = '0';
	fseek(fp, num, SEEK_SET);
	fputc('0', fp);
	fclose(fp);
}
// ����: ��ʾ��������
void help(void)
{
	printf("command: \n\
help   ---  show help menu \n\
clear  ---  clear the screen \n\
cd     ---  change directory \n\
mkdir  ---  make directory   \n\
creat  ---  create a new file \n\
open   ---  open a exist file \n\
read   ---  read a file \n\
write  ---  write something to a file \n\
close  ---  close a file \n\
delete ---  delete a exist file \n\
format ---  format a exist filesystem \n\
logout ---  exit user \n\
rd     ---  delete a directory \n\
quit   ---  exit this system\n");
}
//�����ļ�·��
void pathset()
{
	char path[50];
	int m, n;
	if (inode_array[inum_cur].inum == 0)
		strcpy(path, user.user_name);
	else
	{
		strcpy(path, user.user_name);
		m = 0;
		n = inum_cur;
		while (m != inum_cur)
		{
			while (inode_array[n].iparent != m)
			{
				n = inode_array[n].iparent;
			}
			strcat(path, "/");
			strcat(path, inode_array[n].file_name);
			m = n;
			n = inum_cur;
		}
	}
	printf("[%s]@", path);
}
// ����: �л�Ŀ¼(cd .. ���� cd dir1)
void cd(void)
{
	int i;
	if (argc != 2)
	{
		printf("Command cd must have two args. \n");
		return;
	}
	if (!strcmp(argv[1], ".."))
		inum_cur = inode_array[inum_cur].iparent;
	else
	{
		// ����i�ڵ�����
		for (i = 0; i < INODENUM; i++)
			if ((inode_array[i].inum>0) &&
				(inode_array[i].type == 'd') &&
				(inode_array[i].iparent == inum_cur) &&
				!strcmp(inode_array[i].file_name, argv[1]) &&
				check(i))
				break;
		if (i == INODENUM)
			printf("This directory isn't exsited.\n");
		else inum_cur = i;
	}
}
// ����: ��ʾ��ǰĿ¼�µ���Ŀ¼���ļ�(dir)
void dir(void)
{
	int i;
	int dcount = 0, fcount = 0;
	short bcount = 0;
	if (argc != 1)
	{
		printf("Command dir must have one args. \n");
		return;
	}
	// ����i�ڵ�����, ��ʾ��ǰĿ¼�µ���Ŀ¼���ļ���
	for (i = 0; i < INODENUM; i++)
		if ((inode_array[i].inum> 0) &&
			(inode_array[i].iparent == inum_cur))
		{
			if (inode_array[i].type == 'd' && check(i))
			{
				dcount++;
				printf("%-20s<DIR>\n", inode_array[i].file_name);
			}
			if (inode_array[i].type == '-' && check(i))
			{
				fcount++;
				bcount += inode_array[i].length;
				printf("%-20s%12d bytes\n", inode_array[i].file_name, inode_array[i].length);
			}
		}
	printf("\n                    %d file(s)%11d bytes\n", fcount, bcount);
	printf("                    %d dir(s) %11d bytes FreeSpace\n", dcount, 1024 * 1024 - bcount);
}
// ����: ɾ��Ŀ¼��(rd dir1)
void rd()
{
	int i, j, t, flag = 0, chk;
	if (argc != 2)
	{
		printf("Command delete must have two args. \n");
		return;
	}
	for (i = 0; i < INODENUM; i++)//���Ҵ�ɾ��Ŀ¼
		if ((inode_array[i].inum > 0) &&//�Ƿ�Ϊ��
			(inode_array[i].iparent == inum_cur) &&
			(inode_array[i].type == 'd') &&
			(!strcmp(inode_array[i].file_name, argv[1])))
		{
			chk = check(i);//����û�Ȩ��
			if (chk != 1)
			{
				printf("This directory is not yours !\n");
				return;
			}
			else j = inode_array[i].inum;
			for (t = 0; t<INODENUM; t++)
			{
				if ((inode_array[t].inum>0) &&
					(inode_array[t].iparent == j) &&
					(inode_array[i].type == '-'))
					delet(t);//Ŀ¼�����ļ���ɾ��
				else if ((inode_array[t].inum>0) &&
					(inode_array[t].iparent == j) &&
					(inode_array[i].type == 'd'))
					delet(t);//Ŀ¼���п�Ŀ¼��ɾ��
			}
			if (t == INODENUM)
				delet(j);//�²�Ŀ¼Ϊ��ɾ��֮
		}
	if (i == INODENUM)
		delet(i);//��ɾ��Ŀ¼Ϊ��ɾ��֮
	return;
}
// ����: �ڵ�ǰĿ¼�´�����Ŀ¼(mkdir dir1)
void mkdir(void)
{
	int i;
	if (argc != 2)
	{
		printf("command mkdir must have two args. \n");
		return;
	}
	// ����i�ڵ�����, ����δ�õ�i�ڵ�
	for (i = 0; i < INODENUM; i++)
		if (inode_array[i].inum < 0) break;
	if (i == INODENUM)
	{
		printf("Inode is full.\n");
		exit(-1);
	}
	inode_array[i].inum = i;
	strcpy(inode_array[i].file_name, argv[1]);
	inode_array[i].type = 'd';
	strcpy(inode_array[i].user_name, user.user_name);
	inode_array[i].iparent = inum_cur;
	inode_array[i].length = 0;
	save_inode(i);
}
// ����: �ڵ�ǰĿ¼�´����ļ�(creat file1)
void creat(void)
{
	int i;
	if (argc != 2)
	{
		printf("command creat must have one args. \n");
		return;
	}
	for (i = 0; i < INODENUM; i++)
	{
		if ((inode_array[i].inum > 0) &&
			(inode_array[i].type == '-') &&
			!strcmp(inode_array[i].file_name, argv[1]))
		{
			printf("This file is exsit.\n");
			return;
		}
	}
	for (i = 0; i < INODENUM; i++)
		if (inode_array[i].inum < 0) break;
	if (i == INODENUM)
	{
		printf("Inode is full.\n");
		exit(-1);
	}
	inode_array[i].inum = i;
	strcpy(inode_array[i].file_name, argv[1]);
	inode_array[i].type = '-';
	strcpy(inode_array[i].user_name, user.user_name);
	inode_array[i].iparent = inum_cur;
	inode_array[i].length = 0;
	save_inode(i);
}
// ����: �򿪵�ǰĿ¼�µ��ļ�(open file1)
void open()
{
	int i, inum, mode, filenum, chk;
	if (argc != 2)
	{
		printf("command open must have one args. \n");
		return;
	}
	for (i = 0; i < INODENUM; i++)
		if ((inode_array[i].inum > 0) &&
			(inode_array[i].type == '-') &&
			!strcmp(inode_array[i].file_name, argv[1]))
			break;
	if (i == INODENUM)
	{
		printf("The file you want to open doesn't exsited.\n");
		return;
	}
	inum = i;
	chk = check(i);
	if (chk != 1)
	{
		printf("This file is not your !\n");
		return;
	}
	printf("Please input open mode:(1: read, 2: write, 3: read and write):");
	scanf("%d", &mode);
	gets_s(temp);
	if ((mode < 1) || (mode > 3))
	{
		printf("Open mode is wrong.\n");
		return;
	}
	for (i = 0; i < FILENUM; i++)
		if (file_array[i].inum < 0) break;
	if (i == FILENUM)
	{
		printf("The file table is full, please close some file.\n");
		return;
	}
	filenum = i;
	file_array[filenum].inum = inum;
	strcpy(file_array[filenum].file_name, inode_array[inum].file_name);
	file_array[filenum].mode = mode;
	file_array[filenum].offset = 0;
	printf("Open file %s by ", file_array[filenum].file_name);
	if (mode == 1) printf("read only.\n");
	else if (mode == 2) printf("write only.\n");
	else printf("read and write.\n");
}
// ����: ���ļ��ж����ַ�(read file1)
void read()
{
	int i, start, num, inum;
	if (argc != 2)
	{
		printf("command read must have one args. \n");
		return;
	}
	for (i = 0; i < FILENUM; i++)
		if ((file_array[i].inum > 0) &&
			!strcmp(file_array[i].file_name, argv[1]))
			break;
	if (i == FILENUM)
	{
		printf("Open %s first.\n", argv[1]);
		return;
	}
	else if (file_array[i].mode == 2)
	{
		printf("Can't read %s.\n", argv[1]);
		return;
	}
	inum = file_array[i].inum;
	printf("The length of %s:%d.\n", argv[1], inode_array[inum].length);
	if (inode_array[inum].length > 0)
	{
		printf("The start position:");
		scanf("%d", &start);
		gets_s(temp);
		if ((start<0) || (start >= inode_array[inum].length))
		{
			printf("Start position is wrong.\n");
			return;
		}
		printf("The bytes you want to read:");
		scanf("%d", &num);
		gets_s(temp);
		if (num <= 0)
		{
			printf("The num you want to read is wrong.\n");
			return;
		}
		read_blk(inum);
		for (i = 0; (i < num) && (temp[i] != '\0'); i++)
			printf("%c", temp[start + i]);
		printf("\n");
	}
}
// ����: ���ļ���д���ַ�(write file1)
void write()
{
	int i, inum, length;
	if (argc != 2)
	{
		printf("Command write must have one args. \n");
		return;
	}
	for (i = 0; i < FILENUM; i++)
		if ((file_array[i].inum>0) &&
			!strcmp(file_array[i].file_name, argv[1])) break;
	if (i == FILENUM)
	{
		printf("Open %s first.\n", argv[1]);
		return;
	}
	else if (file_array[i].mode == 1)
	{
		printf("Can't write %s.\n", argv[1]);
		return;
	}
	inum = file_array[i].inum;
	printf("The length of %s:%d\n", inode_array[inum].file_name, inode_array[inum].length);
	if (inode_array[inum].length == 0)
	{
		printf("The length you want to write(0-1024):");
		scanf("%d", &length);
		gets_s(temp);
		if ((length < 0) && (length >1024))
		{
			printf("Input wrong.\n");
			return;
		}
		inode_array[inum].length = length;
		inode_array[inum].address[0] = get_blknum();
		if (length > 512)
			inode_array[inum].address[1] = get_blknum();
		save_inode(inum);
		printf("Input the data(Enter to end):\n");
		gets_s(temp);
		write_blk(inum);
	}
	else
		printf("This file can't be written.\n");
}
// ����: �ر��Ѿ��򿪵��ļ�(close file1)
void close(void)
{
	int i;
	if (argc != 2)
	{
		printf("Command close must have one args. \n");
		return;
	}
	for (i = 0; i < FILENUM; i++)
		if ((file_array[i].inum > 0) &&
			!strcmp(file_array[i].file_name, argv[1])) break;
	if (i == FILENUM)
	{
		printf("This file doesn't be opened.\n");
		return;
	}
	else
	{
		file_array[i].inum = -1;
		printf("Close %s successful!\n", argv[1]);
	}
}
//ɾ��Ŀ¼��
void delet(int innum)
{
	/*int chk;
	chk=check(innum);
	if(chk!=1)
	{
	//printf("This directory is not yours !\n");
	return ;
	}*/
	inode_array[innum].inum = -1;
	if (inode_array[innum].length >= 0)
	{
		release_blk(inode_array[innum].address[0]);
		if (inode_array[innum].length >= 512)
			release_blk(inode_array[innum].address[1]);
	}
	save_inode(innum);
}
// ����: ɾ���ļ�(delete file1)
void del(void)
{
	int i, chk;
	if (argc != 2)
	{
		printf("Command delete must have one args. \n");
		return;
	}
	for (i = 0; i < INODENUM; i++)
		if ((inode_array[i].inum > 0) &&
			(inode_array[i].type == '-') &&
			!strcmp(inode_array[i].file_name, argv[1])) break;
	if (i == INODENUM)
	{
		printf("This file doesn't exist.\n");
		return;
	}
	chk = check(i);
	if (chk != 1)
	{
		printf("This file is not your !\n");
		return;
	}
	/*inode_array[i].inum = -1;
	if(inode_array[i].length > 0)
	{
	release_blk(inode_array[i].address[0]);
	if(inode_array[i].length > 512)
	release_blk(inode_array[i].address[1]);
	}
	save_inode(i);*/
	delet(i);
}
// ����: �˳���ǰ�û�(logout)
void logout()
{
	char choice;
	printf("Do you want to exit this user(y/n)?");
	scanf("%c", &choice);
	gets_s(temp);
	if ((choice == 'y') || (choice == 'Y'))
	{
		printf("\nCurrent user exited!\nPlease to login by other user!\n");
		login();
	}
	return;
}
//��鵱ǰI�ڵ���ļ��Ƿ����ڵ�ǰ�û�
int check(int i)
{
	int j;
	char *uuser, *fuser;
	uuser = user.user_name;
	fuser = inode_array[i].user_name;
	j = strcmp(fuser, uuser);
	if (j == 0)  return 1;
	else      return 0;
}
// ����: �˳��ļ�ϵͳ(quit)
void quit()
{
	char choice;
	printf("Do you want to exist(y/n):");
	scanf("%c", &choice);
	gets_s(temp);
	if ((choice == 'y') || (choice == 'Y'))
		exit(0);
}
// ����: ��ʾ����
void errcmd()
{
	printf("Command Error!!!\n");
}
//����ڴ��д��ڵ��û���
void free_user()
{
	int i;
	for (i = 0; i<10; i++)
		user.user_name[i] = '\0';
}
// ����: ѭ��ִ���û����������, ֱ��logout
// "help", "cd", "dir", "mkdir", "creat", "open","read", "write", "close", "delete", "logout", "clear", "format","quit","rd"
void command(void)
{
	char cmd[100];
	system("cls");
	do
	{
		pathset();
		gets_s(cmd);
		switch (analyse(cmd))
		{
		case 0:
			help();
			break;
		case 1:
			cd();
			break;
		case 2:
			dir();
			break;
		case 3:
			mkdir();
			break;
		case 4:
			creat();
			break;
		case 5:
			open();
			break;
		case 6:
			read();
			break;
		case 7:
			write();
			break;
		case 8:
			close();
			break;
		case 9:
			del();
			break;
		case 10:
			logout();
			break;
		case 11:
			system("cls");
			break;
		case 12:
			format();
			init();
			free_user();
			login();
			break;
		case 13:
			quit();
			break;
		case 14:
			rd();
			break;
		case 15:
			errcmd();
			break;
		default:
			break;
		}
	} while (1);
}
// ������
int main(void)
{
	login();
	init();
	command();
	return 0;
}