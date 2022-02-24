#include <iostream>
#include<stdlib.h>
#include <mirai.h>
#include<ctime>
#include<vector>
#include<algorithm>
#include<map>
#include<string>
using namespace std;
using namespace Cyan;

//----------------------------------------------------------------------------------
//全局变量
map<GID_t, bool> group_list;
vector<QQ_t> admin_list;
vector<QQ_t> master_vector;
map<string, string> command_list;
//----------------------------------------------------------------------------------
//工具函数
bool IsValidGroup(const GID_t& group_number) {
	//vector<GID_t>::iterator result = find(valid_group_number_list.begin(), valid_group_number_list.end(),group_number);
	//if (result == valid_group_number_list.end()) {
	//	return false;
	//}
	//else {
	//	return true;
	//}
	return group_list[group_number];
}
bool IsAdmin(const QQ_t& user_qq) {
	vector<QQ_t>::iterator result = find(admin_list.begin(), admin_list.end(), user_qq);
	if (result == admin_list.end()) {
		return false;
	}
	else {
		return true;
	}
}
void GroupRandomSpeak(const GroupMessage& m, const string& first_plain_string) {
	int rand_value = rand();
	if (rand_value % 50 == 1) {
		m.Reply(MessageChain().Plain("傻逼"));
	}
	else if (rand_value % 50 == 2) {
		m.Reply(MessageChain().Plain("嗯嗯嗯好好好哦哦哦"));
	}
	else if (rand_value % 50 == 3 || rand_value % 10 == 4) {
		m.Reply(m.MessageChain);
	}
}
void GroupCommandSystem(const GroupMessage& m,const string& plain_message) {
	if (!IsAdmin(m.Sender.QQ)) {
		//m.Reply(MessageChain().Plain("No Permission."));
		return;
	}
	if (plain_message == "/help") {
		string result;
		for (auto it = command_list.begin(); it != command_list.end(); it++) {
			result += it->first;
			result += "  ";
			result += it->second;
			result += '\n';
		}
		m.Reply(MessageChain().At(m.Sender.QQ).Plain("\ncommand list: \n").Plain(result));
		return;
	}
	if (plain_message == "/validate_this_group") {
		group_list[m.Sender.Group.GID] = true;
		m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nBot On."));
		return;
	}
	if (plain_message == "/invalidate_this_group") {
		group_list[m.Sender.Group.GID] = false;
		m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nBot Off."));
		return;
	}
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nno such command.\n输入/help查看所有指令"));
}
//----------------------------------------------------------------------------------
//应用层函数
void ReceiveGroupMessage(const GroupMessage& m){
	if (m.Sender.QQ == (QQ_t)1575602650 && m.MessageChain.GetPlainText() == "/validate_this_group") {
		m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nBot On."));
		group_list[m.Sender.Group.GID] = true;
		return;
	}
	if (!IsValidGroup(m.Sender.Group.GID))
		return;
	string plain_message = m.MessageChain.GetPlainText();
	if (plain_message[0]=='/') {
		GroupCommandSystem(m, plain_message);
		return;
	}
	GroupRandomSpeak(m, plain_message);
	
	return;
}
void ReceiveFriendMessage(FriendMessage m) {

	return;
}
//----------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	//initialing....
	srand(unsigned(time(NULL)));
	MiraiBot bot;
	SessionOptions opts;
	opts.BotQQ.Set((QQ_t)1209315958);
	opts.VerifyKey = "114514";
	opts.EnableVerify = true;
	opts.SingleMode = false;
	opts.ThreadPoolSize = 4;
	group_list.clear();
	command_list.clear();
	admin_list.clear();
	master_vector.push_back((QQ_t)1575602650);
	admin_list.push_back((QQ_t)1575602650);
	//群列表
	group_list.insert({ (GID_t)749257398,true });//yhk群
	group_list.insert({ (GID_t)180068294,true });//爽哥群
	group_list.insert({ (GID_t)867976597,true });//萝卜鸡


	command_list.insert({ "/help","查看帮助" });
	command_list.insert({ "/add_admin","添加管理员权限(搭建中)" });
	command_list.insert({ "/delete_admin","删除管理员权限(搭建中)" });
	command_list.insert({ "/validate_this_group","启用本群bot" });
	command_list.insert({ "/invalidate_this_group","关闭本群bot" });



	//try connect
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
		MiraiBot::SleepSeconds(10);
	}
	cout << "Bot Working..." << endl;
	//----------------------------------------------------------------------------------
	bot.On<GroupMessage>(ReceiveGroupMessage);
	//bot.On<FriendMessage>(ReceiveFriendMessage);
	
	//----------------------------------------------------------------------------------
	//command
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