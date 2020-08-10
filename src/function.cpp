#include "../inc/function.h"

DataType::DataType(string fileName, int stage)
{
    /* let's count # lines first.*/
    row = 1;
    column = 0;
    string buffer;
    ifstream fileInput(fileName, ios::in);
    if (!fileInput)
    {
        cerr << "\nError opening file: " << fileName << endl;
    }

    while (fileInput.good())
    {
        getline(fileInput, buffer, ',');
        column++;
        // cout << buffer << endl;
        if (buffer.find("\n") < UINT32_MAX)
        {
            column++;
            row++;
            // cout << column << endl;
        }
    }

    column /= row;
    /* file's content size */
    // cout << "row : " << row << endl;
    // cout << "column : " << column << endl;

    //allocate memory for storing data
    content = new string[(row - 1) * 4];

    //re-read the file and this time read the useful data into allocated memory
    fileInput.seekg(0, fileInput.beg);
    getline(fileInput, buffer);
    for (size_t i = 1; i < row; i++)
    {
        for (size_t j = 0; j < column; j++)
        {

            if (!j)
            {
                getline(fileInput, content[(i - 1) * 4], ',');
            }
            else if (j == (pArea - 1))
            {
                getline(fileInput, content[(i - 1) * 4 + 1], ',');
            }
            else if (j == (pPCI - 1))
            {
                getline(fileInput, content[(i - 1) * 4 + 2]);
            }
            else
            {
                getline(fileInput, buffer, ',');
            }
        }
    }

    // for (size_t i = 0; i < (row - 1); i++)
    // {
    //     for (size_t j = 0; j < 3; j++)
    //     {
    //         cout << content[i * 3 + j] << " ";
    //     }
    //     cout << endl;
    // }
    fileInput.close();
    /* figure data size */
    row -= 1;
    column = 4;
    //cout << "original data : " << row << " records" << endl;
    //count how many data be used on different stage

    //put data into different stage
    int j = 0;
    int k = 0;
    for (size_t i = 0; i < row; i++)
    {
        //stage 1
        if (stage == 1 && atof(content[i * column + 2].c_str()) < 40)
        {
            content[j * column] = content[i * column];
            content[j * column + 1] = content[i * column + 1];
            content[j * column + 2] = content[i * column + 2];
            content[j * column + 3] = to_string((100 - atof(content[j * column + 2].c_str()) * PCI_decline) * (atof(content[j * column + 1].c_str())));
            j++;
        }
        //stage 2
        if (stage == 2 && atof(content[i * column + 2].c_str()) < 70 && atof(content[i * column + 2].c_str()) > 40)
        {
            content[k * column] = content[i * column];
            content[k * column + 1] = content[i * column + 1];
            content[k * column + 2] = content[i * column + 2];
            content[k * column + 3] = to_string((100 - atof(content[k * column + 2].c_str()) * PCI_decline) * (atof(content[k * column + 1].c_str())));
            k++;
        }
    }
    (stage == 1 ? row = j : row = k);
    //sort
    string temp[4];
    for (size_t i = 0; i < row - 1; i++)
    {
        for (size_t j = 0; j < row - 1; j++)
        {
            if (atof(content[j * column + 3].c_str()) < atof(content[(j + 1) * column + 3].c_str()))
            {
                temp[0] = content[j * column];
                temp[1] = content[j * column + 1];
                temp[2] = content[j * column + 2];
                temp[3] = content[j * column + 3];
                content[j * column] = content[(j + 1) * column];
                content[j * column + 1] = content[(j + 1) * column + 1];
                content[j * column + 2] = content[(j + 1) * column + 2];
                content[j * column + 3] = content[(j + 1) * column + 3];
                content[(j + 1) * column] = temp[0];
                content[(j + 1) * column + 1] = temp[1];
                content[(j + 1) * column + 2] = temp[2];
                content[(j + 1) * column + 3] = temp[3];
            }
        }
    }

    //cout << "pro-data: " << row << " records" << endl;

    //cout << "stage " << stage << endl;
    //for (size_t i = 0; i < row; i++)
    //{
    //	for (size_t j = 0; j < column; j++)
    //	{
    //		cout << content[i * column + j] << " ";
    //	}
    //	cout << endl;
    //}
}

