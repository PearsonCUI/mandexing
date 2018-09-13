// Mandexing: a manual indexing program for crystallographic data.
// Copyright (C) 2017-2018 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

#ifndef __Windexing__PredictionView__
#define __Windexing__PredictionView__

#include "Crystal.h"
#include "Detector.h"
#include "shared_ptrs.h"
#include <QtWidgets/qgraphicsview.h>

class Tinker;

class PredictionView : public QGraphicsView
{
    Q_OBJECT
    
public:
    PredictionView(QWidget *parent = 0);
    
    void setCrystal(Crystal *crystal)
    {
        _crystal = crystal;
    }

    void setDetector(Detector *det)
    {
        _detector = det;
    }

    void setTinker(Tinker *tinker)
    {
        _tinker = tinker;
    }
    
    void setRadiansPerKeyPress(double rad)
    {
        _radPerKeyPress = rad;
        _keyPressSwitch = deg2rad(0.5) / rad;
    }

    void setFixAxisStage(int stage);
    void setRefineStage(int stage);
    void setIdentifyHklStage(int stage);
    virtual void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *event);
   
    Detector *_detector; 
    Crystal *_crystal;
    Tinker *_tinker;
    
    int _lastX;
    int _lastY;
    
    int _keyPresses;
    int _keyPressSwitch;
        
    int _fixAxisStage;
    int _refineStage;
	int _identifyHklStage;

    double _radPerKeyPress;
    
	int _singleWatch;
    
    vec3 _fixAxisPoints[2];
};

#endif 
