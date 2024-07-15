#include <iostream>
#include <algorithm>
#include <vector>
#include<random>
#include<chrono>
using namespace std;
#define INT_MIN -1

class Queens
{
public:
    vector<int> queens, q_row, q_d1, q_d2, conflicts;
    int size;

    void calc_conflicts();
    void move(int q_index, int row);
    bool has_conflicts() const;
    void reinit();

public:
    Queens(int n);
    int get_max_conflict() const;
    int get_min_conflict(int q_index) const;
    int get_d1_q(int row, int col) const;
    int get_d2_q(int row, int col) const;

    void print() const;
    void solve(); 
};

void Queens::calc_conflicts()
{
    for (int i = 0; i < size; i++)
    {
        conflicts[i] = q_row[queens[i]] + get_d1_q(queens[i],i) + get_d2_q(queens[i],i) - 3;
    }
} 

void Queens::move(int q_index, int row)
{  
    q_row[queens[q_index]]--;
    q_row[row]++;
    q_d1[q_index - queens[q_index] + size - 1]--;
    q_d1[q_index - row + size - 1]++;
    q_d2[q_index + queens[q_index]]--;
    q_d2[q_index + row]++;
    queens[q_index] = row;

    calc_conflicts();
}

bool Queens::has_conflicts() const
{
    for (int i = 0; i < size; i++)
    {
        if (conflicts[i] != 0)
            return true;
    }
    return false;
}

// reinit get true random helper - used: ChatGPT

std::vector<int> generateRandomPermutation(int n) {
    // Create a vector to store the permutation
    std::vector<int> permutation(n);

    // Initialize the vector with values from 0 to n-1
    for (int i = 0; i < n; i++) {
        permutation[i] = i;
    }

    // Create a random number generator
    std::random_device rd;
    std::mt19937 rng(rd());

    // Perform Fisher-Yates shuffle
    for (int i = n - 1; i > 0; i--) {
        // Generate a random index between 0 and i
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(rng);

        // Swap the elements at indices i and j
        std::swap(permutation[i], permutation[j]);
    }

    return permutation;
}

void Queens::reinit()
{
    vector<int> half = generateRandomPermutation(size/2); // size == 9
    for (int i = 0; i < size/2; i++)                      // size / 2 = 4
        queens[i] = half[i]*2+1;                          // size + 1 / 2 = 5
                                                          // 
    half = generateRandomPermutation((size+1)/2);         // i - size + 1 / 2 = -1
    for (int i = size/2; i < size; i++)                      
        queens[i] = half[i-(size+1)/2 + (size % 2)]*2;

    //diag init
    for (int i = 0; i < size*2-1; i++)
    {
        q_d1[i] = 0;
        q_d2[i] = 0;
    }

    for (int i = 0; i < size; i++)
    {
        q_row[i] = 1;
        q_d1[i - queens[i] + size - 1]++;
        q_d2[queens[i] + i]++;
    }
    calc_conflicts();
}

Queens::Queens(int n) : size(n), q_row(n, 1), q_d1(n * 2 - 1, 0), q_d2(n * 2 - 1, 0), conflicts(n,0)
{
    for (int i = 0; i < size / 2; i++)
        queens.push_back(i * 2 + 1);
    for (int i = 0; i < (size + 1) / 2; i++)
        queens.push_back(i * 2);

    //diag init
    for (int i = 0; i < size; i++)
    {
        q_d1[i - queens[i] + size - 1]++;
        q_d2[queens[i] + i]++;
    }
    calc_conflicts();
}

int Queens::get_max_conflict() const
{
    int max_index = 0, max_value = INT_MIN;
    
    for (int i = 0; i < size; i++)
    {
        if (conflicts[i] > max_value)
        {
            max_value = conflicts[i];
            max_index = i;
        }
    }
    
    if (max_value == 0)
        return -1;

    vector<int> max_conflicts_indexes(size);
    int conf_count = 0;

    for (int i = 0; i < size; i++)
        if (conflicts[i] == max_value)
            max_conflicts_indexes[conf_count++] = i;
    
    int random = rand() % conf_count;
    return max_conflicts_indexes[random];
}

int Queens::get_min_conflict(int q_index) const
{
    int min_conflicts = conflicts[q_index], conf_count = 0;
    vector<int> min_conflicts_col(size);
    min_conflicts_col[conf_count++] = q_index;
    
    for (int i = 0; i < size; i++)
    {
        int conflicts = q_row[i] + get_d1_q(i, q_index) + get_d2_q(i, q_index);
        
        if (conflicts < min_conflicts)
        {
            min_conflicts = conflicts;
            conf_count = 0;
            min_conflicts_col[conf_count++] = i;
        }
        else if (conflicts == min_conflicts)
        {
            min_conflicts_col[conf_count++] = i;
        }
    }

    int rand_i = rand() % conf_count;
    return min_conflicts_col[rand_i];
}

int Queens::get_d1_q(int row, int col) const
{
    return q_d1[col - row + size - 1];
}

int Queens::get_d2_q(int row, int col) const
{
    return q_d2[row + col];
}

void Queens::print() const
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (queens[j] == i)
                printf("Q");
            else
                printf("_");
        }
        printf("\n");
    }
}


void Queens::solve()
{
    if (size < 4)
    {
        if (size == 1)
        {
            cout << "[0]";
            return;
        }
        cout << -1;
        return;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
  
    while (true)
    {
        int iter = 0;
        while (iter++ < size)
        {
            int col = get_max_conflict();
            if (col == -1)
            {
                auto end = std::chrono::high_resolution_clock::now();
                if (size > 100)
                {
                    std::chrono::duration<double> duration = end - start;
                    double dur_format = duration.count() * 100;
                    int cutting = dur_format;
                    dur_format = cutting / 100.0;
                    cout << dur_format;
                }
                else
                    print();

                return;
            }
            int row = get_min_conflict(col);
            move(col, row);
        }
        if (!has_conflicts())
        {
            auto end = std::chrono::high_resolution_clock::now();
            if (size > 100)
            {
                std::chrono::duration<double> duration = end - start;
                double dur_format = duration.count()*100;
                int cutting = dur_format;
                dur_format = cutting / 100.0;
                cout << dur_format;
            }
            else
                print();
            return;
        }
        reinit();
        cout << "reinit" << endl;
    }
}

int main()
{
    int n;
    cin >> n;   
    Queens test(n);
    test.solve();
}
