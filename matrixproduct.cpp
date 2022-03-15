#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <papi.h>

using namespace std;

#define SYSTEMTIME clock_t

double OnMult(int m_ar, int m_br)
{

	SYSTEMTIME Time1, Time2;

	char st[100];
	double temp;
	int i, j, k;

	double *pha, *phb, *phc;

	pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

	for (i = 0; i < m_ar; i++)
		for (j = 0; j < m_ar; j++)
			pha[i * m_ar + j] = (double)1.0;

	for (i = 0; i < m_br; i++)
		for (j = 0; j < m_br; j++)
			phb[i * m_br + j] = (double)(i + 1);

	Time1 = clock();

	for (i = 0; i < m_ar; i++)
	{
		for (j = 0; j < m_br; j++)
		{
			temp = 0;
			for (k = 0; k < m_ar; k++)
			{
				temp += pha[i * m_ar + k] * phb[k * m_br + j];
			}
			phc[i * m_ar + j] = temp;
		}
	}

	Time2 = clock();
	sprintf(st, "Time: %3.3f seconds\n", (double)(Time2 - Time1) / CLOCKS_PER_SEC);
	cout << st;

	// display 10 elements of the result matrix to verify correctness
	cout << "Result matrix: " << endl;
	for (i = 0; i < 1; i++)
	{
		for (j = 0; j < min(10, m_br); j++)
			cout << phc[j] << " ";
	}
	cout << endl;

	free(pha);
	free(phb);
	free(phc);

	return (double)(Time2 - Time1) / CLOCKS_PER_SEC;
}

double OnMultLine(int m_ar, int m_br)
{
	SYSTEMTIME Time1, Time2;

	char st[100];
	double temp;
	int i, j, k;

	double *pha, *phb, *phc;

	pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

	for (i = 0; i < m_ar; i++)
		for (j = 0; j < m_ar; j++)
			pha[i * m_ar + j] = (double)1.0;

	for (i = 0; i < m_br; i++)
		for (j = 0; j < m_br; j++)
			phb[i * m_br + j] = (double)(i + 1);

	for (i = 0; i < m_br; i++)
		for (j = 0; j < m_br; j++)
			phc[i * m_br + j] = (double)(0);

	Time1 = clock();

	for (i = 0; i < m_ar; i++)
	{
		for (k = 0; k < m_ar; k++)
		{
			for (j = 0; j < m_br; j++)
			{
				phc[i * m_ar + j] += pha[i * m_ar + k] * phb[k * m_br + j];
			}
		}
	}

	Time2 = clock();
	sprintf(st, "Time: %3.3f seconds\n", (double)(Time2 - Time1) / CLOCKS_PER_SEC);
	cout << st;

	cout << "Result matrix: " << endl;
	for (i = 0; i < 1; i++)
	{
		for (j = 0; j < min(10, m_br); j++)
			cout << phc[j] << " ";
	}
	cout << endl;

	free(pha);
	free(phb);
	free(phc);

	return (double)(Time2 - Time1) / CLOCKS_PER_SEC;
}

double OnMultBlock(int m_ar, int m_br, int blk_sizes)
{
	SYSTEMTIME Time1, Time2;

	char st[100];
	double temp;
	int i, j, k;

	double *pha, *phb, *phc;

	pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

	for (i = 0; i < m_ar; i++)
		for (j = 0; j < m_ar; j++)
			pha[i * m_ar + j] = (double)1.0;

	for (i = 0; i < m_br; i++)
		for (j = 0; j < m_br; j++)
			phb[i * m_br + j] = (double)(i + 1);

	for (i = 0; i < m_br; i++)
		for (j = 0; j < m_br; j++)
			phc[i * m_br + j] = (double)(0);

	Time1 = clock();
	for (i = 0; i < m_ar; i += blk_sizes)
	{
		for (j = 0; j < m_br; j += blk_sizes)
		{
			for (int blk_row = i; blk_row < i + blk_sizes; blk_row++)
			{
				for (int blk_col = j; blk_col < j + blk_sizes; blk_col++)
				{

					for (k = 0; k < m_ar; k++)
					{
						phc[blk_row * m_ar + blk_col] += pha[blk_row * m_ar + k] * phb[k * m_br + blk_col];
					}
				}
			}
		}
	}
	Time2 = clock();
	sprintf(st, "Time: %3.3f seconds\n", (double)(Time2 - Time1) / CLOCKS_PER_SEC);
	cout << st;

	cout << "Result matrix: " << endl;
	for (i = 0; i < 1; i++)
	{
		for (j = 0; j < min(10, m_br); j++)
			cout << phc[j] << " ";
	}
	cout << endl;

	free(pha);
	free(phb);
	free(phc);

	return (double)(Time2 - Time1) / CLOCKS_PER_SEC;
}

