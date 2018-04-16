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

DaoTask::DaoTask(DaoGeneral* daoGeneral, uint32_t idMap) : daoGenaral(daoGeneral), idMap(idMap)
{
}

DaoTask::~DaoTask()
{
}

const std::string DaoTask::REMOVE = "R";
const std::string DaoTask::WAITING = "W";
const std::string DaoTask::OPENNED = "O";
const std::string DaoTask::PERFORMING = "P";
const std::string DaoTask::DONE = "D";

bool DaoTask::UpdateStatus(int idTask, std::string status)
{
    std::stringstream statement;
    statement << "UPDATE TASK\n";
    statement << "SET STATUS = '" << status << "'\n";
    statement << "WHERE ID_TASK = " << idTask << ";";

    if (daoGenaral->ExecuteUpdate(statement.str()))
        return true;

    return false;
}

bool DaoTask::UpdateStatusPlace(int idTask, uint32_t seqNumber, std::string status)
{
    std::stringstream statement;
    statement << "UPDATE TASK_PLACE\n";
    statement << "SET STATUS = '" << status << "'\n";
    statement << "WHERE ((ID_TASK = " << idTask << ") AND (SEQ_NUMBER = " << seqNumber << "));";

    if (daoGenaral->ExecuteUpdate(statement.str()))
        return true;

    return false;
}

bool DaoTask::Delete(int idTask)
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
    statement << "WHERE STATUS = '" << WAITING << "';";

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

    int factor = 1;
    statement.str("");
    statement << "SELECT O_PIX_PER_METER, R_PIX_PER_METER FROM MAP\n";
    statement << "WHERE ID_MAP = " << idMap << ";";
    res = daoGenaral->ExecuteQuery(statement.str());
    if (res != NULL)
    {
        res->next();
        factor = res->getUInt("O_PIX_PER_METER") / res->getUInt("R_PIX_PER_METER");
        delete res;
    }

    for (int i = 0; i < tasks.size(); i++)
    {
        statement.str("");
        statement << "SELECT TP.SEQ_NUMBER, P.DESC, P.X, P.Y FROM PLACE AS P\n";
        statement << "INNER JOIN TASK_PLACE AS TP ON P.ID_PLACE = TP.ID_PLACE\n";
        statement << "WHERE ((TP.ID_TASK = " << tasks[i].id << ") AND (TP.STATUS = '" << WAITING << "'))\n";
        statement << "ORDER BY TP.SEQ_NUMBER ASC;";

        res = daoGenaral->ExecuteQuery(statement.str());

        if (res != NULL)
        {
            while (res->next())
            {
                TaskPlace tmp;
                
                tmp.position.x = res->getUInt("X") / factor;
                tmp.position.y = res->getUInt("Y") / factor;
                tmp.description = res->getString("DESC");
                tmp.seqNumber = res->getUInt("SEQ_NUMBER");
      
                tasks[i].places.push_back(tmp);
            }

            delete res;
        }
    }

    return tasks;
}

std::vector<int> DaoTask::GetRemovedTasks()
{
    std::vector<int> tasks;

    std::stringstream statement;
    statement << "SELECT ID_TASK FROM TASK\n";
    statement << "WHERE STATUS = '" << REMOVE << "';";

    sql::ResultSet* res = daoGenaral->ExecuteQuery(statement.str());

    if (res != NULL)
    {
        while (res->next())
        {
            tasks.push_back(res->getInt("ID_TASK"));
        }
        delete res;
    }
    else
        return tasks;
}
