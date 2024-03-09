验证 wlnet 百万并发服务器

启动程序：
1、./client_TCP ip port
2、./server_TCP port 100

测试情况：
1、由于环境限制，主要是硬件环境太拉跨，只能使用有限的client虚拟机，连接server虚拟机。
2、修改 server-test.cc 代码，监听多个 port、和 ip.
3、2024-03-08 第一次测试：
    only one 环境，客户端和服务端在同一个虚拟机中.
    服务器建立大约20万tcp连接后到达当前虚拟机环境限制.
    待更新虚拟机环境后再次测试.
4、2024-03-09 第二次测试：
    换了家里的电脑环境，建立6万多链接之后，客户端报错：socket: Too many open files in system.
    当前环境的系统打开的文件描述符数超过了操作系统的限制。
    查看当前系统文件描述符上限：ulimit -a
5、2024-03-09 第三次测试：
    修改系统配置，由于只有两台环境需要考虑修改五元组数据，使环境能够建立最大连接数量
    ulimit -n 1024587       #临时修改的办法，可以修改系统文件进行修改
    使用 sudo vim /etc/security/limits.conf 进行永久修改
    sudo vim /etc/sysctl.conf   
        client:
            net.ipv4.tcp_mem = 262144 524288 786432
            net.ipv4.tcp_wmem = 1024 1024 2048
            net.ipv4.tcp_rmem = 1024 1024 2048
            fs.file-max = 1048576
            net.ipv4.tcp_max_orphans = 16384

        server:
            net.ipv4.tcp_mem = 252144 524288 786432
            net.ipv4.tcp_wmem = 2048 2048 4096
            net.ipv4.tcp_rmem = 2048 2048 4096
            fs.file-max = 1048576
            net.nf_conntrack_max = 1048576
            net.netfilter.nf_conntrack_tcp_timeout_established = 1200

如截图 wlnet03.PNG 所示，建立了 80 多万个TCP链接后，服务器环境重启，勉强算是成功吧~

总结：
1、使用reactor框架，epoll的IO多路复用进行编码，是可以支持服务器百万并发的。
2、系统环境默认的配置不足以支持百万TCP链接，需要进行调优，然后具体的调优数据已经写出来了，后续再更新为什么这样做（今天太晚了~）
3、后续优化方向：
    日志打印出来了建立一千个TCP链接的耗时，链接时间的优化
    htop观察 CPU占用很高
    更新测试的 client 代码设计一个简单的通信协议，每隔一段时间互相回数据，测试当前相同的环境的并发量能达到多少。

update：
1、使用vecode远程连接进行测试，每次都是到81万左右进程挂掉，给服务器加了资源之后还是不行，但是通过htop监测CPU、内存等数据还没到顶点，在虚拟机环境中直接启动服务器程序，终于到达百万并发.如图：wlnet03.PNG
