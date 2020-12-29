#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>

using namespace std;

int num;
bool status = false;
mutex m;
condition_variable cv1;

enum class gameStatus {
	GUESS,
	NEWNUM
};
gameStatus gstatus = gameStatus::NEWNUM;

void getRandNum() {
	srand(time(NULL));
	while (!status) {
		if (gstatus == gameStatus::NEWNUM) {
			unique_lock<mutex> um(m);
			num = rand() % 10;
			cout << "P1: My num is " << num << " and your guess?" << endl;
			gstatus = gameStatus::GUESS;
			um.unlock();
			cv1.notify_one();

		}
	}
}

void tryToGuess() {
	int guessNum = 0;
	this_thread::sleep_for(chrono::milliseconds(1000));
	srand(time(NULL));
	while (!status) {
		unique_lock<mutex> um(m);
		cv1.wait(um, [=]() {return gstatus == gameStatus::GUESS; });
		guessNum = rand() % 10;
		cout << "P2: My guess is " << guessNum << endl;
		if (guessNum == num) {
			cout << "P2: I finaly won! Your num is " << num << " and my " << guessNum << endl;
			this_thread::sleep_for(chrono::milliseconds(1000));
			gstatus = gameStatus::NEWNUM;
		}
	}
}

void stopGame() {
	string s;
	while (!status) {
		cin >> s;
		if (s == "q")
			status = true;
	}
}

int main() {

	thread t1(getRandNum);
	this_thread::sleep_for(chrono::milliseconds(1));
	thread t2(tryToGuess);
	thread t3(stopGame);

	t1.join();
	t2.join();
	t3.join();

	return EXIT_SUCCESS;
}