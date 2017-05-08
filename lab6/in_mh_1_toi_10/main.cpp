#include <pthread.h>
#include <iostream>

const int NUM_THREADS = 10;

int number = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* printNumber(void* /*args*/)
{
	pthread_mutex_lock(&mutex);
	std::cout << ++number << std::endl;
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
	return 0;
}

int main()
{
	pthread_t threads[NUM_THREADS];

	int threadCreateResult;
	for (int i = 0; i < NUM_THREADS; ++i) {
		threadCreateResult = pthread_create(&threads[i], NULL, printNumber, (void*) 0);
	}

	pthread_exit(NULL);
	return 0;
}
