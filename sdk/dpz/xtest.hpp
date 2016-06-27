#ifndef __DPZ__XTEST__HPP__ 
#define __DPZ__XTEST__HPP__

#include "xstring.hpp"
#include "xstream.hpp"
#include "xfile.hpp"

#ifdef WINCE
#define DEF_REPORT_FILE "/nand/report.html";
#else
#define DEF_REPORT_FILE "c:/report.html";
#endif

class Failure
{

public:
	Failure (const dpz::xstring& theTestName, 
		const dpz::xstring&	theFileName, 
		long theLineNumber,
		const dpz::xstring&	theCondition,
		const dpz::xstring& theInfo="无说明")
		: message(theCondition), 
		testName(theTestName), 
		fileName(theFileName), 
		lineNumber(theLineNumber),
		info(theInfo)
	{
	}

	/*Failure (const dpz::xstring& theTestName, 
		const dpz::xstring&	theFileName, 
		long theLineNumber,
		const dpz::xstring&	expected,
		const dpz::xstring&	actual)
		: testName(theTestName), 
		fileName(theFileName), 
		lineNumber(theLineNumber)
	{
		char *part1 = "expected ";
		char *part3 = " but was: ";

		char *stage = new char [strlen (part1) 
			+ expected.size () 
			+ strlen (part3)
			+ actual.size ()
			+ 1];

		sprintf(stage, "%s%s%s%s", 
			part1, 
			expected.c_str(), 
			part3, 
			actual.c_str());

		message = dpz::xstring(stage);

		delete stage;
	}*/

	dpz::xstring		info;
	dpz::xstring		message;
	dpz::xstring		testName;
	dpz::xstring		fileName;
	long				lineNumber;
};
class TestResult
{
public:
	TestResult():failureCount(0)
	{
		path=DEF_REPORT_FILE;
	}
	TestResult(const char* filename,const char* testname,const char* author):failureCount(0)
	{
		path=filename;
		this->testname=testname;
		this->author=author;
	}
	virtual void testsStarted(){
		char buf1[4000];
		DWORD dwSize=0;
		OutputDebugString(L"TestStarted!!!!");

		/*xstream tmp("testhead.tpl");
		tmp.copy(fname.c_str());*/
		copyfile(path,"/nand/testhead.tpl",0);
		xs.bind(path);
		dwSize=sprintf(buf1,"<div id=\"layout\" class=\"pure-g\"><div class=\"sidebar pure-u-1 pure-u-md-1-4\"><div class=\"header\"><h1 class=\"brand-title\">%s</h1><h2 class=\"brand-tagline\">测试：%s</h2><nav class=\"nav\"><ul class=\"nav-list\"><li class=\"nav-item\"><a class=\"pure-button\" href=\"http://www.hangsheng.com.cn\">HASE 主页</a></li><li class=\"nav-item\"><a class=\"pure-button\" href=\"http://www.hangsheng.com.cn\">HASE 主页</a></li></ul></nav></div></div>",this->testname,this->author);
		xs.append(buf1,dwSize);
		strcpy(buf1,"<div class=\"content pure-u-1 pure-u-md-3-4\">");
		xs.append(buf1,strlen(buf1));
		
	}

