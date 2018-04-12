#include "TaskPlanner.h"
#include "../PathPlanning/DStartLite.h"
#include <thread>
#include <iostream>
#include <set>

TaskPlanner::TaskPlanner(int8_t* map, uint32_t mapWidth, uint32_t mapHeight)
{
    this->map = map;
    this->mapWidth = mapWidth;
    this->mapHeight = mapHeight;
}

TaskPlanner::~TaskPlanner()
{
}

void TaskPlanner::SetCurrentPosition(VertexPosition currentPosition)
{
    this->currentPosition = currentPosition;
    positionChanged = true;
}

void TaskPlanner::Add(Task t)
{
    if (t.id >= 0)
    {
        if (taskHash.find(t.id) == taskHash.end())
        {
            TaskData& tD = taskHash[t.id];
            tD.locals = t.locals;
            tD.priority = t.priority;
            taskHashHasChanged = true;
        }
    }
}

void TaskPlanner::Remove(int id)
{
    if (id >= 0)
    {
        auto it = taskHash.find(id);
        if (it != taskHash.end())
        {
            taskHash.erase(it);
            taskHashHasChanged = true;
        }
    }
}

bool TaskPlanner::GetCurrentTask(Task & t)
{
    if (taskHash.size() > 0)
    {
        TaskData& newTask = taskHash[taskQueue.begin()->second];
        currentTask.id = taskQueue.begin()->second;
        currentTask.priority = newTask.priority;
        currentTask.cost = taskQueue.begin()->first;
        currentTask.locals = newTask.locals;
        t = currentTask;
        return true;
    }
    return false;
}

bool TaskPlanner::TaskHasChanged()
{
    if (taskHash.size() > 0)
    {
        if (currentTask.id != taskQueue.begin()->second)
            return true;

        return false;
    }
    return false;
}

bool TaskPlanner::GetCurrentPlace(VertexPosition & vP)
{
    if (taskHash.size() == 0)
        return false;

    if (taskHashHasChanged || positionChanged)
    {
        SortTasks();
        taskHashHasChanged = false;
        positionChanged = false;
    }

    TaskData* tD = &taskHash[taskQueue.begin()->second];

    vP = tD->locals.front();

    return true;
}

void TaskPlanner::PopCurrentPlace()
{
    if (taskHash.size() > 0)
    {
        TaskData* tD = &taskHash[taskQueue.begin()->second];
        currentPosition = tD->locals.front();
        tD->locals.erase(tD->locals.begin());

        if (tD->locals.size() == 0)
        {
            taskHash.erase(taskQueue.begin()->second);
            taskQueue.erase(taskQueue.begin());
        }
    }
}

void TaskPlanner::SortTasks()
{
    failedTasks.clear();
    std::set<int> taskNotAdded;
    for (auto i : taskHash)
        taskNotAdded.insert(i.first);

    VertexPosition last = currentPosition;

    taskQueue.clear();
    while (!taskNotAdded.empty())
    {
        double minCost = INFINITY_CONST;
        int minId;

        for (auto i : taskNotAdded)
        {
            TaskData& t = taskHash[i];
            ComputeTotalCost(t, last);
            if (t.cost < minCost)
            {
                minCost = t.cost;
                minId = i;
            }
        }

        if (minCost != INFINITY_CONST)
        {
            last = taskHash[minId].locals.back();
            taskQueue.push_back(std::make_pair(minCost, minId));
        }
        else
        {
            Task tmp;
            tmp.id = minId;
            tmp.locals = taskHash[minId].locals;
            tmp.priority = taskHash[minId].priority;
            tmp.cost = INFINITY_CONST;
            failedTasks.push_back(tmp);
        }
        taskNotAdded.erase(minId);
    }
}

void TaskPlanner::ComputeTotalCost(TaskData& t, VertexPosition initialPosition)
{
    //When I have to compute all
    if (t.costs.size() == 0)
    {
        std::thread th[t.locals.size()];
        double distances[t.locals.size()];

        VertexPosition last = initialPosition;
        for (int i = 0; i < t.locals.size(); i++)
        {
            th[i] = std::thread(ComputeDistance, map, mapWidth, mapHeight, last, t.locals[i], std::ref(distances[i]));
            last = t.locals[i];
        }

        t.costs.clear();
        for (int i = 0; i < t.locals.size(); i++)
        {
            th[i].join();
            t.costs.push_back(distances[i]);
        }
    }
    else //When i have to update just the first step
    {
        while (t.costs.size() > t.locals.size()) //When the position was reached
            t.costs.erase(t.costs.begin());
        ComputeDistance(map, mapWidth, mapHeight, initialPosition, t.locals.at(0), std::ref(t.costs.at(0)));
    }

    t.cost = 0;
    for (auto i : t.costs)
        t.cost += i / (double) t.priority;

}

void TaskPlanner::ComputeDistance(int8_t* map, uint32_t mapWidth, uint32_t mapHeight, VertexPosition start, VertexPosition goal, double& cost)
{
    DStartLite d;
    d.MountTheMap(map, mapWidth, mapHeight);
    d.Initialize(start, goal);
    cost = d.TotalCost();
}

FailedTasks TaskPlanner::GetFailedTasks()
{
    return failedTasks;
}