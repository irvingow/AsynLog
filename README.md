#### 异步日志库

参考muduo的实现,感觉陈硕能分享这么值得学习的项目，自己学习了一下并动手实现了，同时根据自己的理解来做了一些解释，
同时做了一些微不足道的改动

#### 简单介绍

异步日志库往往分为前端和后端，前端就是使用日志库的线程，后端则是单独运行的线程，前端多个线程将数据写入到buffer中，
当buffer满或者flushInterval时间到了，后端线程就从共享的buffers中取走数据，并写入到文件中，思路非常简单明了，
不过仍然有一些需要注意的地方，下面针对文件来做简单的介绍

##### asynLogging.h/cpp

这个是后端的入口，当我们需要将日志写入到文件中去的时候（本日志默认是将日志信息写入到标准输出的）就需要引入这个文件，
threadFunc函数是后端线程运行的主函数，当共享的buffer被写满或者设置的FlushInterval时间到时，线程就会开始工作，
将buffer中的数据写入到文件中去（实际上是封装的文件类，也就是logFile），同时设置空的buffer，这里为了避免占用共享的
buffer太久，使用的技巧是swap和move，这样的好处是避免占用共有的buffer太久，导致前端线程因为锁被占用儿阻塞，影响
吞吐量，除此之外，还有小技巧是重用buffer，简单来说，就是一共申请了四个buffer大小的空间，前端只使用两个buffer，
当两个buffer用完之后，后端将交换空闲的两个buffer和已经写满了的buffer的空间，这样原先已经写满的两个buffer又空了，
两个满的buffer交给后台线程来处理，这个trick就大大减少了处理buffer中数据导致的延时，从理论上来看是个非常好用的技巧，
不过这里需要注意的是因为buffer的大小是4M，而我们一共只申请了4个buffer大小的空间，所以可能出现的问题是当前端线程写
入日志的速度太快，后端不断申请和释放空间，可能导致性能问题，因为可以根据实际需要来修改buffer的大小

##### logFile.h/cpp

这个实际上是对文件的封装（其实最底层的封装是fileUtil，但是fileUtil比较简单，这里我就不多介绍），这个的主要功能是
根据RollSize的大小和时间（因为新的一天需要新的日志文件）来对文件进行更新以及根据FlushInterval或者CheckEveryN来
对flush内存中的数据来写入到磁盘中，实现起来并没有很多的技巧

##### logging.h/cpp和logStream.h/cpp

这两个是前端的入口，当我们使用如下代码：

> LOG_INFO << "hello asynLog";

的时候，实际上生成了一个匿名LogStream对象，这样做的好处是匿名对象会立刻被析构，进而调用LogStream的析构函数，将数据
通过用户给的接口写出去（默认的话日志直接写入到标准输出中），这里稍微值得一提的是PreDefineMacroHelper，这个使用了
模板来编译期实例化的特性，获得了一些内置的宏的长度，比如__func__, __FILE__等，这样的好处就是避免在这些数据写入
LogStream内部buffer的时候还需要计算这些预定义宏的长度，提升了性能

##### num2String.h/cpp

这里我做了一些微小的改动，我参考了miloyip的项目 [itoa-benchmark](https://github.com/miloyip/itoa-benchmark)，
使用了branchlut来将数据转化为字符串,这里再次感谢miloyip
