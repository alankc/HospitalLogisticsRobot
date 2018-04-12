/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DaoTask.h
 * Author: x
 *
 * Created on 11 de Abril de 2018, 13:40
 */

#ifndef DAOTASK_H
#define DAOTASK_H

#include "DaoGeneral.h"
#include "../TasksPlanning/TaskPlanningDefines.h"

class DaoTask
{
public:
    DaoTask(DaoGeneral* daoGeneral);
    DaoTask(const DaoTask& orig);
       
    bool UpdateStatus(uint32_t idTask, std::string status);
    bool UpdateStatusPlace(uint32_t idTask, uint32_t seqNumber, std::string status);   
    bool Delete(uint32_t idTask);
    
    std::vector<Task> GetPendingTasks();
    
    virtual ~DaoTask();
private:
    DaoGeneral* daoGenaral;
};

#endif /* DAOTASK_H */