	virtual void addTestInfo(char* str){
		//char buf[200];
		char* torken=NULL,*ptr=str;
		xs.append("<div>",5);
		while (ptr)
		{
			torken=strchr(ptr,' ');
			if (torken)
			{
				xs.append(ptr,torken-ptr);
				xs.append("<br>",4);
				ptr=torken+1;
			}else{
				xs.append(ptr,strlen(ptr));
				ptr=NULL;
			}
		}
		xs.append("</div>",6);
	}
	virtual void addTestStep(char* str){
		//char buf[200];
		xs.append("<div>",5);
		xs.append(str,strlen(str));
		xs.append("</div>",6);
	}
	virtual void addInfo(const Failure& failure){
		//char buf[200];
		int len=0;
		if(newtable){
			strcpy(buf,"<table class=\"pure-table pure-table-bordered\"><caption>【自动单元测试结果】</caption><thead><tr><th>测试说明</th><th>检查语句</th><th>测试位置</th><th>测试结果</th></thead>");
			xs.append(buf,strlen(buf));
			newtable=0;
		}
		len=sprintf (buf, "<tr bgcolor=\"#00ff00\"><td>%s</td><td>%s</td><td>%s:%ld</td><td>%s</td></tr>\r\n",
			failure.info.c_str(),
			failure.message.c_str (),
			failure.fileName.c_str (),
			failure.lineNumber,
			"info");
		xs.append(buf,len);
		failureCount++;
	}
	virtual void addError(const Failure& failure){
		//char buf[200];
		int len=0;
		if(newtable){
			strcpy(buf,"<table class=\"pure-table pure-table-bordered\"><caption>【自动单元测试结果】</caption><thead><tr><th>测试说明</th><th>检查语句</th><th>测试位置</th><th>测试结果</th></thead>");
			xs.append(buf,strlen(buf));
			newtable=0;
		}
		len=sprintf (buf, "<tr bgcolor=\"#ff0000\"><td>%s</td><td>%s</td><td>%s:%ld</td><td>%s</td></tr>\r\n",
			failure.info.c_str(),
			failure.message.c_str (),
			failure.fileName.c_str (),
			failure.lineNumber,
			"error");
		xs.append(buf,len);
		failureCount++;
	}
	virtual void addFailure(const Failure& failure){
		//char buf[200];
		int len=0;
		if(newtable){
			strcpy(buf,"<table class=\"pure-table pure-table-bordered\"><caption>【自动单元测试结果】</caption><thead><tr><th>测试说明</th><th>检查语句</th><th>测试位置</th><th>测试结果</th></thead>");
			xs.append(buf,strlen(buf));
			newtable=0;
		}
		len=sprintf (buf, "<tr bgcolor=\"#ff0000\"><td>%s</td><td>%s</td><td>%s:%ld</td><td>%s</td></tr>\r\n",
			failure.info.c_str(),
			failure.message.c_str (),
			failure.fileName.c_str (),
			failure.lineNumber,
			"failed");
		xs.append(buf,len);
		failureCount++;
	}
	
	virtual void addSuccess(const Failure& failure){
		//char buf[200];
		int len=0;
		if(newtable){
			strcpy(buf,"<table class=\"pure-table pure-table-bordered\"><caption>【自动单元测试结果】</caption><thead><tr><th>测试说明</th><th>检查语句</th><th>测试位置</th><th>测试结果</th></thead>");
			xs.append(buf,strlen(buf));
			newtable=0;
		}
		len=sprintf (buf, "<tr bgcolor=\"#00ff00\"><td>%s</td><td>%s</td><td>%s:%ld</td><td>%s</td></tr>\r\n",
			failure.info.c_str(),
			failure.message.c_str (),
			failure.fileName.c_str (),
			failure.lineNumber,
			"passed");
		xs.append(buf,len);
		failureCount++;
	}
	virtual void prerun(){
		newtable=1;
		strcpy(buf,"<div class=\"posts\"><h1 class=\"content-subhead\">单元测试</h1>");
		xs.append(buf,strlen(buf));
	}
	virtual void postrun(){
		//char buf[200];
		if(!newtable){
			strcpy(buf,"</table></div>");
			xs.append(buf,strlen(buf));
		}
	}
	virtual void testsEnded (){
		////char buf[200];
		int len;
		if (failureCount > 0)
			len=sprintf (buf, "总共进行了 %ld 次测试或记录<br>", failureCount);
		else
			len=sprintf (buf, "没有测试与记录，请检查测试代码<br>");
		xs.write(buf,len);
		len=sprintf(buf,"</div></body></html>");
		xs.append(buf,len);
		OutputDebugString(L"TestStoped!!!!");
	}
private:
	const char *path;
	const char *testname;
	const char *author;
	dpz::xstream xs;
	int	failureCount;
	int newtable;
	char buf[1024];
};

template<typename T>
class TestRegistry
{
public:
	static void addTest(T *test){
		instance().add(test);
	}
	static void runAllTests(TestResult& result){
		instance().run(result);
	}

private:

