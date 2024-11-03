#include <iostream>
#include <thread>
#include <mutex>
#include <random>


std::mutex mtx;
size_t N = 3;


std::mt19937 hashRand(std::thread::id th) {
    std::hash<std::thread::id> hasher;
    size_t threadIdHash = hasher(th);
    std::mt19937 gen(threadIdHash);
    return gen;
}

void lines() {
    std::cout << "---------------------------------" << std::endl;
}


void maker(std::vector<int>& pool, int& goods) {
    mtx.lock();
    lines();
    std::mt19937 gen = hashRand(std::this_thread::get_id());
    std::uniform_int_distribution<> dis(0, 99);

    goods = rand() % 4;
    while (pool.size() > N - goods) {
        pool.erase(pool.begin());
    }
    std::cout << "Я ПОЛОЖИЛ " << goods << " ТОВАРОВ: ";
    for (int i{ 0 }; i < goods; i++) {
        pool.push_back(rand() % 100);
        std::cout << pool[pool.size() - 1] << " ";
    }
    std::cout << std::endl;
    lines();
    mtx.unlock();
}

void consumer(std::vector<int>& pool, const int& goods) {
    mtx.lock();
    lines();

    if (goods < 0) {
        std::cout << "Кажется, производитель еще не привез товар..." << std::endl;
    }
    else {
        std::cout << "Я ЗАБРАЛ " << goods << " ТОВАРОВ: ";
        for (int i{ 0 }; i < goods; i++) {
            std::cout << pool[pool.size() - 1] << " ";
            pool.pop_back();      
        }
        std::cout << std::endl;
    }
    lines();
    mtx.unlock();
}

int t1() {
    std::vector<int> pool(rand() % (N + 1));
    int goods = -1;

    std::thread t1(maker, std::ref(pool), std::ref(goods));
    std::thread t2(consumer, std::ref(pool), std::ref(goods));

    t1.join();
    t2.join();

    return 0;
}


void writer(std::vector<int>& pool, int& books) {
    mtx.lock();
    lines();

    std::mt19937 gen = hashRand(std::this_thread::get_id());
    std::uniform_int_distribution<> dis(0, 99);
    

    books = dis(gen) % (N * 2);
    while (pool.size() > (N * 2) - books) {
        pool.erase(pool.begin());
    }

    std::cout << "Я НАПИСАЛ " << books << " КНИГ: ";
    for (int i{ 0 }; i < books; i++) {
        pool.push_back(dis(gen));
        std::cout << pool[pool.size() - 1] << " ";
    }
    std::cout << std::endl;
    lines();
    mtx.unlock();
}

void reader(std::vector<int>& pool, int& books) {
    mtx.lock();
    lines();
    if (pool.size() <= 0) {
        std::cout << "Пока что читать нечего..." << std::endl;
    }
    else {
        std::mt19937 gen = hashRand(std::this_thread::get_id());
        std::uniform_int_distribution<> dis(0, 99);

        books = dis(gen) % (N * 2);
        std::cout << "Я ХОЧУ ПРОЧИТАТЬ " << books << " КНИГ" << std::endl;
        if (books > pool.size()) {
            books = pool.size();
        }
        std::cout << "Я ПРОЧИТАЛ " << books << " КНИГ: ";
        for (int i{ 0 }; i < books; i++) {
            int taken = dis(gen) % pool.size();
            std::cout << pool[taken] << " ";
        }
        std::cout << std::endl;
    }
    lines();
    mtx.unlock();
}

int t2() {
    std::vector<int> pool;
    int books = -1;

    std::thread w1(writer, std::ref(pool), std::ref(books));
    std::thread r1(reader, std::ref(pool), std::ref(books));
    std::thread w2(writer, std::ref(pool), std::ref(books));
    std::thread r2(reader, std::ref(pool), std::ref(books));
    std::thread w3(writer, std::ref(pool), std::ref(books));
    std::thread r3(reader, std::ref(pool), std::ref(books));

    w1.join();
    r1.join();
    w2.join();
    r2.join();
    w3.join();
    r3.join();
    
    return 0;
}