float produtoInterno(float *v1, float *v2, int col)
{
	int i;
	float soma = 0.0;

	for (i = 0; i < col; i++)
		soma += v1[i] * v2[i];

	return (soma);
}

void handle_error(int retval)
{
	printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
	exit(1);
}

void init_papi()
{
	int retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT && retval < 0)
	{
		printf("PAPI library version mismatch!\n");
		exit(1);
	}
	if (retval < 0)
		handle_error(retval);

	std::cout << "PAPI Version Number: MAJOR: " << PAPI_VERSION_MAJOR(retval)
			  << " MINOR: " << PAPI_VERSION_MINOR(retval)
			  << " REVISION: " << PAPI_VERSION_REVISION(retval) << "\n";
}

int setupPAPI(int &EventSet)
{
	int ret = PAPI_library_init(PAPI_VER_CURRENT);
	if (ret != PAPI_VER_CURRENT)
		std::cout << "FAIL" << endl;

	ret = PAPI_create_eventset(&EventSet);
	if (ret != PAPI_OK)
		cout << "ERROR: create eventset" << endl;

	ret = PAPI_add_event(EventSet, PAPI_L1_DCM);
	if (ret != PAPI_OK)
		cout << "ERROR: PAPI_L1_DCM" << endl;

	ret = PAPI_add_event(EventSet, PAPI_L2_DCM);
	if (ret != PAPI_OK)
		cout << "ERROR: PAPI_L2_DCM" << endl;

	return ret;
}

void multTest(int line, int end, int step, fstream &logfile)
{
	double time;

	int EventSet = PAPI_NULL;
	long long values[2];
	// TODO: Test if this logging is working
	int ret = setupPAPI(EventSet);

	logfile << "Normal multiplicatiom: from " << line << " to " << end << " step:" << step << endl;
	logfile << "lineCol"
			<< " "
			<< "time"
			<< " "
			<< "L1_DCM"
			<< " "
			<< "L2_DCM" << endl;

	for (int lineCol = line; lineCol <= end; lineCol += step)
	{
		time = OnMult(lineCol, lineCol);

		ret = PAPI_stop(EventSet, values);
		if (ret != PAPI_OK)
			cout << "ERROR: Stop PAPI" << endl;
		printf("L1 DCM: %lld \n", values[0]);
		printf("L2 DCM: %lld \n", values[1]);

		ret = PAPI_reset(EventSet);
		if (ret != PAPI_OK)
			std::cout << "FAIL reset" << endl;

		logfile << lineCol << " " << time << " " << values[0] << " " << values[1] << endl;
	}
}

void multLineTest(int line, int end, int step, fstream &logfile)
{
	double time;

	int EventSet = PAPI_NULL;
	long long values[2];
	// TODO: Test if this logging is working
	int ret = setupPAPI(EventSet);

	logfile << "Line Multiplication: from " << line << " to " << end << " step:" << step << endl;
	logfile << "lineCol"
			<< " "
			<< "time"
			<< " "
			<< "L1_DCM"
			<< " "
			<< "L2_DCM" << endl;

	for (int lineCol = line; lineCol <= end; lineCol += step)
	{
		time = OnMultLine(lineCol, lineCol);

		ret = PAPI_stop(EventSet, values);
		if (ret != PAPI_OK)
			cout << "ERROR: Stop PAPI" << endl;
		printf("L1 DCM: %lld \n", values[0]);
		printf("L2 DCM: %lld \n", values[1]);

		ret = PAPI_reset(EventSet);
		if (ret != PAPI_OK)
			std::cout << "FAIL reset" << endl;

		logfile << lineCol << " " << time << " " << values[0] << " " << values[1] << endl;
	}
}

