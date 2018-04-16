/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: x
 *
 * Created on 13 de Março de 2018, 14:34
 */

#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif

#include <libplayerc++/playerc++.h>
#include "Auxiliar/MapResizer.h"
#include "PathPlanning/DStartLite.h"
#include "TasksPlanning/TaskPlanner.h"
#include "DataBase/DaoGeneral.h"
#include "DataBase/DaoTask.h"

#include <thread>

bool notNear(player_point_2d_t current, player_point_2d_t dest)
{
    if (std::abs(current.px - dest.px) > 1 || std::abs(current.py - dest.py) > 1)
        return true;

    return false;
}

void VerifyAdd(DaoTask& daoTask, TaskPlanner& taskPlanner)
{
    while (true)
    {
        auto tasks = daoTask.GetPendingTasks();
        for (auto t : tasks)
        {
            daoTask.UpdateStatus(t.id, DaoTask::OPENNED);
            uint32_t i = 0;
            for (auto p : t.places)
            {
                daoTask.UpdateStatusPlace(t.id, i++, DaoTask::OPENNED);
            }
            taskPlanner.Add(t);
        }
        sleep(10);
    }
}

void VerifyRemove(DaoTask& daoTask, TaskPlanner& taskPlanner)
{
    while (true)
    {
        auto tasks = daoTask.GetRemovedTasks();
        for (auto t : tasks)
        {
            taskPlanner.Remove(t);
            daoTask.Delete(t);
        }
        sleep(10);
    }
}

int main(int argc, char** argv)
{
    DaoGeneral tst("tcp://127.0.0.1:3306", "robot_database", "root", "");
    DaoTask dbT(&tst, 0);

    std::cout << "Conecting ro robot" << std::endl;
    PlayerCc::PlayerClient r("localhost");
    PlayerCc::MapProxy m(&r, 0);
    PlayerCc::Position2dProxy p(&r, 2);
    r.Read();

    std::cout << "Reading Map" << std::endl;
    m.RequestMap();
    int8_t* aMap = new int8_t[m.GetHeight() * m.GetWidth()];
    m.GetMap(aMap);

    std::cout << "Map Resizing" << std::endl;
    MapResizer mR;
    mR.Resize(aMap, m.GetWidth(), m.GetHeight(), 37, 15);
    mR.SetBegin(-16, -29);

    std::cout << "Mount DStartLite" << std::endl;
    DStartLite pP;
    pP.MountTheMap(mR.GetResizedMap(), mR.GetResizedWidth(), mR.GetResizedHeigth());

    std::cout << "Init TaskPlanner" << std::endl;
    TaskPlanner tP(mR.GetResizedMap(), mR.GetResizedWidth(), mR.GetResizedHeigth());

    player_point_2d_t point;

    point.px = p.GetXPos();
    point.py = p.GetYPos();

    tP.SetCurrentPosition(mR.RealToResized(point));

    std::thread t_add(VerifyAdd, std::ref(dbT), std::ref(tP));
    std::thread t_remove(VerifyRemove, std::ref(dbT), std::ref(tP));

    VertexPosition current;
    VertexPosition next;
    TaskPlace place;
    while (true)
    {
        r.Read();
        Task t;
        while (tP.GetCurrentPlace(place))
        {
            next = place.position;
            if (tP.TaskHasChanged())
            {
                //When exists more than one task into the list
                if (t.id != -1)
                    dbT.UpdateStatus(t.id, DaoTask::DONE);
                tP.GetCurrentTask(t);
                dbT.UpdateStatus(t.id, DaoTask::PERFORMING);
                std::cout << "Task : " << t.id << std::endl;
                std::cout << "Desc.: " << t.description << std::endl;
            }

            std::cout << "Going to: " << place.description << std::endl;
            dbT.UpdateStatusPlace(t.id, place.seqNumber, DaoTask::PERFORMING);

            r.Read();

            point.px = p.GetXPos();
            point.py = p.GetYPos();
            current = mR.RealToResized(point);

            pP.Initialize(current, next);

            VertexPosition tmp;
            std::vector<VertexUpdate> up;
            while (pP.GetNext(tmp, up))
            {
                player_point_2d_t pointTmp;
                pointTmp = mR.ResizedToReal(tmp);
                p.GoTo(pointTmp.px, pointTmp.py, 0);

                r.Read();
                point.px = p.GetXPos();
                point.py = p.GetYPos();
                while (notNear(point, pointTmp))
                {
                    r.Read();
                    point.px = p.GetXPos();
                    point.py = p.GetYPos();
                }
            }

            tP.PopCurrentPlace();
            dbT.UpdateStatusPlace(t.id, place.seqNumber, DaoTask::DONE);

            //When exists just one task into the list
            if (!tP.GetCurrentPlace(place))
                dbT.UpdateStatus(t.id, DaoTask::DONE);

        }
        for (auto k : tP.GetFailedTasks())
        {
            std::cout << "F.ID.: " << k.id << std::endl;
        }
        sleep(1);
    }
    return EXIT_SUCCESS;
}