std::vector<std::mutex> mtxs(5);
void ph_forks(const int ph_id, std::mutex& m1, std::mutex& m2, const int thinker) {
    if (thinker == ph_id) {
        std::cout << "ФИЛОСОФ " << ph_id + 1 << " ЗАДУМАЛСЯ" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    m2.lock();
    std::cout << "ФИЛОСОФ " << ph_id + 1 << " ВЗЯЛ ПРАВУЮ ВИЛКУ" << std::endl;
    m1.lock();
    std::cout << "ФИЛОСОФ " << ph_id + 1 << " ВЗЯЛ ЛЕВУЮ ВИЛКУ" << std::endl;
    lines();
    std::cout << "ФИЛОСОФ " << ph_id + 1 << " ЕСТ" << std::endl;
    std::cout << "ФИЛОСОФ " << ph_id + 1 << " ДОЕЛ" << std::endl;

    m2.unlock();
    m1.unlock();
}

int t3() {
    std::mt19937 gen = hashRand(std::this_thread::get_id());
    std::uniform_int_distribution<> dis(0, 99);
    int thinker = dis(gen) % 5;

    std::thread p0(ph_forks, 0, std::ref(mtxs[4]), std::ref(mtxs[0]), thinker);
    std::thread p1(ph_forks, 1, std::ref(mtxs[0]), std::ref(mtxs[1]), thinker);
    std::thread p2(ph_forks, 2, std::ref(mtxs[1]), std::ref(mtxs[2]), thinker);
    std::thread p3(ph_forks, 3, std::ref(mtxs[2]), std::ref(mtxs[3]), thinker);
    std::thread p4(ph_forks, 4, std::ref(mtxs[3]), std::ref(mtxs[4]), thinker);

    
    p0.join();
    p1.join();
    p2.join();
    p3.join();
    p4.join();

    return 0;
}


std::condition_variable cv;
std::condition_variable cv1;
std::mutex mtx1;
bool in_process = false;
bool ready = true;
bool asleep = true;
void hairdresser(std::vector<int>& que) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        if (que.empty()) {
            asleep = true;
            ready = true;
            std::cout << "Парикхмахер уснул..." << std::endl;
            cv.wait(lock);
        }

        asleep = false;

        while (!in_process) {}
        ready = false;
        std::cout << "...СТРИЖКА..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        std::cout << "КЛИЕНТ УШЕЛ!" << std::endl;

        in_process = false;
        cv1.notify_one();
    }

}

void customers(std::vector<int>& que) {
    std::unique_lock<std::mutex> lock(mtx1);
    if (que.size() >= N) {
        std::cout << "В очереди нет мест..." << std::endl;
        return;
    }
    else {
        std::cout << "Клиент занял очередь" << std::endl;
        que.push_back(1);

        while (asleep) {
            cv.notify_one();
        }

        if (!ready) {
            cv1.wait(lock);
        }
        std::cout << "КЛИЕНТ ПОШЕЛ К ПАРИКХМАХЕРУ!" << std::endl;
        que.erase(que.begin());
        in_process = true;
    }
}

int t4() {
    std::vector<int> que;
    
    std::thread hd(hairdresser, std::ref(que));
    std::thread c1(customers, std::ref(que));
    std::thread c2(customers, std::ref(que));
    std::thread c3(customers, std::ref(que));
    std::thread c4(customers, std::ref(que));
    std::thread c5(customers, std::ref(que));
    std::thread c6(customers, std::ref(que));
    std::thread c7(customers, std::ref(que));

    c1.join();
    c2.join();
    c3.join();
    c4.join();
    c5.join();
    c6.join();
    c7.join();
    hd.join();

    return 0;
}


int main()
{   
    setlocale(LC_ALL, "rus");

    std::cout << "///TASK 1///" << std::endl;
    t1();

    std::cout << "///TASK 2///" << std::endl;
    t2();

    std::cout << "///TASK 3///" << std::endl;
    t3();

    std::cout << "///TASK 4///" << std::endl;
    t4();

    return 0;
}

