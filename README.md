# Kyte

## 目录

- [介绍](#介绍)
- [安装](#安装)
- [环境](#环境)
- [使用](#使用)
- [TODO](#TODO)

## 介绍

这是作者在学习socket编程时，用于练习tcp的运用时所写，未来会不定期维护，和将TODO完成。

## 安装

如果你使用git则在终端中执行以下代码

```bash
git clone https://github.com/Ge0rgeCai/Kyte.git
```

Github CLI用户可以在终端中执行以下代码

```bash
gh repo clone Ge0rgeCai/Kyte
```

## 环境

作者目前只写了在**window**平台是的server端，与client端，未来也会支持**linux**平台。<br/>

### 编译器/IDE:<br/>

**msvc**<br/>

都用`msvc`直接编译了，多的就不用说明了吧 ：）<br/>

(VS中自带，如果不喜欢IDE，可以自行下载msvc编译器，来将代码进行编译)<br/>

**Visual Studio 2019**<br/>

将`.c`文件都放入VS的`源文件`中，`.h`文件放在VS的`头文件`中VS将代码编译。

## 使用
使用时会默认在服务端当前目录创建`Download_Dir`文件夹，用于存储接收到的文件。

**WINDOWS:**

客户端:	

```bash
./Kyte_Client_Win.exe <IP> <FileName>
```

服务端:	

```bash
./Kyte_Server_Win.exe
```



## TODO

- [x] 大文件传输

- [x] 进度条

- [ ] 服务端验证

- [ ] 多线程接收大文件(提高速度)

- [ ] 实现下载功能

  

## LISENCE

[MIT](https://github.com/Ge0rgeCai/Kyte/blob/master/LICENSE)

  
