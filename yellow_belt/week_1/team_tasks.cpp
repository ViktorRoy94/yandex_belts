#include <iostream>
#include <tuple>
#include <string>
#include <map>

#ifdef DEBUG
enum class TaskStatus {
    NEW,
    IN_PROGRESS,
    TESTING,
    DONE
};

using TasksInfo = std::map<TaskStatus, int>;
#endif

class TeamTasks {
public:
    const TasksInfo& GetPersonTasksInfo(const std::string& person) const {
        return data.at(person);
    }

    // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
    void AddNewTask(const std::string& person) {
        data[person][TaskStatus::NEW]++;
    }

    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже
    std::tuple<TasksInfo, TasksInfo> PerformPersonTasks(
            const std::string& person, int task_count) {
        if (data.count(person) == 0) {
            return {TasksInfo(), TasksInfo()};
        }

        TasksInfo& person_tasks = data[person];
        TasksInfo updated_tasks;
        TasksInfo old_tasks;
        TasksInfo new_person_tasks;
        for (const auto& it : person_tasks) {
            TaskStatus status = it.first;
            int count = it.second;
            if (status != TaskStatus::DONE) {
                TaskStatus next_status = TaskStatus(static_cast<int>(status) + 1);
//                updated_tasks[next_status] = std::min(task_count, count);
//                task_count -= updated_tasks[next_status];
//                if (task_count < 0) {
//                    old_tasks[status] = count;
//                }

                if (task_count - count < 0) {
                    count -= task_count;
                    old_tasks[status] = count;
                    if (task_count != 0) {
                        updated_tasks[next_status] = task_count;
                        task_count = 0;
                    }
                }
                else {
                    updated_tasks[next_status] = count;
                    task_count -= count;
                    count = 0;
                }
            }
            else {
                new_person_tasks[status] += count;
            }
        }

        for (const auto& it : old_tasks) {
            new_person_tasks[it.first] += it.second;
        }
        for (const auto& it : updated_tasks) {
            new_person_tasks[it.first] += it.second;
        }

        person_tasks = new_person_tasks;
        return {updated_tasks, old_tasks};
    }

private:
    std::map<std::string, TasksInfo> data;
};

#ifdef DEBUG
void PrintTasksInfo(TasksInfo tasks_info) {
    for (const auto& it : tasks_info) {
        std::cout << tasks_info[it.first] << " ";
    }
    std::cout << std::endl;
}

int main() {
    {
        TeamTasks tasks;
        for (int i = 0; i < 5; ++i) {
            tasks.AddNewTask("Alice");
        }
        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        TasksInfo updated_tasks, untouched_tasks;

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Alice", 5);
//        std::cout << "Updated Alice's tasks: ";
//        PrintTasksInfo(updated_tasks);
//        std::cout << "Untouched Alice's tasks: ";
//        PrintTasksInfo(untouched_tasks);

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Alice", 5);
//        std::cout << "Updated Alice's tasks: ";
//        PrintTasksInfo(updated_tasks);
//        std::cout << "Untouched Alice's tasks: ";
//        PrintTasksInfo(untouched_tasks);

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Alice", 1);
//        std::cout << "Updated Alice's tasks: ";
//        PrintTasksInfo(updated_tasks);
//        std::cout << "Untouched Alice's tasks: ";
//        PrintTasksInfo(untouched_tasks);

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        for (int i = 0; i < 5; ++i) {
            tasks.AddNewTask("Alice");
        }

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Alice", 2);
        std::cout << "Updated Alice's tasks: ";
        PrintTasksInfo(updated_tasks);
        std::cout << "Untouched Alice's tasks: ";
        PrintTasksInfo(untouched_tasks);

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Alice", 1);
        std::cout << "Updated Alice's tasks: ";
        PrintTasksInfo(updated_tasks);
        std::cout << "Untouched Alice's tasks: ";
        PrintTasksInfo(untouched_tasks);

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Alice", 4);
        std::cout << "Updated Alice's tasks: ";
        PrintTasksInfo(updated_tasks);
        std::cout << "Untouched Alice's tasks: ";
        PrintTasksInfo(untouched_tasks);

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Alice",   1);
        std::cout << "Updated Alice's tasks: ";
        PrintTasksInfo(updated_tasks);
        std::cout << "Untouched Alice's tasks: ";
        PrintTasksInfo(untouched_tasks);

        std::cout << "Alice's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));
        std::cout << std::endl;
    }

    {
        TeamTasks tasks;
        tasks.AddNewTask("Ilia");
        for (int i = 0; i < 3; ++i) {
            tasks.AddNewTask("Ivan");
        }
        std::cout << "Ilia's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
        std::cout << "Ivan's tasks: ";
        PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

        TasksInfo updated_tasks, untouched_tasks;

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Ivan", 2);
        std::cout << "Updated Ivan's tasks: ";
        PrintTasksInfo(updated_tasks);
        std::cout << "Untouched Ivan's tasks: ";
        PrintTasksInfo(untouched_tasks);

        std::tie(updated_tasks, untouched_tasks) =
                tasks.PerformPersonTasks("Ivan", 2);
        std::cout << "Updated Ivan's tasks: ";
        PrintTasksInfo(updated_tasks);
        std::cout << "Untouched Ivan's tasks: ";
        PrintTasksInfo(untouched_tasks);
    }
    return 0;
}
#endif