PopulationType::PopulationType(string argv1, int stage) : DataType(argv1, stage)
{
    //cout << "binary value initilization" << endl;
    // srand(1);
    for (size_t i = 0; i < POPULATION_CNT; i++)
    {
        for (size_t j = 0; j < row; j++)
        {
            parent[i].binaryValue += to_string(BinaryRand());
            //(PCI'-PCI)* Area /(total Area) * binary
            parent[i].benefit += atof(content[j * column + 3].c_str()) * (parent[i].binaryValue[j] - '0');
            //cout << atof(content[j*column + 3].c_str()) * (parent[i].binaryValue[j] - '0') << endl;
            parent[i].cost += atof(content[j * column + 1].c_str()) * COST_PER_AREA * (parent[i].binaryValue[j] - '0');
            parent[i].fitnessValue = parent[i].benefit;
        }
        //cout << parent[i].binaryValue << " " << parent[i].benefit;
        //cout << endl;
    }

    for (size_t i = 0; i < POPULATION_CNT; i++)
    {
        pool[i] = parent[0];
        //cout << i << "\t" << endl;
    }

    for (size_t i = 0; i < row; i++)
    {
        BestOne.binaryValue += '0';
        //cout << i << "\t" << endl;
    }
}
void PopulationType::reproduction()
{
    //cout << "do reproduction" << endl;
    double sum = 0;
    for (size_t i = 0; i < POPULATION_CNT; i++)
    {
        sum += parent[i].fitnessValue;
    }

    parent[0].probability = parent[0].fitnessValue / sum;
    // cout << parent[0].probability << endl;
    for (size_t i = 1; i < POPULATION_CNT; i++)
    {
        parent[i].probability = parent[i - 1].probability + parent[i].fitnessValue / sum;
        // cout << parent[i].probability << " ~ " << parent[i].binaryValue << endl;
    }

    double r[POPULATION_CNT];
    for (size_t i = 0; i < POPULATION_CNT; i++)
    {
        r[i] = SRand();
    }

    for (size_t i = 0; i < POPULATION_CNT; i++)
    {
        for (size_t j = 0; j < POPULATION_CNT - 1; j++)
        {
            if (r[i] > parent[j].probability && r[i] < parent[j + 1].probability)
            {

                pool[i] = parent[j + 1];
            }
        }
        // cout << r[i] << " ~ " << pool[i].binaryValue << endl;
    }
}

void PopulationType::crossover()
{
    // cout << "do crossover" << endl;

    size_t t1, t2, p, i = 0;
    while (i < POPULATION_CNT)
    {
        t1 = rand() % POPULATION_CNT;
        do
        {
            t2 = rand() % POPULATION_CNT;
        } while (t1 == t2);
        // //cout << t1 << "\t" << t2 << endl;

        double r = SRand();
        if (r < CROSSOVER_RATE)
        {
            p = rand() % row;
            offspring[i].binaryValue = pool[t1].binaryValue.substr(0, p) + pool[t2].binaryValue.substr(p, row);
            offspring[i].cost = 0;
            offspring[i].benefit = 0;
            // cout << offspring[i].binaryValue << endl;
            i++;
        }
    }
}
void PopulationType::mutation()
{
    // cout << "do mutation" << endl;
    double mr;
    // #pragma omp parallel for schedule(static, 256)
    for (size_t i = 0; i < POPULATION_CNT; i++)
    {
        // #pragma omp parallel for schedule(static, 1)
        for (size_t j = 0; j < row; j++)
        {
            mr = SRand();
            if (mr < MUTATION_RATE / (row - j))
            {
                if (offspring[i].binaryValue[j] == '0')
                    offspring[i].binaryValue[j] = '1';
                else
                    offspring[i].binaryValue[j] = '0';
            }
        }
        //cout << "i = " << i << " :" << offspring[i].binaryValue << endl;
    }
}

