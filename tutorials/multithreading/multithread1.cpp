#include <iostream>
#include <thread>
using namespace std;

static const int NUM_THREADS = 10;

//This function will be called from a thread

void call_from_thread(int tid) {
    std::cout << "Launched by thread " << tid << std::endl;
}

int main() {
    //Launch a thread
    std::thread t[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        t[i] = std::thread(call_from_thread, i);
    }
    
    std::cout << "Launched from main" << std::endl;
    
    //Join the thread with the main thread
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        t[i].join();
    }
    return 0;
}
