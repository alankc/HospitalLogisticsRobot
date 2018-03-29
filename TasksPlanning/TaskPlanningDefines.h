#ifndef TASKPLANNINGDEFINES_H
#define TASKPLANNINGDEFINES_H

#include "../PathPlanning/PathPlanningDefines.h"
#include <vector>

struct Task
{
    int id;
    std::vector<VertexPosition> locals;
    int priority;
    double cost;

    Task() : id(-1), priority(-1), cost(-1)
    {
    }

    bool operator==(const Task &t) const
    {
        return id == t.id;
    }
};

struct TaskData
{
    std::vector<VertexPosition> locals;
    std::vector<double> costs;

    int priority;
    double cost;

    TaskData() : priority(-1), cost(-1)
    {
    }
};

#endif /* TASKPLANNINGDEFINES_H */

