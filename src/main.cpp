#include <iostream>
#include<stdlib.h>
#include <mirai.h>
#include<ctime>
using namespace std;
using namespace Cyan;
//----------------------------------------------------------------------------------
void ReceiveMessageAndSend(Message m){
	srand((unsigned)time(NULL));
	if(m.ToString()==(string)"你是谁") {m.Reply(MessageChain().Plain("我是lyf的女朋友"));return;}
	if(m.ToString()==(string)"出来见见大伙吧") {m.Reply(MessageChain().Plain("大家好"));return;}
	int rand_value=rand();
	if(rand_value%50==1){
		m.Reply(MessageChain().Plain("傻逼"));
	}else if(rand_value%50==2){
		m.Reply(MessageChain().Plain("嗯嗯嗯好好好哦哦哦"));
	}else if(rand_value%50==3||rand_value%10==4){
		m.Reply(m.MessageChain);
	}
	return;
}
//----------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	srand(unsigned(time(NULL)));
	MiraiBot bot;
	SessionOptions opts = SessionOptions::FromJsonFile("D:\\mirai_cpp_vs\\MyBot\\session_options.json");
	while (true)
	{
		try
		{
			cout << "尝试与 mirai-api-http 建立连接..." << endl;
			bot.Connect(opts);
			break;
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
		MiraiBot::SleepSeconds(30);
	}
	cout << "Bot Working..." << endl;
	//----------------------------------------------------------------------------------
	bot.On<Message>(ReceiveMessageAndSend);
	
	//----------------------------------------------------------------------------------
	string command;
	while (cin >> command)
	{
		if (command == "exit")
		{
			bot.Disconnect();
			cout<<"Bot Disconnected..."<<endl;
			break;
		}
	}
	return 0;
}