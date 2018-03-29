#ifndef TASKPLANNER_H
#define TASKPLANNER_H

#include <queue>
#include <map>

#include "TaskPlanningDefines.h"

typedef std::unordered_map<int, TaskData> TaskHash;
typedef std::vector<std::pair<double, int>> TaskQueue;
//typedef std::multimap<double, int, std::less<double>> TaskQueue;

class TaskPlanner
{
public:
     
    TaskPlanner(int8_t* map, uint32_t mapWidth, uint32_t mapHeight);
    virtual ~TaskPlanner();
    
    void SetCurrentPosition(VertexPosition currentPosition);
    
    void Add(Task t);
    void Remove(int id);
    Task Pop();
    
    bool GetCurrentTask(Task& t);
    bool GetCurrentPlace(VertexPosition& vP);
    void PopCurrentPlace();
    bool TaskHasChanged();

private:
    int8_t* map;
    uint32_t mapWidth;
    uint32_t mapHeight;

    TaskHash taskHash;
    TaskQueue taskQueue;
    bool taskHashHasChanged;
    
    Task currentTask;
    VertexPosition currentPosition;
    bool positionChanged;

    void SortTasks();
    void ComputeTotalCost(TaskData& t, VertexPosition initialPosition);
    void static ComputeDistance(int8_t* map, uint32_t mapWidth, uint32_t mapHeight, VertexPosition start, VertexPosition goal, double& cost);
};

#endif /* TASKPLANNER_H */