void PopulationType::computeFitness(size_t iteration)
{
// cout << "compute fitness value" << endl;
#pragma omp parallel for schedule(static, 256)
    for (size_t i = 0; i < POPULATION_CNT; i++)
    {
        for (size_t j = 0; j < row; j++)
        {
            //(PCI'-PCI)* Area * binary
            offspring[i].benefit += atof(content[j * column + 3].c_str()) * (offspring[i].binaryValue[j] - '0');
            offspring[i].cost += atof(content[j * column + 1].c_str()) * 586.0 * (offspring[i].binaryValue[j] - '0');
        }
        offspring[i].fitnessValue = offspring[i].benefit;
    }
    //penalty
    double min = 0;
    PopulationType::ChromosomeType pBest;
    for (size_t i = 0; i < POPULATION_CNT; i++)
    {

        //punish for over budget
        if (offspring[i].cost > budget)
            offspring[i].fitnessValue -= pow(abs(offspring[i].cost - budget), ALPHA) * pow(BETA * iteration, GAMMA);

        //record local best one
        if (offspring[i].fitnessValue > pBest.fitnessValue && offspring[i].cost < budget)
            pBest = offspring[i];

        //record global best one
        if (offspring[i].fitnessValue > BestOne.fitnessValue && offspring[i].cost < budget)
            BestOne = offspring[i];

        //banchmark of tanslation
        if (offspring[i].fitnessValue < min)
            min = offspring[i].fitnessValue;

        offspring[i].fitnessValue -= min - 1;

        if (iteration > ITERA_CNT / 5 && i < POPULATION_CNT / 5)
            offspring[i] = BestOne;

        parent[i] = offspring[i];
    }
    // cout << pBest.fitnessValue << "," << BestOne.fitnessValue << endl;
}

void PopulationType::printBestOne()
{
    /*cout << "Best one: " << BestOne.binaryValue << endl;
	cout << "cost :" << BestOne.cost / 1000000.0 << " M" << endl;
	cout << "benefit :" << BestOne.benefit << endl;
	cout << "Number: ";*/
    bool flag = false;
    for (size_t i = 0; i < row; i++)
    {
        if (BestOne.binaryValue[i] == '1')
        {
            content[i * column + 2] = "100";
            if (!flag)
            {
                mustBeDone += content[i * column];
                flag = true;
            }
            else
            {
                mustBeDone += "," + content[i * column];
            }
        }
    }
    /*cout << mustBeDone << endl;
	cout << endl;*/
}
double sum[2] = {0};

void saveJSON(const PopulationType &p1, string fileName)
{
    ofstream fileOutput(fileName, ios::out);
    if (!fileOutput)
    {
        cerr << "\nError opening file: " << fileName;
    }
    else
    {
        sum[0] += p1.BestOne.cost;
        sum[1] += p1.BestOne.benefit;
        //fileOutput << "[" << endl;
        fileOutput << "\t{" << endl;
        fileOutput << "\t\t\"NUMBER\":[\"";
        fileOutput << p1.mustBeDone;
        // if (p2.mustBeDone != "")
        // {
        //     fileOutput << "," << p2.mustBeDone;
        // }
        fileOutput << "\"]," << endl;
        fileOutput << "\t\t\"COST\":" << sum[0] << "," << endl;
        fileOutput << "\t\t\"BENEFIT\":" << sum[1] << endl;
        // fileOutput << "\t\t\"COST\":" << p1.BestOne.cost << "," << endl;
        // fileOutput << "\t\t\"BENEFIT\":" << p1.BestOne.benefit << endl;
        fileOutput << "\t}" << endl;
        //fileOutput << "]" << endl;
    }
    fileOutput.close();
}