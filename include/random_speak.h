#ifndef _RANDOM_SPEAK_H_
#define _RANDOM_SPEAK_H_

//const string chat_record_json_file_path = "D:\\SucroseBot\\json\\chat_record.json";
const string chat_record_json_file_path = "/home/ubuntu/Desktop/Json/chat_record.json";
class RandomSpeakController {
public:
	json chat_record_json_;
	vector<string> chat_record_vector_;
public:
	void WriteInNewMessage(string new_message);
	void TryDeleteChatRecord();
	void UpdateFile2Json();
	void UpdateJson2Vector();
	void AddVector2Json(string new_message);
	void UpdateVector2Json();
	void UpdateJson2File();
	void Init();
	string GetAMessage(int rand_value);
};


#endif