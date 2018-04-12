/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DaoTask.cpp
 * Author: x
 * 
 * Created on 11 de Abril de 2018, 13:40
 */

#include "DaoTask.h"
#include <sstream>

DaoTask::DaoTask(DaoGeneral* daoGeneral) : daoGenaral(daoGeneral)
{
}

DaoTask::~DaoTask()
{
}

bool DaoTask::UpdateStatus(uint32_t idTask, std::string status)
{
    std::stringstream statement;
    statement << "UPDATE TASK\n";
    statement << "SET STATUS = '" << status << "'\n";
    statement << "WHERE ID_TASK = " << idTask << ";";

    if (daoGenaral->ExecuteUpdate(statement.str()))
        return true;

    return false;
}

bool DaoTask::UpdateStatusPlace(uint32_t idTask, uint32_t seqNumber, std::string status)
{
    std::stringstream statement;
    statement << "UPDATE TASK_PLACE\n";
    statement << "SET STATUS = '" << status << "'\n";
    statement << "WHERE ((ID_TASK = " << idTask << ") AND (SEQ_NUMBER = " << seqNumber << "));";

    if (daoGenaral->ExecuteUpdate(statement.str()))
        return true;

    return false;
}

bool DaoTask::Delete(uint32_t idTask)
{
    std::stringstream statement;
    statement << "DELETE FROM TASK\n";
    statement << "WHERE ID_TASK = " << idTask << ";";

    if (daoGenaral->ExecuteUpdate(statement.str()))
        return true;

    return false;
}

std::vector<Task> DaoTask::GetPendingTasks()
{
    std::vector<Task> tasks;

    std::stringstream statement;
    statement << "SELECT ID_TASK, DESCRIPTION, PRIORITY FROM TASK\n";
    statement << "WHERE STATUS = '" << "A" << "';";

    sql::ResultSet* res = daoGenaral->ExecuteQuery(statement.str());

    if (res != NULL)
    {

        while (res->next())
        {
            Task tmp;
            tmp.id = res->getInt("ID_TASK");
            tmp.description = res->getString("DESCRIPTION");
            tmp.priority = res->getInt("PRIORITY");
            tasks.push_back(tmp);
        }
        delete res;
    }
    else
        return tasks;

    for (int i = 0; i < tasks.size(); i++)
    {
        statement.str("");
        statement << "SELECT P.X, P.Y FROM PLACE AS P\n";
        statement << "INNER JOIN TASK_PLACE AS TP ON P.ID_PLACE = TP.ID_PLACE\n";
        statement << "WHERE ((TP.ID_TASK = " << tasks[i].id << ") AND (TP.STATUS = '" << "A" << "'))\n";
        statement << "ORDER BY TP.SEQ_NUMBER ASC;";

        res = daoGenaral->ExecuteQuery(statement.str());

        if (res != NULL)
        {
            while (res->next())
            {
                VertexPosition tmp;
                tmp.x = res->getUInt("X");
                tmp.y = res->getUInt("Y");
                tasks[i].locals.push_back(tmp);
            }

            delete res;
        }
    }

    return tasks;
}
