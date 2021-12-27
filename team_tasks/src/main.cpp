#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>


using namespace std;
// Перечислимый тип для статуса задачи
enum class TaskStatus {
  NEW,          // новая
  IN_PROGRESS,  // в разработке
  TESTING,      // на тестировании
  DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:
  // Получить статистику по статусам задач конкретного разработчика
  const TasksInfo& GetPersonTasksInfo(const string& person) const {
    if (tasks_.count(person))
      return tasks_.at(person);
    else
      throw runtime_error("Person " + person + " does not exist in database!");
  }

  // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
  void AddNewTask(const string& person) {
    tasks_[person][TaskStatus::NEW]++;
  }

  // Обновить статусы по данному количеству задач конкретного разработчика,
  // подробности см. ниже
  tuple<TasksInfo, TasksInfo> PerformPersonTasks(
      const string& person, int task_count) {

    TasksInfo updated_tasks, not_updated_tasks;
    TasksInfo& person_tasks = tasks_[person];

    for (auto ts = TaskStatus::NEW; ts != TaskStatus::DONE;
         ts = TaskStatus(static_cast<int>(ts) + 1)) {
      updated_tasks[TaskStatus(static_cast<int>(ts) + 1)] = min(task_count, person_tasks[ts]);
      task_count -= updated_tasks[TaskStatus(static_cast<int>(ts) + 1)];
    }

    for (auto ts = TaskStatus::NEW; ts != TaskStatus::DONE;
         ts = TaskStatus(static_cast<int>(ts) + 1)) {
      not_updated_tasks[ts] = person_tasks[ts] - updated_tasks[TaskStatus(static_cast<int>(ts) + 1)];
      person_tasks[ts] += updated_tasks[ts] - updated_tasks[TaskStatus(static_cast<int>(ts) + 1)];
    }
    person_tasks[TaskStatus::DONE] += updated_tasks[TaskStatus::DONE];

    RemoveZeros(updated_tasks);
    RemoveZeros(not_updated_tasks);
    RemoveZeros(person_tasks);

    return {updated_tasks, not_updated_tasks};
  }

private:
  unordered_map<string, TasksInfo> tasks_;

  void RemoveZeros(TasksInfo& info) {
    vector<TaskStatus> task_to_delete;
    for (const auto& item : info) {
      if (item.second == 0)
        task_to_delete.push_back(item.first);
    }
    for (auto ts : task_to_delete)
      info.erase(ts);
  }
};


// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
  cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
      ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
      ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
      ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
  TeamTasks tasks;
  tasks.AddNewTask("Ilia");
  for (int i = 0; i < 3; ++i) {
    tasks.AddNewTask("Ivan");
  }
  cout << "Ilia's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
  cout << "Ivan's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

  TasksInfo updated_tasks, untouched_tasks;

  tie(updated_tasks, untouched_tasks) =
      tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

  tie(updated_tasks, untouched_tasks) =
      tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

  return 0;
}
