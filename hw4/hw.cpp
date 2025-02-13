#include <iostream>
#include <vector>
#include <thread>
#include <ctime>
#include <condition_variable>
#include <mutex>
#include <atomic>

std::condition_variable cv;

class ArrayThread {
private:
	std::vector<int> arr;
	std::atomic<bool> _isActive = false;
	std::mutex _mtx;
	std::thread _th;
protected:
	void init_gen(size_t n) {
		for (size_t i{ 0 }; i < n; i++) {
			this->arr.push_back(rand() % 10);
		}
	}

	void print() {
	std::unique_lock<std::mutex> lock(_mtx);
	std::cout << "�������" << std::endl;
		while (_isActive) {
			tm time = current_time();
			std::cout << "TIME:" << std::endl;
			std::cout << "HOURS: " << time.tm_hour << " MINUTES: " << time.tm_min << " SECONDS: " << time.tm_sec << std::endl;

			std::cout << "ARRAY SUM:" << std::endl;
			std::cout << arr_sum() << std::endl;
			cv.wait_for(lock, std::chrono::seconds(30));
		}
	}
public:
	ArrayThread() {
		init_gen(1 + (rand() % 10));
	}

	ArrayThread(size_t n) {
		init_gen(n);
	}


	void addElem(int elem, size_t pos) {
		if (pos > this->arr.size()) {
			arr.push_back(elem);
		}
		else {
			this->arr.insert(arr.begin() + pos - 1, elem);
		}
	}

	void removeElem(int elem) {
		auto pos = std::find(arr.begin(), arr.end(), elem);
		if (pos != arr.end()) {
			this->arr.erase(pos);
		}
	}

	int arr_sum() {
		int sum = 0;
		for (size_t i{ 0 }; i < this->arr.size(); i++) {
			sum += this->arr[i];
		}
		return sum;
	}

	tm current_time() {
		time_t now = time(NULL);
		tm currentTime;
		localtime_s(&currentTime, &now);
		return currentTime;
	}

	void stop_printing() {
		if (!_isActive) {
			return;
		}
		_isActive = false;
		cv.notify_one();
		if (_th.joinable()) {
			_th.join();
		}
	}

	void start_printing() {
		if (_isActive) {
			return;
		}
		_isActive = true;
		_th = std::thread(&ArrayThread::print, this);
	}
};

int main() {
	setlocale(LC_ALL, "rus");
	srand(time(NULL));

	ArrayThread arr(10);

	std::cout << "�� ������:" << std::endl;
	std::cout << "1 - �������� ������� � ������" << std::endl;
	std::cout << "2 - ������� ������� �� �������" << std::endl;
	std::cout << "3 - �������� ������ � ������� ����� �������" << std::endl;
	std::cout << "4 - ��������� ������ � ������� ����� �������" << std::endl;
	std::cout << "0 - �����" << std::endl;
	std::cout << "��� �����: " << std::endl;

	size_t choice = 1;
	while (choice != 0) {
		std::cin >> choice;
		switch (choice) {
		case 1:
			arr.addElem(rand() % 10, rand() % 10);
			break;
		case 2:
			arr.removeElem(rand() % 10);
			break;
		case 3:
			arr.start_printing();
			break;
		case 4:
			arr.stop_printing();
			break;
		default:
			choice = 0;
			arr.stop_printing();
			break;
		}
	}

	return 0;
}