# mysql & Redis 连接池

> 1、数据库连接池的原理
>
> 2、连接池连接数量的设置
>
> 3、同步连接和异步处理的优缺点分析



## 数据库连接池运行机制

```
1、从连接池获取或创建可用连接;
2、使用完毕之后，把连接返回给连接池;
3、在系统关闭前，断开所有连接并释放连接占用的系统资源;
4、处理无效连接（原来登记为可用的连接，由于某种原因不可再用，如超时、通讯问题），并能够限制连接池中的连接总数不低于某个预定值和不超过某个预定值。
```

## 连接池的属性

```c
name: 连接池的地址
url： 数据库的地址
username： 登录数据库的用户名
password： 登录数据库的密码
maxidle：  最大的空闲连接数，这里取值为2，表示即使没有数据库连接时，依然保持2空闲的连接，从而不被清除，随时处于待命状态。
maxwait：  最大建立等待时间。如果超过此时间接到异常，设为-1表示无限制。缺省为5000，表示5秒后超时。
maxActive： 最大激活连接数，这里取值为4，表示同时最多有4个数据库连接。
```

## 连接池的大小

```c
连接池的大小需要比较复杂的计算，一般使用计算公式获得连接数之后，根据实际业务场景进行微调。
常用的计算公式：
	连接数 = ((核心数 * 2) + 有效磁盘数)
```


