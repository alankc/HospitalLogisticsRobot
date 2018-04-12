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

bool notNear(player_point_2d_t current, player_point_2d_t dest)
{
    if (std::abs(current.px - dest.px) > 1 || std::abs(current.py - dest.py) > 1)
        return true;

    return false;
}

int main(int argc, char** argv)
{
    DaoGeneral tst("tcp://127.0.0.1:3306", "robot_database", "root", "");
    DaoTask dbT(&tst);

    auto tasks = dbT.GetPendingTasks();
    for (auto t : tasks)
    {
        std::cout << " Identifier: " << t.id << std::endl;
        std::cout << "Description: " << t.description << std::endl;
        std::cout << "   Priority: " << t.priority << std::endl;
        std::cout << "     Places: " << t.locals.size() << " => ";
        for (auto l : t.locals)
        {
            std::cout << "(" << l.x << ", " << l.y << ") ";
        }

        std::cout << std::endl << std::endl;
    }

    /*std::cout << "Conecting" << std::endl;

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

    Task t1;
    t1.id = 0;
    point.px = 7;
    point.py = -11;
    t1.locals.push_back(mR.RealToResized(point));
    point.px = -14;
    point.py = 25;
    t1.locals.push_back(mR.RealToResized(point));
    t1.priority = 1;

    Task t2;
    t2.id = 1;
    point.px = -4;
    point.py = -11;
    t2.locals.push_back(mR.RealToResized(point));
    point.px = -13.5;
    point.py = -11;
    t2.locals.push_back(mR.RealToResized(point));
    t2.priority = 5;

    tP.Add(t1);
    tP.Add(t2);

    VertexPosition current;
    VertexPosition next;
    while (tP.GetCurrentPlace(next))
    {
        if (tP.TaskHasChanged())
        {
            Task t;
            tP.GetCurrentTask(t);
            std::cout << "Task: " << t.id << std::endl;
        }

        std::cout << "Going to: " << mR.ResizedToReal(next) << std::endl;

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
    }*/
    return EXIT_SUCCESS;
}

