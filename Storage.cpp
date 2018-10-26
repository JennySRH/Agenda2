#include "Storage.hpp"
#include "Path.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
using namespace std;

shared_ptr<Storage> Storage::m_instance = nullptr;


//	static std::shared_ptr<Storage> m_instance;
//	std::list<User> m_userList;
//	std::list<Meeting> m_meetingList;
//	bool m_dirty;



	/**
	*   default constructor
	*/
	Storage::Storage() {
		m_dirty = false;
		readFromFile();
	}

	/**
	*   disallow the copy constructor and assign operator
	*/

	/**
	*   read file content into memory
	*   @return if success, true will be returned
	*/
	bool Storage::readFromFile(void) {
		ifstream fin1(Path::userPath);
		ifstream fin2(Path::meetingPath);
		if (!fin1.is_open() || !fin2.is_open()) {
			return false;
		}
		string user_str, meeting_str;
		while (getline(fin1, user_str)) {
			if (user_str.length() > 11) {
				int mark_count = 0;
				string name;
				string password;
				string email;
				string phone;
				for (int i = 0; i < user_str.length(); i++) {
					if (user_str[i] == '\"') {
						mark_count++;
						int word_count = 0;
						for (int j = i + 1; j < user_str.length(); j++) {
							if (user_str[j] == '\"') {
								mark_count++;
								break;
							}
							word_count++;
						}
					
						if (mark_count == 2) {
							name = user_str.substr(i + 1, word_count);
						}
						else if (mark_count == 4) {
							password = user_str.substr(i + 1, word_count);
						}
						else if (mark_count == 6) {
							email = user_str.substr(i + 1, word_count);
						}
						else if(mark_count == 8){
							phone = user_str.substr(i + 1, word_count);
						}
						i = i + word_count + 1;
					}
				}
	
				User temp(name, password, email, phone);
				m_userList.push_back(temp);
			}

		}

		while (getline(fin2, meeting_str)) {
			if (meeting_str.length() > 14) {
				int mark_count = 0;
				string sponsor;
				vector<string> participators;
				string startDate;
				string endDate;
				string title;
				for (int i = 0; i < meeting_str.length(); i++) {
					if (meeting_str[i] == '\"') {
						mark_count++;
						int word_count = 0;
						for (int j = i + 1; j < meeting_str.length(); j++) {
							if (meeting_str[j] == '\"') {
								mark_count++;
								break;
							}
							word_count++;
						}

						if (mark_count == 2) {
							sponsor = meeting_str.substr(i + 1, word_count);
						}
						else if (mark_count == 4) {
							string participators_temp;
							participators_temp = meeting_str.substr(i + 1, word_count);
							int start = 0;
							int count_p = 0;
							for (int j = 0; j < participators_temp.length(); j++) {
								if (participators_temp[j] == '&') {
									string p_str;
									p_str = participators_temp.substr(start, count_p);
									participators.push_back(p_str);
									count_p = 0;
									start = j+1;
								}
								else {
									count_p++;
								}
							}
							string p_last;
							p_last = participators_temp.substr(start, participators_temp.length() - start);
							participators.push_back(p_last);
						}
						else if (mark_count == 6) {
							startDate = meeting_str.substr(i + 1, word_count);
						}
						else if (mark_count == 8) {
							endDate = meeting_str.substr(i + 1, word_count);
						}
						else if (mark_count == 10) {
							title = meeting_str.substr(i + 1, word_count);
						}
						i = i + word_count + 1;
					}

				}

				Meeting temp(sponsor, participators, (Date)startDate, (Date)endDate, title);
				m_meetingList.push_back(temp);
			}
		}
		return true;

	}

	/**
	*   write file content from memory
	*   @return if success, true will be returned
	*/
	bool Storage::writeToFile(void) {
		fstream fout1(Path::userPath);
		fstream fout2(Path::meetingPath);
		if (!fout1.is_open() || !fout2.is_open() || (m_userList.size() == 0 && m_meetingList.size() == 0)) {
			return false;
		}
		for (auto it = m_userList.begin(); it != m_userList.end(); it++) {
			string user_str = "";
			user_str += "\"";
			user_str += (*it).getName();
			user_str += "\",\"";
			user_str += (*it).getPassword();
			user_str += "\",\"";
			user_str += (*it).getEmail();
			user_str += "\",\"";
			user_str += (*it).getPhone();
			user_str += "\"\n";
			fout1 << user_str;
		}
		for (auto it = m_meetingList.begin(); it != m_meetingList.end(); it++) {
			string meeting_str = "";
			meeting_str += "\"";
			meeting_str += (*it).getSponsor();
			meeting_str += "\",\"";
			vector<string> participators = (*it).getParticipator();
			string p_str = "";
			for (auto it2 = participators.begin(); it2 != participators.end(); it2++) {
				p_str += *it2;
				if (it2 + 1 != participators.end()) {
					p_str += "&";
				}
			}
			meeting_str += p_str;
			meeting_str += "\",\"";
			meeting_str += (*it).getStartDate().dateToString((*it).getStartDate());
			meeting_str += "\",\"";
			meeting_str += (*it).getEndDate().dateToString((*it).getEndDate());
			meeting_str += "\",\"";
			meeting_str += (*it).getTitle();
			meeting_str += "\"\n";
			fout2 << meeting_str;
		}
		return true;
	}

	/**
	* get Instance of storage
	* @return the pointer of the instance
	*/
	std::shared_ptr<Storage> Storage::getInstance(void) {
		if (m_instance == nullptr) {
			m_instance = shared_ptr<Storage>(new Storage());
			return m_instance;
		}
		return m_instance;
	}

	/**
	*   destructor
	*/
	Storage::~Storage() {
		sync();
	}

	// CRUD for User & Meeting
	// using C++11 Function Template and Lambda Expressions

	/**
	* create a user
	* @param a user object
	*/
	void Storage::createUser(const User &t_user) {
		m_userList.push_back(t_user);
		m_dirty = true;
	}

	/**
	* query users
	* @param a lambda function as the filter
	* @return a list of fitted users
	*/
	std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const {
		list<User> fitted_users;
		for (auto it = m_userList.begin(); it != m_userList.end(); it++) {
			if (filter(*it)) {
				fitted_users.push_back(*it);
			}
		}
		return fitted_users;
	}

	/**
	* update users
	* @param a lambda function as the filter
	* @param a lambda function as the method to update the user
	* @return the number of updated users
	*/
	int Storage::updateUser(std::function<bool(const User &)> filter,
		std::function<void(User &)> switcher) {
		int count = 0;
		for (auto it = m_userList.begin(); it != m_userList.end(); it++) {
			if (filter(*it)) {
				switcher(*it);
				count++;
			}
		}
		if (count > 0) {
			m_dirty = true;
		}
		return count;
	}

	/**
	* delete users
	* @param a lambda function as the filter
	* @return the number of deleted users
	*/
	int Storage::deleteUser(std::function<bool(const User &)> filter) {
		int count = 0;
		for (auto it = m_userList.begin(); it != m_userList.end(); it++) {
			if (filter(*it)) {
				count++;
				m_userList.erase(it);
			}
		}
		if (count > 0) {
			m_dirty = true;
		}
		return count;
	}

	/**
	* create a meeting
	* @param a meeting object
	*/
	void Storage::createMeeting(const Meeting &t_meeting) {
		m_meetingList.push_back(t_meeting);
		m_dirty = true;
	}

	/**
	* query meetings
	* @param a lambda function as the filter
	* @return a list of fitted meetings
	*/
	std::list<Meeting> Storage::queryMeeting(
		std::function<bool(const Meeting &)> filter) const {
		list<Meeting> fitted_meetings;
		for (auto it = m_meetingList.begin(); it != m_meetingList.end(); it++) {
			if (filter(*it)) {
				fitted_meetings.push_back(*it);
			}
		}
		return fitted_meetings;
	}

	/**
	* update meetings
	* @param a lambda function as the filter
	* @param a lambda function as the method to update the meeting
	* @return the number of updated meetings
	*/
	int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
		std::function<void(Meeting &)> switcher) {
		int count = 0;
		for (auto it = m_meetingList.begin(); it != m_meetingList.end(); it++) {
			if (filter(*it)) {
				switcher((*it));
				count++;
			}
		}
		if (count > 0) {
			m_dirty = true;
		}
		return count;
	}

	/**
	* delete meetings
	* @param a lambda function as the filter
	* @return the number of deleted meetings
	*/
	int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter) {
		int count = 0;
		for (auto it = m_meetingList.begin(); it != m_meetingList.end(); it++) {
			if (filter(*it)) {
				count++;
				m_meetingList.erase(it);
			}
		}
		if (count > 0) {
			m_dirty = true;
		}
		return count;
	}

	/**
	* sync with the file
	*/
	bool Storage::sync(void) {
		if (m_dirty) {
			if (writeToFile()) {
				m_dirty = false;
			}
			else {
				return false;
			}
		}
		return true;


	}



