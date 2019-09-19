# SMuduoSever

参照muduo网络库和github上其他一些开源大佬(也是muduo思想)，最后自己动手实现了这个smuduoSever。

**使用muduo思想，但是比muduo简洁明了**。没有muduo的那些base库,全部使用c++11的东西。



## 技术点

- 采用基于epoll的IO多路复用机制，+非阻塞IO+边缘触发  基于事件驱动Reactor模式

- 由于采用ET模式，read、write和accept的时候必须采用循环的方式，直到error==EAGAIN为止，防止漏读等清况，这样的效率会比LT模式高很多，减少了触发次数

- 线程模型划分为主线程、IO线程和worker线程，主线程用于接收客户端连接，并通过Round-Roubin策略分发给IO线程，IO线程负责连接管理（即事件监听和读写操作）,worker线程负责业务计算任务（即对数据进行处理，应用层复杂时候可以开启）

- 采用了c++11当中智能指针管理多线程下的对象资源

- 采用c++11 std::lock_guard 及std::unique_lock RAII机制管理std::mutex资源，在离开作用域时锁自动释放

- 支持优雅关系连接

  - 通常情况下，由客户端主动发起FIN关闭连接

  - 客户端发送FIN关闭连接后，服务器把数据发完才close，而不是直接暴力close

  - 如果连接出错，则服务器可以直接close

    

## 亮点

使用TCPServer网络库，用户在需要自己编写一个TCPServer时，只需要写其业务逻辑，TCPServer底层自动开启主线程接收客户端连接，IO线程进行连接管理，工作线程处理业务计算任务，用户一般来说只需要设置其连接回调，新消息回调，写完成回调，关闭回调和错误回调等业务逻辑回调函数，TCPSever根据编写业务逻辑进行处理就OK了。