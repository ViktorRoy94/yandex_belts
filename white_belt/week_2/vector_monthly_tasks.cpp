#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main()
{
    int n = 0;
    cin >> n;
    
    int curr_month = 0;
    vector<int> days_in_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    vector<vector<string>> tasks(31);
    for (int iter=0; iter < n; iter++) {
        string op;
        cin >> op;
        if (op == "NEXT") {
            int days_in_this_month = days_in_month[curr_month % 12];
            int days_in_next_month = days_in_month[(curr_month+1) % 12];
            if (days_in_this_month > days_in_next_month) {
                for (int j=days_in_next_month; j < days_in_this_month; j++) {
                    tasks[days_in_next_month-1].insert(tasks[days_in_next_month-1].end(), tasks[j].begin(), tasks[j].end());
                    tasks[j].clear();
                }
            }
            else {
                for (int j=days_in_this_month; j < days_in_next_month; j++) {
                    tasks[j].clear();
                }
            }
            curr_month++;
        }
        else {
            int day_num = 0;
            cin >> day_num;
            
            day_num--;
            
            if (op == "ADD") {
                string task;
                cin >> task;
                tasks[day_num].push_back(task);
            }
            if (op == "DUMP") {
                cout << tasks[day_num].size() << " ";
                for (size_t j=0; j < tasks[day_num].size(); j++) {
                    cout <<  tasks[day_num][j] << " ";
                }
                cout << endl;
            }
        }
    }
}