int main(int argc, char *argv[])
{

	char c;
	int lin, col, nt = 1;
	int op;
	int blockSize;

	int EventSet = PAPI_NULL;
	long long values[2];
	int ret;

	fstream logfile;
	logfile.open("logfile.txt", ifstream::out);

	ret = PAPI_library_init(PAPI_VER_CURRENT);
	if (ret != PAPI_VER_CURRENT)
		std::cout << "FAIL" << endl;

	ret = PAPI_create_eventset(&EventSet);
	if (ret != PAPI_OK)
		cout << "ERROR: create eventset" << endl;

	ret = PAPI_add_event(EventSet, PAPI_L1_DCM);
	if (ret != PAPI_OK)
		cout << "ERROR: PAPI_L1_DCM" << endl;

	ret = PAPI_add_event(EventSet, PAPI_L2_DCM);
	if (ret != PAPI_OK)
		cout << "ERROR: PAPI_L2_DCM" << endl;

	op = 1;
	do
	{
		cout << endl
			 << "1. Multiplication" << endl;
		cout << "2. Line Multiplication" << endl;
		cout << "3. Block" << endl;
		cout << "4. Multiplication test 600 -> 3000, step: 400" << endl;
		cout << "5. Line Multiplication test 600 -> 3000, step: 400" << endl;
		cout << "6. Line Multiplication test 4096 -> 10240, step: 2048" << endl;
		cout << "9. All rests" << endl;
		cout << "Selection?: ";
		cin >> op;
		if (op == 0)
			break;
		if (op == 1 || op == 2 || op == 3)
		{
			printf("Dimensions: lins=cols ? ");
			cin >> lin;
			col = lin;
		}

		// Start counting
		ret = PAPI_start(EventSet);
		if (ret != PAPI_OK)
			cout << "ERROR: Start PAPI" << endl;

		switch (op)
		{
		case 1:
			OnMult(lin, col);
			break;
		case 2:
			OnMultLine(lin, col);
			break;
		case 3:
			cout << "Block Size?" ;
			cin >> blockSize;
			OnMultBlock(lin, col, blockSize);
			break;
		case 4:
			multTest(600, 3000, 400, logfile);
			break;
		case 5:
			multLineTest(600, 3000, 400, logfile);
			break;
		case 6:
			multLineTest(2048, 10240, 2048, logfile);
			break;
		case 9:
			multTest(600, 3000, 400, logfile);
			multLineTest(600, 3000, 400, logfile);
			multLineTest(2048, 10240, 2048, logfile);
			break;
		}

		ret = PAPI_stop(EventSet, values);
		if (ret != PAPI_OK)
			cout << "ERROR: Stop PAPI" << endl;
		printf("L1 DCM: %lld \n", values[0]);
		printf("L2 DCM: %lld \n", values[1]);

		ret = PAPI_reset(EventSet);
		if (ret != PAPI_OK)
			std::cout << "FAIL reset" << endl;

	} while (op != 0);

	ret = PAPI_remove_event(EventSet, PAPI_L1_DCM);
	if (ret != PAPI_OK)
		std::cout << "FAIL remove event" << endl;

	ret = PAPI_remove_event(EventSet, PAPI_L2_DCM);
	if (ret != PAPI_OK)
		std::cout << "FAIL remove event" << endl;

	ret = PAPI_destroy_eventset(&EventSet);
	if (ret != PAPI_OK)
		std::cout << "FAIL destroy" << endl;
}
