#ifndef _RANDOM_SPEAK_HPP_
#define _RANDOM_SPEAK_HPP_
using namespace std;
//const string chat_record_json_file_path = "D:\\SucroseBot\\json\\chat_record.json";
const string chat_record_1_json_file_path = "/home/ubuntu/Desktop/Json/chat_record_1.json";
const string chat_record_2_json_file_path = "/home/ubuntu/Desktop/Json/chat_record_2.json";
const string chat_record_3_json_file_path = "/home/ubuntu/Desktop/Json/chat_record_3.json";


class RandomSpeakController {
public:
	json chat_record_1_json_;
	json chat_record_2_json_;
	json chat_record_3_json_;
	vector<string> chat_record_1_vector_;
	vector<string> chat_record_2_vector_;
	vector<string> chat_record_3_vector_;
	void WriteInNewMessage(vector<string>& chat_record_vector,string new_message) {
		chat_record_vector.push_back(new_message);
	}
	void TryDeleteChatRecord(vector<string>& chat_record_vector,json& chat_record_json,string chat_record_json_file_path) {
		if (chat_record_vector.size() >= 10000) {
			for (int i = 0; i < 5000; i++) {
				chat_record_vector.erase(chat_record_vector.begin());
			}
			//在main里面调用的时候，后面有刷新，这里就不刷新了
			//UpdateVector2Json(chat_record_json, chat_record_vector);
			//UpdateJson2File(chat_record_json_file_path, chat_record_json);
		}
	}
	void UpdateFile2Json(string chat_record_json_file_path, json& chat_record_json) {
		ifstream chat_record_file(chat_record_json_file_path);
		chat_record_file >> chat_record_json;
		chat_record_file.close();
	}
	void UpdateJson2Vector(json& chat_record_json, vector<string>& chat_record_vector) {
		for (auto& it : chat_record_json["chat_record"]) {
			chat_record_vector.push_back(it);
		}
		chat_record_json.clear();
	}

	void UpdateVector2Json(json& chat_record_json, vector<string>& chat_record_vector) {
		chat_record_json["chat_record"].clear();
		for (auto it : chat_record_vector) {
			chat_record_json["chat_record"].push_back(it);
		}
		return;
	}
	void UpdateJson2File(string chat_record_json_file_path, json& chat_record_json) {
		ofstream chat_record_file(chat_record_json_file_path, ios::trunc);
		chat_record_file << chat_record_json;
		chat_record_file.close();
		chat_record_json.clear();
		return;
	}
	void Init() {
		UpdateFile2Json(chat_record_1_json_file_path,chat_record_1_json_);
		UpdateJson2Vector(chat_record_1_json_,chat_record_1_vector_);

		UpdateFile2Json(chat_record_2_json_file_path, chat_record_2_json_);
		UpdateJson2Vector(chat_record_2_json_, chat_record_2_vector_);

		UpdateFile2Json(chat_record_3_json_file_path, chat_record_3_json_);
		UpdateJson2Vector(chat_record_3_json_, chat_record_3_vector_);
	}
	string GetAMessage(vector<string>& chat_record_vector,int rand_value) {
		int message_pool_size = chat_record_vector.size();
		if (message_pool_size <= 10) {
			return "嗯嗯";
		}
		return chat_record_vector[rand_value % message_pool_size];

	}

};
#endif