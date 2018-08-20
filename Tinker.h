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

#ifndef __Windexing__QTinker__
#define __Windexing__QTinker__

#include <stdio.h>

#include "shared_ptrs.h"
#include "Detector.h"
#include "Dialogue.h"
#include <QtWidgets/qwidget.h>
#include <QtCore/qglobal.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlabel.h>
#include <QtGui/qpixmap.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qgraphicsview.h>
#include "Crystal.h"
#include "PredictionView.h"
#include <vector>
#include <QtCore/qsignalmapper.h>

class Tinker : public QMainWindow
{
    Q_OBJECT
    
public:
    Tinker(QWidget *parent = 0);

	/* Buttons down the side */
    QPushButton *bUnitCell;
    QPushButton *bAxisOnScreen;
    QPushButton *bBeamCentre;
    QPushButton *bFixAxis;
    QPushButton *bDetDist;
    QPushButton *bWavelength;
    QPushButton *bRlpSize;
    QPushButton *bRefine;
	QPushButton *bIdentifyHkl;
    QPushButton *bDegrees;
    QPushButton *bResolution;
    QPushButton *bLatPrimitive, *bLatBody, *bLatFace, *bLatBase;
    QSignalMapper *latMapper;
    
    /* Beam centre adjust buttons */
    QPushButton *bBeamXPlus, *bBeamXMinus;
    QPushButton *bBeamYPlus, *bBeamYMinus;
    
    /* Image display */
    QPixmap blankImage;
    QGraphicsScene *overlay;
    PredictionView *overlayView;
    QLabel *imageLabel;
    
    Dialogue *myDialogue;
	QFileDialog *fileDialogue;
    
    void setup();

	void receiveDialogue(DialogueType type, std::string diagString);
	void drawPredictions();
	void finishFixAxis();
	void transformToDetectorCoordinates(int *x, int *y);
	void startRefinement();


    ~Tinker();
protected:
    virtual void resizeEvent(QResizeEvent *event);
private slots:
    
    /* Crystal params */
    
    void setUnitCellClicked();
    void setRlpSizeClicked();
    void setResolutionClicked();
    void identifyHkl();
    void changeLattice(BravaisLatticeType type);
    
    
    /* Expt params*/
	
	void setDetDistClicked();
	void setWavelengthClicked();
    
    void setAxisOnScreenClicked();
    void fixAxisClicked();
    
    void setDegreeStepClicked();


	/* Beam centre adjustments */
    void setBeamCentreClicked();
	void beamXPlus();
	void beamYPlus();
	void beamXMinus();
	void beamYMinus();
    
    
    /* Menu slots */
    
    void openImage();
    void saveMatrix();
    void loadMatrix();
    
    /* Process */
	
	void refineClicked();
	

private:
	void changeBeamCentre(double deltaX, double deltaY);
	QLabel *_notice;
	
	
	std::vector<double> _unitCell;
	Crystal _crystal;
	Detector _detector;

	int _identifyHklStage;
	int _fixAxisStage;
	int _refineStage;
};

#endif /* defined(__CaroCode__QTinker__) */
