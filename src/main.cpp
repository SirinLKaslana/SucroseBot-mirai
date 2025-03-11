#include<iostream>
#include<stdlib.h>
#include<mirai.h>
#include<ctime>
#include<vector>
#include<algorithm>
#include<map>
#include<string>
#include<fstream>
#include<sstream>
#include<main.h>
#include<time.h>
#include<random_speak.hpp>

using namespace std;
using namespace Cyan;
//----------------------------------------------------------------------------------
//全局变量
RandomSpeakController random_speak_controller;
map<GID_t, bool> group_list;//在main函数里初始化
vector<QQ_t> admin_list;//从文件里读
map<string, string> command_list;//在main函数里初始化
json admin_list_json;
int prev_day = 1;
int prev_hour = 1;
int today = 1;
int hour_now = 1;
time_t time_real{};
vector<QQ_t> black_list;
vector<string> dirty_words = {};
//----------------------------------------------------------------------------------
//底层工具函数

void GetRealTime() {
	time(&time_real);
	time_real += 8 * 3600;
	tm* t = gmtime(&time_real);
	today = t->tm_mday;
	hour_now = t->tm_hour;
}

//黑名单，每个小时的初清一次
bool IsInBlackList(QQ_t qq_num) {
	for (auto& it : black_list) {
		if (it == qq_num)return 1;
	}
	return 0;
}
void AddBlackList(QQ_t qq_num) {
	black_list.push_back(qq_num);
	return;
}
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
bool IsMaster(const QQ_t& user_qq) {
	return user_qq == (QQ_t);
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
bool MessageIsCommand(const string& text) {
	return text[0] == '/';
}
bool MessageIsFunction(const string& text) {
	return (text[0] == '.' && text[1] == '/');
}
vector<string> SplitText(const string& input_text) {
	vector<string> rst;
	istringstream ss(input_text);
	string buff;
	buff.clear();
	while (ss >> buff) {
		rst.push_back(buff);
	}
	return rst;
}
//bool HasBeenInAdminJsonFile(QQ_t new_admin, json& j) {////????????????????????????????????????????
//	for (auto& it : j["admin_list"]) {
//		if (it== to_string((int64_t)new_admin)) {
//			return true;
//		}
//	}
//	return false;
//}
void UpdateFile2Json(json& json_in_ram, const string& json_file_location_string) {
	ifstream json_file(json_file_location_string);
	json_file >> json_in_ram;
	json_file.close();
}

void UpdateJson2File(json& json_in_ram, const string& json_file_location_string) {
	ofstream json_file(json_file_location_string, ios::trunc);
	json_file << json_in_ram;
	json_file.close();
	json_in_ram.clear();
	return;
}
void UpdateAdminJson2Vector() {
	for (auto& it : admin_list_json["admin_list"]) {
		admin_list.push_back(QQ_t(stoi((string)it)));
	}
	admin_list_json.clear();
}
void UpdateAdminVector2Json() {
	admin_list_json["admin_list"].clear();
	for (auto it : admin_list) {
		admin_list_json["admin_list"].push_back(to_string((int64_t)it));
	}
	return;
}
bool MessageHasDirtyWords(const vector<string>& text_vector) {
	for (auto it_in_message : text_vector) {
		for (auto it_in_dirty_words : dirty_words) {
			if (it_in_message.find(it_in_dirty_words) != string::npos) {
				return true;
			}
		}
	}
	return false;
}
void TryUpdateChatRecordFileAndBlackList() {
	if (hour_now != prev_hour) {
		black_list.clear();
	}
	if (today != prev_day) {
		prev_day = today;
		random_speak_controller.TryDeleteChatRecord(
			random_speak_controller.chat_record_1_vector_,
			random_speak_controller.chat_record_1_json_,
			chat_record_1_json_file_path
		);
		random_speak_controller.UpdateVector2Json(random_speak_controller.chat_record_1_json_, random_speak_controller.chat_record_1_vector_);
		random_speak_controller.UpdateJson2File(chat_record_1_json_file_path, random_speak_controller.chat_record_1_json_);

		random_speak_controller.TryDeleteChatRecord(
			random_speak_controller.chat_record_2_vector_,
			random_speak_controller.chat_record_2_json_,
			chat_record_2_json_file_path
		);
		random_speak_controller.UpdateVector2Json(random_speak_controller.chat_record_2_json_, random_speak_controller.chat_record_2_vector_);
		random_speak_controller.UpdateJson2File(chat_record_2_json_file_path, random_speak_controller.chat_record_2_json_);

		random_speak_controller.TryDeleteChatRecord(
			random_speak_controller.chat_record_2_vector_,
			random_speak_controller.chat_record_2_json_,
			chat_record_2_json_file_path
		);
		random_speak_controller.UpdateVector2Json(random_speak_controller.chat_record_3_json_, random_speak_controller.chat_record_3_vector_);
		random_speak_controller.UpdateJson2File(chat_record_3_json_file_path, random_speak_controller.chat_record_3_json_);
	}
}

//----------------------------------------------------------------------------------
//指令函数
void InValidateThisGroup(const GroupMessage& m) {
	group_list[m.Sender.Group.GID] = false;
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nBot Off."));
}

void ValidateThisGroup(const GroupMessage& m) {
	group_list[m.Sender.Group.GID] = true;
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nBot On."));
}

void AddAdmin(const GroupMessage& m, QQ_t new_admin) {//判断输入QQ合法性，搁置，但存在安全隐患

	if (IsAdmin(new_admin)) {
		m.Reply(MessageChain().At(m.Sender.QQ).Plain("\n这位已经是admin了."));
		return;
	}
	admin_list.push_back(new_admin);
	UpdateAdminVector2Json();
	UpdateJson2File(admin_list_json, admin_list_json_file_path);
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\n新的admin,ID:\n").Plain(to_string((int64_t)new_admin)));
	return;
}

void DeleteAdmin(const GroupMessage& m, QQ_t delete_admin) {
	if (!IsAdmin(delete_admin)) {
		m.Reply(MessageChain().At(m.Sender.QQ).Plain("\n这位不是admin哦."));
		return;
	}
	auto it = admin_list.begin();
	for (; it != admin_list.end(); it++) {
		if (*it == delete_admin) break;
	}
	admin_list.erase(it);
	UpdateAdminVector2Json();
	UpdateJson2File(admin_list_json, admin_list_json_file_path);
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nadmin已经删除,ID:\n").Plain(to_string((int64_t)delete_admin)));
	return;
}

void ListAdmins(const GroupMessage& m) {
	MessageChain rst;
	rst.At(m.Sender.QQ).Plain("\n目前所有的管理员:\n");
	for (auto it : admin_list) {
		rst.Add(PlainMessage(to_string((int64_t)it)));
		rst.Plain('\n');
	}
	m.Reply(rst);
	return;
}

void GetHelp(const GroupMessage& m) {
	string result;
	for (auto it = command_list.begin(); it != command_list.end(); it++) {
		result += it->first;
		result += "  ";
		result += it->second;
		result += '\n';
	}
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\ncommand list: \n").Plain(result));
}


//----------------------------------------------------------------------------------
//功能函数
bool GroupAntiAbuseAndDirtyWords(const GroupMessage& m, const vector<string>& text_vector) {
	if (MessageHasDirtyWords(text_vector)) {
		if (m.AtMe()) {
			AddBlackList(m.Sender.QQ);
			if (m.Sender.Group.GID == (GID_t)749257398) {
				m.Reply(MessageChain().At(m.Sender.QQ).Plain("\n."));
				return 1;
			}
			else {
				m.Reply(MessageChain().At(m.Sender.QQ).Plain("\n不要骂我,我只是个机器人,呜呜..."));
				return 1;
			}
		}
		return 1;
	}
	return 0;
}

void GroupSaveTextInVector(const GroupMessage& m, const vector<string>& text_vector) {
	//1群
	if (m.Sender.Group.GID == (GID_t)749257398) {
		random_speak_controller.WriteInNewMessage(random_speak_controller.chat_record_1_vector_, text_vector[0]);

		
		
	}
	//2群
	if (m.Sender.Group.GID == (GID_t)180068294) {
		random_speak_controller.WriteInNewMessage(random_speak_controller.chat_record_2_vector_,text_vector[0]);
	}
	//3群
	if (m.Sender.Group.GID == (GID_t)867976597) {
		random_speak_controller.WriteInNewMessage(random_speak_controller.chat_record_3_vector_,text_vector[0]);
	}
}

void GroupRandomSpeak(const GroupMessage& m, const vector<string>& text_vector) {
	int rand_value = rand();
	int which_group = 0;
	if (m.Sender.Group.GID == (GID_t)749257398) {
		which_group = 1;
	}
	else if(m.Sender.Group.GID == (GID_t)180068294){
		which_group = 2;
	}
	else if (m.Sender.Group.GID == (GID_t)867976597) {
		which_group = 3;
	}
	if (which_group == 0)return;

	if (rand_value % 100 == 1) {
		m.Reply(MessageChain().Plain("嗯嗯"));
	}
	else if (rand_value % 100 == 2) {
		m.Reply(m.MessageChain);
	}
	else if (rand_value % 100 == 99|| rand_value % 100 == 98) {
		if (which_group == 1) {
			m.Reply(MessageChain().Plain(random_speak_controller.GetAMessage(random_speak_controller.chat_record_1_vector_,rand_value)));
		}
		else if (which_group == 2) {
			m.Reply(MessageChain().Plain(random_speak_controller.GetAMessage(random_speak_controller.chat_record_2_vector_, rand_value)));
		}
		else if (which_group == 3) {
			m.Reply(MessageChain().Plain(random_speak_controller.GetAMessage(random_speak_controller.chat_record_3_vector_, rand_value)));
		}
	}
	return;
}

bool ReplyToMasterAt(const GroupMessage& m) {
	if (m.AtMe()) {
		m.Reply(MessageChain().At(m.Sender.QQ).Plain("\n主人，我在."));
		return 1;
	}
	return 0;
}
void GroupMasterCommandSystem(const GroupMessage& m, const vector<string>& text_vector) {
	string first_command = text_vector[0];

	//以下是所有的命令
	if (first_command == "/help") {
		GetHelp(m);
		return;
	}

	if (first_command == "/invalidate_this_group") {
		InValidateThisGroup(m);
		return;
	}
	if (first_command == "/add_admin") {
		if (text_vector.size() == 1) {
			m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nNo Valid QQ."));
		}
		else {
			AddAdmin(m, QQ_t(stoi(text_vector[1])));
		}
		return;
	}

	if (first_command == "/delete_admin") {
		if (text_vector.size() == 1) {
			m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nNo Valid QQ."));
		}
		else {
			DeleteAdmin(m, QQ_t(stoi(text_vector[1])));
		}
		return;
	}

	if (first_command == "/list_admins") {
		ListAdmins(m);
		return;
	}
	//此功能在总开关前实现
	//if (plain_message == "/validate_this_group") {
	//	group_list[m.Sender.Group.GID] = true;
	//	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nBot On."));
	//	return;
	//}

	//如果没有匹配的命令
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nno such MasterCommand.\n输入/help查看所有命令"));
}

void GroupAdminCommandSystem(const GroupMessage& m, const vector<string>& text_vector) {
	string first_command = text_vector[0];

	//以下是所有的命令
	if (first_command == "/help") {
		GetHelp(m);
		return;
	}

	if (first_command == "/invalidate_this_group") {
		InValidateThisGroup(m);
		return;
	}

	//此功能在总开关前实现
	//if (plain_message == "/validate_this_group") {
	//	group_list[m.Sender.Group.GID] = true;
	//	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nBot On."));
	//	return;
	//}

	//如果没有匹配的命令
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nno such AdminCommand.\n输入/help查看所有命令"));
	return;
}

void GroupPeopleCommandSystem(const GroupMessage& m, const vector<string>& text_vector) {
	string first_command = text_vector[0];

	//以下是所有的命令
	if (first_command == "/help") {
		GetHelp(m);
		return;
	}
	//如果没有匹配的命令
	m.Reply(MessageChain().At(m.Sender.QQ).Plain("\nno such PeopleCommand.\n输入/help查看所有命令"));
}

void GroupFunctionSystem(const GroupMessage& m, const vector<string>& text_vector) {

	return;
}

void GroupNormalSystem(const GroupMessage& m, const vector<string>& text_vector) {
	if (GroupAntiAbuseAndDirtyWords(m, text_vector)) return;
	GroupSaveTextInVector(m, text_vector);
	GroupRandomSpeak(m, text_vector);

	return;
}


//----------------------------------------------------------------------------------
//应用层函数 记得加const
void ReceiveGroupMessage(const GroupMessage& m) {
	//总开关
	if ((IsMaster(m.Sender.QQ) || IsAdmin(m.Sender.QQ)) && (m.MessageChain.GetPlainText() == "/validate_this_group")) {
		ValidateThisGroup(m);
		return;
	}
	if (!IsValidGroup(m.Sender.Group.GID))
		return;

	vector<string> text_vector = SplitText(m.MessageChain.GetPlainText());//如果速度不够，这里可以考虑用静态的vector，目前暂不执行

	//如果没有文字消息,先不管
	if (text_vector.empty()) {
		return;
	}

	GetRealTime();
	TryUpdateChatRecordFileAndBlackList();
	//如果是黑名单里的人
	if (IsInBlackList(m.Sender.QQ)) {
		return;
	}
	//以下的内容都是在"消息包含文字内容"的前提下进行
	//如果是master
	if (IsMaster(m.Sender.QQ)) {
		//如果是command
		if (MessageIsCommand(text_vector[0])) {
			GroupMasterCommandSystem(m, text_vector);
		}
		//如果是function
		else if (MessageIsFunction(text_vector[0])) {
			GroupFunctionSystem(m, text_vector);
		}
		//消息不属于command或者funtion
		else {
			if (ReplyToMasterAt(m)) return;
			GroupNormalSystem(m, text_vector);
		}
		return;
	}


	//如果是admin
	if (IsAdmin(m.Sender.QQ)) {
		//如果是command
		if (MessageIsCommand(text_vector[0])) {
			GroupAdminCommandSystem(m, text_vector);
		}
		//如果是function
		else if (MessageIsFunction(text_vector[0])) {
			GroupFunctionSystem(m, text_vector);
		}
		//消息不属于command或者funtion
		else {
			GroupNormalSystem(m, text_vector);
		}
		return;
	}


	//如果是普通用户
	{
		if (MessageIsCommand(text_vector[0])) {
			GroupPeopleCommandSystem(m, text_vector);
		}
		else if (MessageIsFunction(text_vector[0])) {
			GroupFunctionSystem(m, text_vector);
		}
		else {
			GroupNormalSystem(m, text_vector);
		}
		return;
	}

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

	opts.BotQQ.Set((QQ_t));
	opts.VerifyKey = "114514";
	opts.EnableVerify = true;
	opts.SingleMode = false;
	opts.ThreadPoolSize = 4;
	group_list.clear();//在main函数里面初始化
	command_list.clear();//在main函数里面初始化
	admin_list.clear();//从json文件里面读
	admin_list_json.clear();

	//群列表（这个地方需要想办法存储）
	group_list.insert({ (GID_t),true });
	group_list.insert({ (GID_t),true });
	group_list.insert({ (GID_t),false });

	//这个地方可以每次迭代的时候修改
	command_list.insert({ "/help","查看帮助(All)" });
	command_list.insert({ "/add_admin qq","添加Admin权限(Master)" });
	command_list.insert({ "/delete_admin qq","删除Admin权限(Master)" });
	command_list.insert({ "/list_admins","查看当前admin(Master)" });
	command_list.insert({ "/validate_this_group","启用本群bot(Admin)" });
	command_list.insert({ "/invalidate_this_group","关闭本群bot(Admin)" });

	//把adminlist读进json
	UpdateFile2Json(admin_list_json, admin_list_json_file_path);
	//把json读进vector
	UpdateAdminJson2Vector();

	random_speak_controller.Init();

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
			cout << "Bot Disconnected..." << endl;
			break;
		}
		if (command == "test")
		{

		}

	}
	return 0;
}
