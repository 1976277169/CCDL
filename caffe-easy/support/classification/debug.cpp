#include "classification.h"
#include <highgui.h>
#include <process.h>
#define RootDir "../../../../../demo-data/veriCode/"

using namespace cv;

vector<char> readAtFile(const char* filename){
	FILE* f = fopen(filename, "rb");
	if (f == 0) return vector<char>();

	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);

	vector<char> buf(len);
	fread(&buf[0], 1, len, f);
	fclose(f);
	return buf;
}

void recThread(void* param){
	TaskPool* pool = (TaskPool*)param;
	vector<char> imd = readAtFile(RootDir "samples/00W0_27c86a8b9ce8d0b1fe1d3d47b4040a28.png");

	for(int i = 0; i < 1000; ++i){
		int labels[4];
		float confs[4];
		SoftmaxResult* val = predictSoftmaxByTaskPool(pool, &imd[0], imd.size(), 1);
		getMultiLabel(val, labels);
		getMultiConf(val, confs);
		if (i % 50 == 0){
			printf("labels = %d, %d, %d, %d\n", labels[0], labels[1], labels[2], labels[3]);
			printf("confs = %f, %f, %f, %f\n", confs[0], confs[1], confs[2], confs[3]);
		}
		releaseSoftmaxResult(val);
	}
}

void main(int argc, char** argv){

#if 0
	Classifier c(RootDir "deploy.prototxt", RootDir "nin_iter_16000.caffemodel", 1.0, "", 0, 0, -1, 10);

	int num = argc > 1 ? atoi(argv[1]) : 1;
	Mat im = imread(RootDir "samples/00W0_27c86a8b9ce8d0b1fe1d3d47b4040a28.png");
	Mat im1 = imread(RootDir "samples/0FAW_a103991142caf37bfc7912c7cd2162b9.png");

	//��ͼ����
#if 0
	WPtr<SoftmaxResult> softmax = c.predictSoftmax(im, 1);
	int labels[4];
	float confs[4];
	getMultiLabel(softmax, labels);
	getMultiConf(softmax, confs);
	printf("labels = %d, %d, %d, %d\n", labels[0], labels[1], labels[2], labels[3]);
	printf("confs = %f, %f, %f, %f\n", confs[0], confs[1], confs[2], confs[3]);
#endif

	///���Զ�ͼ��������
#if 0
	vector<Mat> imgs;
	for (int i = 0; i <num; ++i){
		imgs.push_back(i % 2 == 0 ? im : im1);
	}

	double tck = cv::getTickCount();
	WPtr<MultiSoftmaxResult> softmax = c.predictSoftmax(imgs, 1);
	tck = (cv::getTickCount() - tck) / cv::getTickFrequency() * 1000.0;
	printf("��ʱ��%.2f ms\n", tck);
	for (int i = 0; i < softmax->count; ++i){
		int labels[4];
		float confs[4];
		SoftmaxResult* val = softmax->list[i];
		getMultiLabel(val, labels);
		getMultiConf(val, confs);
		printf("labels = %d, %d, %d, %d\n", labels[0], labels[1], labels[2], labels[3]);
		printf("confs = %f, %f, %f, %f\n", confs[0], confs[1], confs[2], confs[3]);
	}
#endif
#endif

	//���������
	TaskPool* pool = createTaskPool(RootDir "deploy.prototxt", RootDir "nin_iter_16000.caffemodel", 1.0, "", 0, 0, 0, 100);
	for (int i = 0; i < 8; ++i){
		_beginthread(recThread, 0, pool);
	}
	Sleep(100 * 1000);
	printf("ֹͣ...\n");
	releaseTaskPool(pool);
	printf("�Ѿ�ֹͣ...\n");
	Sleep(3000);
}