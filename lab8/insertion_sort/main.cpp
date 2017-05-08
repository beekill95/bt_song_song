#include <mpi.h>
#include <iostream>
#include <limits>
#include <cstring>

/**
 * Chuong trinh Insertion sort bang giai thuat song song su dung phuong phap pipeline
 * sap xep mang bat ky thanh mang tang dan
 * 
 * Luu y: chuong trinh se chay sai neu trong mang can sap xep co so integer nho nhat (MIN_INTEGER)
 * */

const int UNASSIGNED_NUMBER = std::numeric_limits<int>::min();
const int arr[] = {
	3, 7, 2, 1, 0, -5, 8, 9
};
const int arrLength = 8;

const int TAG_NUMBER = 0;
const int TAG_CONTINUE = 1;

int* allocateArray(int size)
{
	int* arr = new int[size];
	for (int i = 0; i < size; ++i)
		arr[i] = UNASSIGNED_NUMBER;
	return arr;
}

void deallocateArray(int*& arr)
{
	delete[] arr;
	arr = 0;
}

int insert(int* arr, int size, int newNum)
{
	std::cout << "Vao insert" << std::endl;
	int lastElement;
	if (arr[0] == UNASSIGNED_NUMBER) {
		// chung ta dang add vao dau mang
		// do do, cu add vao dau mang thoai mai
		arr[0] = newNum;
		lastElement = UNASSIGNED_NUMBER;
		std::cout << "Insert vao vi tri dau tien" << std::endl;
	} else {
		// trong mang da co nhung so truoc do roi
		// chung ta can phai tim mot cho phu hop de them so moi vao
		int i = 0;
		while (i < size && newNum > arr[i] && arr[i] != UNASSIGNED_NUMBER) {
			++i;
		}

		// insert vao la cho i
		if (arr[i] == UNASSIGNED_NUMBER) {
			// cu insert truc tiep vao, khong can phai dich chuyen cac phan tu trong mang sang phai
			arr[i] = newNum;
			lastElement = UNASSIGNED_NUMBER;
			std::cout << "Insert vao vi tri thu " << i << std::endl;
		} else {
			std::cout << "Truoc khi move" << std::endl;
			// dich cac so khac tu vi tri i+1 sang phai
			lastElement = arr[size - 1];		
			memmove(arr + i + 1, arr + i, (size - i - 1) * sizeof(int));
			arr[i] = newNum;
			std::cout << "Move" << std::endl;
		}
	}

	return lastElement;
}

void printArray(int* arr, int size)
{
	for (int i = 0; i < size; ++i)
		std::cout << arr[i] << ' ';
	std::cout << std::endl;
}

void firstProcess(int numElements, int lastProcessElements, int size)
{
	// ta tao ra mang chua ket qua sap xep
	bool isContinue = true;
	int* array = allocateArray(numElements);
	printArray(array, numElements);

	for (int i = 0; i < arrLength; ++i) {
		std::cout << "Insert lan thu " << i << std::endl;

		// insert mot so moi vao trong mang
		int lastElement = insert(array, numElements, arr[i]);
		
		// neu so cuoi cung khong phai la UNASSIGNED_NUMBER
		// ta gui so nay cho process ke tiep
		if (lastElement != UNASSIGNED_NUMBER) {
			MPI_Send(&lastElement, 1, MPI_INT, 1, TAG_NUMBER, MPI_COMM_WORLD);
			MPI_Send(&isContinue, 1, MPI_CXX_BOOL, 1, TAG_CONTINUE, MPI_COMM_WORLD);
		}
	}

	// sau khi da gui het tat ca cac so trong mang, ta gui tin hieu ket thuc
	isContinue = false;
	MPI_Send(&isContinue, 1, MPI_CXX_BOOL, 1, TAG_CONTINUE, MPI_COMM_WORLD);

	// ta nhan ket qua tra ve da sap xep tu cac process
	int* result = allocateArray(arrLength);
	int index = 0;
	memcpy(result, array, sizeof(int) * numElements);
	index += numElements;
	MPI_Status status;
	for (int i = 1; i < size - 1; ++i) {
		MPI_Recv(result + index, numElements, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		index += numElements;
	}
	MPI_Recv(result + index, lastProcessElements, MPI_INT, size - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	// in ra ket qua da sap xep
	printArray(result, arrLength);

	// xoa nhung mang vua tao
	deallocateArray(array);
	deallocateArray(result);
}

void otherProcesses(int rank, int numElements)
{
	// ta tao ra mang de chua ket qua
	MPI_Status status;
	bool isContinue;
	int* arr = allocateArray(numElements);

	// nhan flag xem coi co ket thuc hay khong
	MPI_Recv(&isContinue, 1, MPI_CXX_BOOL, rank - 1, TAG_CONTINUE, MPI_COMM_WORLD, &status);

	while (isContinue) {
		// nhan so de sap xep
		int newNum;
		MPI_Recv(&newNum, 1, MPI_INT, rank - 1, TAG_NUMBER, MPI_COMM_WORLD, &status);
		int lastElement = insert(arr, numElements, newNum);

		// neu khong phai la UNASSIGNED_NUMBER thi ta se gui tiep
		if (lastElement != UNASSIGNED_NUMBER) {
			MPI_Send(&lastElement, 1, MPI_INT, rank + 1, TAG_NUMBER, MPI_COMM_WORLD);
			MPI_Send(&isContinue, 1, MPI_CXX_BOOL, rank + 1, TAG_CONTINUE, MPI_COMM_WORLD);
		}

		// nhan flag xem co tiep tuc hay khong
		MPI_Recv(&isContinue, 1, MPI_CXX_BOOL, rank - 1, TAG_CONTINUE, MPI_COMM_WORLD, &status);
	}

	// gui flag la da ket thuc
	MPI_Send(&isContinue, 1, MPI_CXX_BOOL, rank + 1, TAG_CONTINUE, MPI_COMM_WORLD);

	// gui ket qua ve cho process dau tien
	MPI_Send(&arr, numElements, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);

	// xoa di nhung mang da tao
	deallocateArray(arr);
}

void lastProcess(int rank, int numElements)
{
	// ta tao ra mang de chua ket qua
	MPI_Status status;
	bool isContinue;
	int* arr = allocateArray(numElements);

	// nhan flag xem coi co ket thuc hay khong
	MPI_Recv(&isContinue, 1, MPI_CXX_BOOL, rank - 1, TAG_CONTINUE, MPI_COMM_WORLD, &status);

	while (isContinue) {
		// nhan so de sap xep
		int newNum;
		MPI_Recv(&newNum, 1, MPI_INT, rank - 1, TAG_NUMBER, MPI_COMM_WORLD, &status);

		// do day la process cuoi cung, nen chi can insert, bo qua so cuoi cung
		insert(arr, numElements, newNum);

		// nhan flag xem co tiep tuc hay khong
		MPI_Recv(&isContinue, 1, MPI_CXX_BOOL, rank - 1, TAG_CONTINUE, MPI_COMM_WORLD, &status);
	}

	// gui ket qua ve cho process dau tien
	MPI_Send(&arr, numElements, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);

	// xoa di nhung mang da tao
	deallocateArray(arr);
}

int main(int argc, char** argv)
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// chia cong viec cho tung process
	int perProcessElements = arrLength / size;
	int lastProcessElements = arrLength % size;

	if (rank == 0) {
		firstProcess(perProcessElements, lastProcessElements, size);
	} else if (rank < size - 1) {
		otherProcesses(rank, perProcessElements);
	} else {
		lastProcess(rank, lastProcessElements);
	}

	MPI_Finalize();
	return 0;
}
