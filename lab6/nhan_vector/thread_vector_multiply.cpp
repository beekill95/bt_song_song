#include <iostream>
#include <cstring>
#include <cstdint>
#include <pthread.h>

const uint64_t NUM_THREADS = 2;
const uint64_t VECTOR_LENGTH = 100;

struct MultiplyVectorArguments {
	uint64_t* lhs;
	uint64_t* rhs;
	uint64_t start;
	uint64_t end;
	uint64_t result;
};

void *multiplyVector(void* params)
{
	MultiplyVectorArguments* args = (MultiplyVectorArguments*) params;
	uint64_t sum = 0;
	for (uint64_t i = args->start; i <	args->end; ++i)
		sum += args->lhs[i] * args->rhs[i];
	args->result = sum;

//	std::cout << "Ket qua tong thread la: " << sum << std::endl;

	return 0;
}

int main()
{
	// tao mang
	uint64_t lhs[VECTOR_LENGTH];
	uint64_t rhs[VECTOR_LENGTH];
	for (uint64_t i = 0; i < VECTOR_LENGTH; ++i) {
		lhs[i] = 1;
		rhs[i] = 1;
	}

	// tao threads
	pthread_t threads[NUM_THREADS];

	// chia cong viec cho tung threads
	uint64_t elementsPerThread = VECTOR_LENGTH / NUM_THREADS;
	uint64_t start = 0;
	MultiplyVectorArguments threadArguments[NUM_THREADS];
	int threadCreateResult;
	for (uint64_t i = 0; i < NUM_THREADS; ++i) {
		MultiplyVectorArguments* args = &threadArguments[i];
		
		// tao argument cho tung thread
		args->lhs = lhs;
		args->rhs = rhs;
		args->start = start;
		args->end = start + elementsPerThread;

		// tao thread va truyen argument cho thread
		threadCreateResult = pthread_create(&threads[i], NULL, multiplyVector, (void*) args);

		// kiem tra viec tao thread co thanh cong
		if (threadCreateResult) {
			std::cerr << "Tao thread thu " << i << " xay ra loi!" << std::endl;
			return -1;
		}

		// tang bien dem vi tri
		start += elementsPerThread;
	}

	// doi cho cac thread lam xong cong viec cua minh
	void* status;
	int threadJoinResult;
	uint64_t result = 0;
	for (uint64_t i = 0; i < NUM_THREADS; ++i) {
		threadJoinResult = pthread_join(threads[i], &status);

		// kiem tra ket qua join thread
		if (threadJoinResult) {
			std::cerr << "Join thread thu " << i << " xay ra loi!" << std::endl;
			return -1;
		}

		// neu khong co loi gi
		// lay ket qua tra ve cua thread
		result += threadArguments[i].result;
	}

	// in ket qua ra man hinh
	std::cout << "Ket qua nhan vector la: " << result << std::endl;

	return 0;
}
