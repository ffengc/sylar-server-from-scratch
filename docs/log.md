# 日志系统

- [日志系统](#日志系统)
  - [整体结构](#整体结构)
  - [关于 `Formatter`](#关于-formatter)


## 整体结构

```cpp
namespace ffengc_sylar
{
    class LogEvent{};
    /**
     * @brief define the log level
     */
    class LogLevel
    {
    public:
        enum Level // define the level of the logs
        {
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
    };

    /**
     * @brief format the logs
     */
    class LogFormatter {};

    /**
     * @brief control where the logs output
     */
    class LogAppender {};

    /**
     * @brief define the logger
     */
    class Logger {};
    // define more useful appenders
    class stdoutLogAppender : public LogAppender {};
    class fileLogAppender : public LogAppender {};
} // namespace ffengc_sylar

```

定义这么一些类：
- 首先是 `LogEvent` 表示一个日志事件，比如想输出一个日志，我就会生成一个这样的事件。一个 `LogEvent` 里面可能有一系列的信息，比如文件名，mesg，threadid，fiberid等等
- 有了一个事件之后，我们就可以交给一个日志器去领取这个日志。
  - 为什么要有不同的日志器呢：比如，我在底层系统有一个`Logger`, 应用层也有一个`Logger`，不同的地方，肯定是要调用不同的日志器去管理的。
- 日志应该怎么输出呢，有没有可能一条日志，可能需要输出到多个地方？所以需要 `LogAppender` 去控制输出到哪了，`Logger` 里面会有一个 `LogAppender` 的列表，这样就可以控制一条日志输出到一个或多个地方。
  - 比如 `stdoutLogAppender`，是控制把日志输出到控制台的
- `LogFormatter` 就很好理解了，因为 `LogEvent` 对象也是通过指针层层传递的（看看代码就知道了），通过把 `LogEvent` 结构体里面的字段，给他整理成字符串，弄一个好看的形式。

## 关于 `Formatter`

```cpp
class LogFormatter
{
private:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual std::string format(LogEvent::ptr event) = 0;
    };
    // def some derive class
private:
    std::string __m_pattern;
    std::vector<FormatItem::ptr> __m_items;
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    std::string format(LogEvent::ptr event);
    void init();
};
```

我们是想，具体日志怎么输出，是可以通过外面决定的，这个很好理解。

比如构造 `LogFormatter` 的时候，指定 `%s%t`, 我们就可以知道，等下输出日志的时候，是 `[hello world][11:20]` 这样输出的（这就个简单的例子而已）

然后其他结构（`Appender`）通过调用 `format` 函数，传入 `event`, 就可以得到我们规定格式的字符串。

`init` 函数就是去解析这个 `%s%t`, 不然程序怎么知道我要什么格式。