	static TestRegistry& instance(){
		static TestRegistry registry;
		return registry;
	}
	void add(T *test){
		if (tests == 0) {
			tests = test;
			return;
		}

		test->setNext (tests);
		tests = test;
	}
	void run(TestResult& result){
		result.testsStarted ();

		for (Test *test = tests; test != 0; test = test->getNext ()){
			result.prerun();
			test->run (result);
			result.postrun();
		}
		result.testsEnded ();
	}
	T *tests;

};
class Test
{
public:
	Test(const dpz::xstring& testName):name_(testName) 
	{
		TestRegistry<Test>::addTest (this);
	}
	virtual void run(TestResult& result) = 0;
	void setNext(Test *test){	
		next_ = test;
	}
	Test *getNext()const
	{
		return next_;
	}
protected:
	bool check(long expected, long actual, TestResult& result, const dpz::xstring& fileName, long lineNumber){
		if (expected == actual){
			result.addSuccess(Failure(
				name_, 
				dpz::xstring(__FILE__), 
				__LINE__, 
				dpz::xstring(expected), 
				dpz::xstring(actual)));
			return true;
		}else{
			result.addFailure (
				Failure(
				name_, 
				dpz::xstring(__FILE__), 
				__LINE__, 
				dpz::xstring(expected), 
				dpz::xstring(actual)));
			return false;
		}
	}
	bool check(const dpz::xstring& expected, const dpz::xstring& actual, TestResult& result, const dpz::xstring& fileName, long lineNumber){
		if(expected == actual){
			result.addSuccess(
				Failure(
				name_, 
				dpz::xstring(__FILE__), 
				__LINE__, 
				expected, 
				actual));
			return true;
		}else{
			result.addFailure(
			Failure(
			name_, 
			dpz::xstring(__FILE__), 
			__LINE__, 
			expected, 
			actual));
			return false;
		}
	}
	dpz::xstring	name_;
	Test			*next_;

};


#define TEST(testName, testGroup)\
class testGroup##testName##Test : public Test \
{ public: testGroup##testName##Test () : Test (#testName "Test") {} \
	void run (TestResult& result_); } \
	testGroup##testName##Instance; \
	void testGroup##testName##Test::run (TestResult& result_) 


#define CHECKTIPS(condition,tips)\
{ if ((condition)) \
{result_.addSuccess (Failure (name_, __FILE__,__LINE__, #condition,tips));}\
else{ result_.addFailure (Failure (name_, __FILE__,__LINE__, #condition,tips)); /*return;*/ } }

#define CHECK(condition)\
{ if ((condition)) \
{result_.addSuccess (Failure (name_, __FILE__,__LINE__, #condition,"没有测试说明"));}\
else{ result_.addFailure (Failure (name_, __FILE__,__LINE__, #condition,"没有测试说明")); /*return;*/ } }

#define INFO(info,tips)\
{result_.addInfo (Failure (name_, __FILE__,__LINE__,info,tips));}\

#define ERROR(info,tips)\
{result_.addError (Failure (name_, __FILE__,__LINE__,info,tips));}\

//#define CHECK_EQUAL(expected,actual)\
//{ if ((expected) != (actual)) { result_.addFailure(Failure(name_, __FILE__, __LINE__, dpz::xstring(expected), dpz::xstring(actual))); }}
//
//
//#define LONGS_EQUAL(expected,actual)\
//{ long actualTemp = actual; \
//	long expectedTemp = expected; \
//	if ((expectedTemp) != (actualTemp)) \
//{ result_.addFailure (Failure (name_, __FILE__, __LINE__, dpz::xstring(expectedTemp), \
//	dpz::xstring(actualTemp))); /*return;*/ } }
//
//
//
//#define DOUBLES_EQUAL(expected,actual,threshold)\
//{ double actualTemp = actual; \
//	double expectedTemp = expected; \
//	if (fabs ((expectedTemp)-(actualTemp)) > threshold) \
//{ result_.addFailure (Failure (name_, __FILE__, __LINE__, \
//	dpz::xstring((double)expectedTemp), dpz::xstring((double)actualTemp))); /*return;*/ } }
//
//
//
//#define FAIL(text) \
//{ result_.addFailure (Failure (name_, __FILE__, __LINE__,(text))); /*return;*/ }


#define TEST_INFO(text) {result_.addTestInfo(#text);}
#define TEST_STEP(text) {result_.addTestStep(#text);text}

typedef TestRegistry<Test> TestKit;


class TestAuto{
public:
	static DWORD TestAutoWork(LPVOID lp){
		/*wchar_t str[200];
		wsprintf(str,L"等%s待秒后执行单元测试\r\n",(int)lp);*/
		TestAuto* pta=(TestAuto*)lp;
		OutputDebugString(L"等%s待秒后执行单元测试\r\n");
		Sleep(pta->sleeptime);
		TestKit test;
		TestResult result(pta->path,pta->testname,pta->author);
		test.runAllTests(result);
		return 0;
	}
	TestAuto(const char *filename,int delay,const char* testname,const char* author){
		sleeptime=delay;
		path=filename;
		this->testname=testname;
		this->author=author;
		CloseHandle(CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)TestAuto::TestAutoWork,(LPVOID)this,NULL,NULL));
	}
private:
	const char *path;
	const char *testname;
	const char *author;
	int sleeptime;
};
#endif