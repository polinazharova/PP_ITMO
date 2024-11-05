#include <iostream>
#include <vector>
#include <thread>
#include <ctime>
#include <condition_variable>
#include <mutex>

std::mutex mtx;
std::condition_variable cv;

class ArrayThread {
private:
	std::vector<int> arr;
	bool running = false;
	bool pending = false;
protected:
	void init_gen(size_t n) {
		for (size_t i{ 0 }; i < n; i++) {
			this->arr.push_back(rand() % 10);
		}
	}

	void print() {
		std::unique_lock<std::mutex> lock(mtx);
		while (true) {
			std::cout << "Çàïóùåí" << std::endl;
			while (running) {
				tm time = current_time();
				std::cout << "TIME:" << std::endl;
				std::cout << "HOURS: " << time.tm_hour << " MINUTES: " << time.tm_min << " SECONDS: " << time.tm_sec << std::endl;

				std::cout << "ARRAY SUM:" << std::endl;
				std::cout << arr_sum() << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(5));
			}
			std::cout << "Îñòàíîâëåí" << std::endl;
			pending = true;
			cv.wait(lock);
			if (!running) {
				std::cout << "Âûêëþ÷åí" << std::endl;
				pending = false;
				break;
			}
		}
	}
public:
	ArrayThread() {
		init_gen(1 + (rand() % 10));
	}

	ArrayThread(size_t n) {
		init_gen(n);
	}

	bool get_pending() { return pending; }

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
		running = false;
		cv.notify_one();
	}

	void start_printing() {
		if (!pending) {
			running = true;
			print();
		}
		else {
			pending = false;
			running = true;
			cv.notify_one();
		}
	}

	bool get_print_status() { return running; }
};

int main() {
	setlocale(LC_ALL, "rus");
	srand(time(NULL));

	ArrayThread arr(10);
	std::thread thr(&ArrayThread::start_printing, &arr);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::cout << "ÂÛ ÌÎÆÅÒÅ:" << std::endl;
	std::cout << "1 - ÄÎÁÀÂÈÒÜ ÝËÅÌÅÍÒ Â ÌÀÑÑÈÂ" << std::endl;
	std::cout << "2 - ÓÄÀËÈÒÜ ÝËÅÌÅÍÒ ÈÇ ÌÀÑÑÈÂÀ" << std::endl;
	std::cout << "3 - ÂÊËÞ×ÈÒÜ ÒÀÉÌÅÐ È Ñ×ÅÒ×ÈÊ ÑÓÌÌÛ ÌÀÑÑÈÂÀ" << std::endl;
	std::cout << "4 - ÂÛÊËÞ×ÈÒÜ ÒÀÉÌÅÐ È Ñ×ÅÒ×ÈÊ ÑÓÌÌÛ ÌÀÑÑÈÂÀ" << std::endl;
	std::cout << "0 - ÂÛÕÎÄ" << std::endl;
	std::cout << "Âàø âûáîð: " << std::endl;

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
			if (arr.get_print_status()) {
				std::cout << "ÓÆÅ ÇÀÏÓÙÅÍ!" << std::endl;
			}
			else { 
				arr.start_printing();
			}
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

	while (!arr.get_pending()) {}
	arr.stop_printing();
	thr.join();

	return 0;
}