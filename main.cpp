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

bool notNear(player_point_2d_t current, player_point_2d_t dest)
{
    if (std::abs(current.px - dest.px) > 1 || std::abs(current.py - dest.py) > 1)
        return true;

    return false;
}

int main(int argc, char** argv)
{
    PlayerCc::PlayerClient r("localhost");
    PlayerCc::MapProxy m(&r, 0);
    PlayerCc::Position2dProxy p(&r, 2);

    r.Read();

    m.RequestMap();
    int8_t* aMap = new int8_t[m.GetHeight() * m.GetWidth()];
    m.GetMap(aMap);

    MapResizer mR;
    mR.Resize(aMap, m.GetWidth(), m.GetHeight(), 37, 10);

    TaskPlanner tP(mR.GetResizedMap(), mR.GetResizedWidth(), mR.GetResizedHeigth());

    player_point_2d_t point;

    point.px = p.GetXPos();
    point.py = p.GetYPos();

    tP.SetCurrentPosition(mR.RealToResized(point, -8, -8));

    Task t1;
    t1.id = 0;
    point.px = 7;
    point.py = -3;
    t1.locals.push_back(mR.RealToResized(point, -8, -8));
    point.px = 1.5;
    point.py = -6.5;
    t1.locals.push_back(mR.RealToResized(point, -8, -8));
    t1.priority = 1;

    Task t2;
    t2.id = 1;
    point.px = 2;
    point.py = 2;
    t2.locals.push_back(mR.RealToResized(point, -8, -8));
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

        std::cout << "Going to: " << mR.ResizedToReal(next, -8, -8) << std::endl;

        r.Read();

        point.px = p.GetXPos();
        point.py = p.GetYPos();
        DStartLite pP;
        pP.MountTheMap(mR.GetResizedMap(), mR.GetResizedWidth(), mR.GetResizedHeigth());
        current = mR.RealToResized(point, -8, -8);

        pP.Initialize(current, next);


        VertexPosition tmp;
        std::vector<VertexUpdate> up;
        while (pP.GetNext(tmp, up))
        {
            player_point_2d_t pointTmp;
            pointTmp = mR.ResizedToReal(tmp, -8, -8);
            std::cout << pointTmp << std::endl;
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
    }

    return 0;
}

