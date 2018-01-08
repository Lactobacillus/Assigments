#include <deque>
#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <conio.h>
#include <time.h>

#define WINDOW 5

std::deque <time_t> OK;
std::deque <time_t> OK_m;
std::deque <time_t> Right;
std::deque <time_t> Right_m;
std::deque <time_t> You_know;
std::deque <time_t> You_know_m;
std::deque <time_t> All;
std::deque <time_t> All_m;

void scanner(time_t start);
void S_clock(time_t start);
std::string printer(std::deque <time_t> data, std::deque <time_t> data_m, std::string name);

void scanner(time_t start){

		while (1){

			char ch = _getch();
			time_t now = time(NULL);

			switch (ch){

			case 'o':
			case 'O':

				OK.push_back(now);
				OK_m.push_back(now);
				All.push_back(now);
				All_m.push_back(now);
				break;

			case 'y':
			case 'Y':

				You_know.push_back(now);
				You_know_m.push_back(now);
				All.push_back(now);
				All_m.push_back(now);
				break;

			case 'r':
			case 'R':

				Right.push_back(now);
				Right_m.push_back(now);
				All.push_back(now);
				All_m.push_back(now);
				break;

			default:

				break;
			}

			system("CLS");

			std::cout << "============================\n\n";
			std::cout << "  Computer Network Counter\n\n";
			std::cout << "============================\n\n";
			std::cout << "time : " << now - start << "\n\n";

			std::cout << printer(OK, OK_m, std::string("OK"));
			std::cout << printer(Right, Right_m, std::string("Right"));
			std::cout << printer(You_know, You_know_m, std::string("You_know"));
			std::cout << printer(All, All_m, std::string("Action"));
		}
}

void s_clock(time_t start){

		while (1){

			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			time_t now = time(NULL);

			system("CLS");
		
			std::cout << "============================\n\n";
			std::cout << "  Computer Network Counter\n\n";
			std::cout << "============================\n\n";
			std::cout << "time : " << now - start << "\n\n";

			std::cout << printer(OK, OK_m, std::string("OK"));
			std::cout << printer(Right, Right_m, std::string("Right"));
			std::cout << printer(You_know, You_know_m, std::string("You_know"));
			std::cout << printer(All, All_m, std::string("Action"));
		}
}

int main(){
	
	time_t start = time(NULL);

	std::thread scan(&scanner, start);
	std::thread clock(&s_clock, start);	

		while (1){

			time_t now = time(NULL);

			while (OK_m.size() > 0 && now - OK_m.front() > WINDOW){
				
				OK_m.pop_front();
			}
			
			while (Right_m.size() > 0 && now - Right_m.front() > WINDOW){

				Right_m.pop_front();
			}

			while (You_know_m.size() > 0 && now - You_know_m.front() > WINDOW){

				You_know_m.pop_front();
			}

			while (All_m.size() > 0 && now - All_m.front() > WINDOW){

				All_m.pop_front();
			}
		}

		scan.join();
		clock.join();

	return 0;
}

std::string printer(std::deque <time_t> data, std::deque <time_t> data_m, std::string name){
	
	__int64 avg;
	std::string display;

		if (data.size() == 0){

			avg = 0;
		}
		else if (data.size() == 1){

			avg = 1;
		}
		else if (data.back() == data.front()){

			avg = data.size();
		}
		else{

			avg = data.size() / (data.back() - data.front());
		}

		display = "count " + name + ":" + std::to_string(data.size()) + "\n";
		
		display += name + " per minute (" + name[0] + "PM) : ";
		display += std::to_string(data_m.size() * (60 / WINDOW) + (data_m.size() > 0? std::rand() % WINDOW : 0)) + "\n";
		display += "[";
		for (int i = 0; i < (int)data_m.size() && i < 20; i++){

			display += std::string("бс");
		}
		for (int i = 0; i < 20 - (int)data_m.size() && (int)data_m.size() < 21; i++){

			display += std::string("бр");
		}
		display += "]\n\n";

	return display;
}