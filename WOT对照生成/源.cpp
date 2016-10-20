#include<iostream>
#include<fstream>
#include<string>
#include<ctime>
#include<afx.h>
#include<afxpriv.h>
#include"tinyxml2.h"
#include"json\json.h"
#pragma warning(disable:4996)

using namespace std;

int main()
{
	cout << "*********************************************************************" << endl
		<< "*                           WOT对照生成 v1.1                        *" << endl
		<< "*                    在坦克世界v0.9.15.1下测试通过                  *" << endl
		<< "*  该程序修改并使用了来自World of Tanks Mod Tools Library的部分代码 *" << endl
		<< "*                  http://wottoolslib.codeplex.com/                 *" << endl
		<< "*                wottoolslib是修改自WoTModTools的类库               *" << endl
		<< "*              https://github.com/katzsmile/WoTModTools             *" << endl
		<< "*             同时该程序也调用了GNU gettext的反编译程序             *" << endl
		<< "*               http://www.gnu.org/software/gettext/                *" << endl
		<< "*       该程序从游戏文件为每个坦克生成一些录像解析必要的参数        *" << endl
		<< "*                    具体使用方法详见Readme.md                      *" << endl
		<< "*                         本软件遵循GPLv3协议                       *" << endl
		<< "*                           祝您使用愉快！                          *" << endl
		<< "*                      ztyzbb 2016.10.20 敬上！                     *" << endl
		<< "*********************************************************************" << endl << endl;
	wcin.imbue(locale("chs"));
	wcout.imbue(locale("chs"));
	printf("请输入坦克世界安装目录（如C:\\Games\\World_of_Tanks）：");
	wchar_t path_org[101];
	wcin.getline(path_org, 100);
	if (path_org[wcslen(path_org) - 1] == L'\\')
		path_org[wcslen(path_org) - 1] = 0;
	//接收目录参数

	wchar_t path[101];
	wcscpy(path, path_org);
	wcscat(path, L"\\res\\text\\LC_MESSAGES\\");
	wcout << endl 
		<< L"当前LC_MESSAGES路径：" << path << endl 
		<< L"请务必确保路径合法有效，是否继续？如否，请右上角" << endl;
	system("pause");
	cout << endl;
	//生成LC_MESSAGES的目录


	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	//定义PI，SI，准备创建进程

	unsigned long exitcode;//接收外部调用程序的返回值
	int counter = 0;//文件计数器

	wchar_t command[150];//为节约内存，command复用，同时用于FindFile与CreateProcess
	wcscpy(command, path);
	wcscat(command, L"*_vehicles.mo");
	//生成_vehicles.mo搜索路径

	CFileFind list_file;
	BOOL bWorking = list_file.FindFile(command);
	while (bWorking)
	{
		bWorking = list_file.FindNextFile();
		wcscpy(command, L"msgunfmt.exe \"");
		wcscat(command, path);
		wcscat(command, list_file.GetFileName());
		wcscat(command, L"\" -o ");
		wcscat(command, list_file.GetFileTitle());
		wcscat(command, L".po");
		//生成调用msgunfmt.exe的命令行

		wcout << L"正在反编译：" << list_file.GetFileName().GetBuffer() << endl;
		if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		{
			wcout << endl 
				<< L"启动msgunfmt.exe失败" << endl 
				<< L"Command = " << command << endl 
				<< L"错误代码=" << GetLastError() << endl;
			system("pause");
			return -1;
		}
		//调用msgunfmt.exe，反编译mo，调用失败返回-1

		WaitForSingleObject(pi.hProcess, INFINITE);//等待程序执行
		GetExitCodeProcess(pi.hProcess, &exitcode);//获取返回值

		if (exitcode != 0)
		{
			wcout << endl 
				<< L"mo反编译失败，请根据提示排除故障！" << endl 
				<< L"Command=" << command << endl 
				<< L"返回值=" << exitcode << endl;
			system("pause");
			return -2;
		}
		//接收返回值，非0则反编译失败，返回-2

		counter++;//文件计数器+1
	}
	list_file.Close();
	cout << endl 
		<< "反编译mo过程结束，共反编译" << counter << "个mo文件" << endl 
		<< "请确保所有_vehicles.mo已经被正确反编译！是否继续？如否，请右上角" << endl;
	system("pause");
	//反编译mo文件

	wcscpy(path, path_org);
	wcscat(path, L"\\res\\scripts\\item_defs\\vehicles\\");
	wcout << endl 
		<< L"当前vehicles路径：" << path << endl 
		<< L"请务必确保路径合法有效，是否继续？如否，请右上角" << endl;
	system("pause");
	wcscat(path, L"*.*");
	//生成vehicles的目录，寻找子文件夹

	SECURITY_ATTRIBUTES sa = { sizeof(sa) };
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	//定义SECURITY_ATTRIBUTES结构体，准备创建管道

	HANDLE piperead, pipewrite;
	if (!CreatePipe(&piperead, &pipewrite, &sa, 100000))
	{
		cout << endl 
			<< "创建管道失败！错误代码=" << GetLastError() << endl;
		CloseHandle(pipewrite);
		CloseHandle(piperead);
		system("pause");
		return -3;
	}
	//创建管道，失败返回-3

	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = pipewrite;
	//设置STARTUPINFO，将stdout指向pipe

	Json::Value out;//Json用于存放解析获得的数据
	tinyxml2::XMLDocument doc;//XMLDocument用于解析XML

	printf("\n当前已知国家代码：\n0-ussr\n1-germany\n2-usa\n3-china\n4-france\n5-gb\n6-japan\n7-czech\n8-sweden\n未知国家代码以科技树在游戏中的出现顺序向后递增\n");//输出已知国家代码
	int countryid;//存储国家代码

	counter = 0;
	//重置计数器

	bWorking = list_file.FindFile(path);
	while (bWorking)
	{
		bWorking = list_file.FindNextFile();
		if (list_file.IsDots() || !list_file.IsDirectory())
			continue;
		//排除当前目录与上级目录

		wprintf(L"\n当前文件夹：%-10ls" ,list_file.GetFileName().GetBuffer());//输出当前文件夹名称

		if (!wcscmp(list_file.GetFileName().GetBuffer(), L"china"))
		{
			countryid = 3;
			cout << "自动识别countryid=3" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"common"))
		{
			cout << "跳过！" << endl;
			continue;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"czech"))
		{
			countryid = 7;
			cout << "自动识别countryid=7" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"france"))
		{
			countryid = 4;
			cout << "自动识别countryid=4" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"germany"))
		{
			countryid = 1;
			cout << "自动识别countryid=1" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"japan"))
		{
			countryid = 6;
			cout << "自动识别countryid=6" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"sweden"))
		{
			countryid = 8;
			cout << "自动识别countryid=8" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"uk"))
		{
			countryid = 5;
			cout << "自动识别countryid=5" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"usa"))
		{
			countryid = 2;
			cout << "自动识别countryid=2" << endl
				<< endl;
		}
		else if (!wcscmp(list_file.GetFileName().GetBuffer(), L"ussr"))
		{
			countryid = 0;
			cout << "自动识别countryid=0" << endl
				<< endl;
		}
		else
		{
			cout << "未知！" << endl 
				<< "如果该文件夹中有用于记录坦克数据的list.xml，请输入对应的国家代码，如果没有，请输入负数" << endl 
				<< "国家代码：";
			scanf("%d", &countryid);
			if (countryid < 0)
				continue;
			else
				cout << endl;
		}
		//自动识别countryid

		wcscpy(command, L"wottoolslib.exe \"");
		wcscat(command, list_file.GetFilePath().GetBuffer());
		wcscat(command, L"\\list.xml\"");
		//生成命令行，调用wottoolslib.exe

		if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		{
			wcout << endl 
				<< L"启动wottoolslib.exe失败" << endl 
				<< L"Command = " << command << endl 
				<< L"错误代码=" << GetLastError() << endl;
			CloseHandle(pipewrite);
			CloseHandle(piperead);
			system("pause");
			return -4;
		}
		//调用wottoolslib.exe解密xml，失败返回-4

		WaitForSingleObject(pi.hThread, INFINITE);//等待程序执行
		GetExitCodeProcess(pi.hProcess, &exitcode);//获取返回值

		unsigned long readcount;
		char *buff = new char[100001];
		ReadFile(piperead, buff, 100000, &readcount, NULL);
		buff[readcount] = 0;
		//将字符串从管道中读取出来

		if (exitcode != 0)
		{
			cout << endl 
				<< buff << endl;
			wcout << L"xml解析失败，请根据提示排除故障！" << endl 
				<< L"Command=" << command << endl 
				<< L"返回值=" << exitcode << endl;
			CloseHandle(pipewrite);
			CloseHandle(piperead);
			delete[] buff;
			system("pause");
			return -6;
		}
		//接收返回值，非0则解密失败，返回-6

		if (doc.Parse(buff))
		{
			doc.PrintError();
			wcout << endl 
				<< L"xml解析失败，请根据提示排除故障！" << endl 
				<< L"当前XML路径：" << list_file.GetFilePath().GetBuffer() << L"\\list.xml" << endl;
			CloseHandle(pipewrite);
			CloseHandle(piperead);
			delete[] buff;
			system("pause");
			return -5;
		}
		delete[] buff;
		//解析XML，失败返回-5

		tinyxml2::XMLElement *root = doc.RootElement();//根节点
		tinyxml2::XMLElement *workingtank = root->FirstChildElement();//坦克节点
		Json::Value currenttank;//生成对象，存储解析获得的坦克数据
		string tagsstr;//存储tags字符串

		while (workingtank)
		{
			currenttank["id"] = atoi(workingtank->FirstChildElement("id")->GetText() + 1);//获取坦克id
			currenttank["countryid"] = countryid;//设置国家代码
			currenttank["title"] = strchr(workingtank->FirstChildElement("userString")->GetText(), ':') + 1;//设置标题，用于匹配翻译
			currenttank["icon_orig"] = workingtank->Name();//设置坦克识别名称，可以用于匹配录像中的坦克名称
			currenttank["tier"] = atoi(workingtank->FirstChildElement("level")->GetText() + 1);//获取等级

			tagsstr = workingtank->FirstChildElement("tags")->GetText();
			if (tagsstr.find("lightTank") ==0)
			{
				currenttank["type"] = 1;
				printf("解析到%-3d级LT  %s\n",currenttank["tier"].asInt(), currenttank["icon_orig"].asCString());
			}
			else if (tagsstr.find("mediumTank") == 0)
			{
				currenttank["type"] = 2;
				printf("解析到%-3d级MT  %s\n", currenttank["tier"].asInt(), currenttank["icon_orig"].asCString());
			}
			else if (tagsstr.find("heavyTank")==0)
			{
				currenttank["type"] = 3;
				printf("解析到%-3d级HT  %s\n", currenttank["tier"].asInt(), currenttank["icon_orig"].asCString());
			}
			else if (tagsstr.find("AT-SPG")==0)
			{
				currenttank["type"] = 4;
				printf("解析到%-3d级TD  %s\n", currenttank["tier"].asInt(), currenttank["icon_orig"].asCString());
			}
			else if (tagsstr.find("SPG")==0)
			{
				currenttank["type"] = 5;
				printf("解析到%-3d级SPG %s\n", currenttank["tier"].asInt(), currenttank["icon_orig"].asCString());
			}
			else if(tagsstr.find("observer")==0)
			{
				currenttank["type"] = 1;
				cout << "解析到观察者 " << currenttank["icon_orig"].asCString() << endl;
			}
			//识别坦克类型

			out.append(currenttank);//将当前坦克对象添加到json根数组
			currenttank.clear();//清空当前坦克对象
			workingtank = workingtank->NextSiblingElement();//下一坦克节点
		}
		//循环解析坦克

		counter++;//文件计数器+1
	}
	list_file.Close();
	CloseHandle(pipewrite);
	CloseHandle(piperead);
	cout << endl 
		<< "xml解析过程结束，共反编译" << counter << "个xml文件" << endl 
		<< "请确保所有list.xml已经被正确解析！是否继续？如否，请右上角" << endl;
	system("pause");
	//解析xml到json

	FILE *fp;//文件指针，此处用于读取*_vehicles.po，之后用于写入tanks.json
	char line[120];//整行读取时的缓冲区，po文件中每行长度均小于118
	int TempSize;//UTF-8-->Unicode-->ANSI时存储临时字符串长度
	wchar_t *WCPointer;//Unicode字符串指针
	char *MBPointer;//ANSI字符串指针
	int jsonsize = out.size();//获取json数组长度，用于循环读取
	counter = 0;//文件计数器清零
	int tankcounter = 0;//坦克计数器，用于检测是否有loacl_name键缺失

	bWorking = list_file.FindFile(L"*.po");
	while (bWorking)
	{
		bWorking = list_file.FindNextFile();
		fp = _wfopen(list_file.GetFileName().GetBuffer(), L"r");
		if (fp == NULL)
		{
			wcout << endl
				<< L"po文件打开失败！" << endl
				<< L"当前文件：" << list_file.GetFilePath().GetBuffer() << endl
				<< "错误代码为 " << errno << endl;
			system("pause");
			return -7;
		}
		//打开po文件，失败返回-7

		wcout << endl 
			<< L"当前文件：" << list_file.GetFileName().GetBuffer() << endl 
			<< endl;
		fgets(line, 120, fp);//读取第一行

		while (!feof(fp))
		{
			if (!(line[0] == 'm'&&line[1] == 's'&&line[2] == 'g'&&line[3] == 'i'&&line[4] == 'd'))
			{
				fgets(line, 120, fp);//读取下一行
				continue;
			}
			//判断是否是msgid开头

			strcpy(line, line + 7);
			line[strlen(line) - 2] = 0;
			//截取获得引号内内容

			for (int i = 0; i < jsonsize; i++)
			{
				if (!strcmp(out[i]["title"].asCString(), line))
				{
					fgets(line, 120, fp);//读取下一行msgstr

					strcpy(line, line + 8);
					line[strlen(line) - 2] = 0;
					//截取获得引号内内容

					for (unsigned int i = 0; i < strlen(line) - 1; i++)
						if (line[i] == '\\'&&line[i + 1] == '\"')
							strcpy(line + i, line + i + 1);
					//将\"转换为"

					TempSize = MultiByteToWideChar(CP_UTF8, 0, line, -1, NULL, 0);
					WCPointer = new wchar_t[TempSize + 1];
					MultiByteToWideChar(CP_UTF8, 0, line, -1, WCPointer, TempSize);
					TempSize = WideCharToMultiByte(CP_ACP, 0, WCPointer, -1, NULL, 0, NULL, NULL);
					MBPointer = new char[TempSize];
					WideCharToMultiByte(CP_ACP, 0, WCPointer, -1, MBPointer, TempSize, NULL, NULL);
					//UTF-8-->Unicode-->ANSI

					out[i]["loacl_name"] = MBPointer;
					printf("为 %-30s 添加loacl_name键：%s\n", out[i]["icon_orig"].asCString(), MBPointer);

					delete[] WCPointer;
					delete[] MBPointer;
					tankcounter++;//坦克计数器+1
					break;
				}
			}
			fgets(line, 120, fp);//读取下一行
		}
		fclose(fp);
		counter++;//文件计数器+1
	}
	list_file.Close();
	cout << endl 
		<< "po解析过程结束，共解析" << counter << "个po文件" << endl 
		<< "请确保所有_vehicels.po已经被正确解析！是否继续？如否，请右上角" << endl;
	system("pause");
	//解析po文件到json

	cout << endl;
	counter = 0;
	char currenttitle[40];
	for (int i = 0; i < jsonsize; i++)
	{
		if (out[i]["icon_orig"].asString().find("_training") != string::npos)
		{
			strcpy(currenttitle, out[i]["icon_orig"].asCString());
			currenttitle[strlen(currenttitle) - 9] = 0;
			for (int j = 0; j < jsonsize; j++)
				if (!strcmp(out[j]["icon_orig"].asCString(), currenttitle))
				{
					out[i]["loacl_name"] = out[j]["loacl_name"];
					printf("为 %-30s 添加loacl_name键：%s\n", out[i]["icon_orig"].asCString(), out[i]["loacl_name"].asCString());
					counter++;
					tankcounter++;
					break;
				}
		}
		//修复新手教学车loacl_name

		else if (!strcmp(out[i]["icon_orig"].asCString(), "Sexton_I"))
		{
			out[i]["loacl_name"] = "\"司事\"I";
			printf("为 %-30s 添加loacl_name键：%s\n", out[i]["icon_orig"].asCString(), out[i]["loacl_name"].asCString());
			counter++;
			tankcounter++;
		}
		//某不明BUG

		else if (!strcmp(out[i]["icon_orig"].asCString(), "Env_Artillery"))
		{
			out[i]["loacl_name"] = "国服炸逼";
			printf("为 %-30s 添加loacl_name键：%s\n", out[i]["icon_orig"].asCString(), out[i]["loacl_name"].asCString());
			counter++;
			tankcounter++;
		}
		//直译环境火炮，但是呢。。。不服打我啊！
	}
	cout << endl 
		<< "修复缺失的local_name键完成，共修复" << counter << "个条目" << endl;
	//修复loacl_name

	if (out.size() != tankcounter)
	{
		cout << endl 
			<< "检测到潜在的数据缺失！总共生成" << out.size() << "个坦克对象" << endl 
			<< "为其中的" << tankcounter << "个条目添加了loacl_name键，缺失" << out.size() - tankcounter << "个！" << endl 
			<< "是否缺少了po文件？请在程序结束后手动检查tanks.json！" << endl;
		system("pause");
	}
	//检测是否所有坦克对象都添加了loacl_name键

	Json::FastWriter jsonwriter;
	fp = fopen("tanks.json", "w");
	if (fp == NULL)
	{
		cout << endl 
			<< "创建tanks.json失败，是否缺少管理员权限？或者文件正在使用？" << endl;
		system("pause");
		return -8;
	}
	fprintf(fp, jsonwriter.write(out).c_str());
	fclose(fp);
	GetCurrentDirectory(101, path);
	wcout << endl 
		<< L"导出到" << path << L"tanks.json成功！" << endl 
		<< L"如要人工编辑，可以通过替换 }, 为 },^r^n 来增加可读性" << endl;
	cout << endl 
		<< "如要保留反编译的po，请现在右上角！不保留则";
	system("pause");
	//输出到tanks.json

	cout << endl;
	bWorking = list_file.FindFile(L"*.po");
	while (bWorking)
	{
		bWorking = list_file.FindNextFile();
		if (_wremove(list_file.GetFileName().GetBuffer()))
			wcout << list_file.GetFileName().GetBuffer() << "失败，错误代码为 " << errno << endl;
		else
			wcout << L"已删除" << list_file.GetFileName().GetBuffer() << endl;
	}
	list_file.Close();
	//删除临时文件

	cout << endl 
		<< "完成！ztyzbb于2016.9.6敬上！" << endl;
	system("pause");
	return 0;
}