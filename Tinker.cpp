//
//  QTinker.cpp
//  Windexing
//
//  Created by Helen Ginn on 11/12/2016.
//  Copyright (c) 2016 Helen Ginn. All rights reserved.
//

#include "Tinker.h"
#include <QtCore/qdebug.h>
#include <QtCore/qalgorithms.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qmessagebox.h>
#include <iostream>
#include <fstream>
#include "RefinementNelderMead.h"
#include "FileReader.h"

#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 800
#define BUTTON_WIDTH 160
#define BEAM_CENTRE_GROUP_YOFFSET 180
#define BRAVAIS_LATTICE_YOFFSET 580

Tinker::Tinker(QWidget *parent) : QMainWindow(parent)
{
    this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    
    this->setWindowTitle("Mandexing");
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QAction *openAct = fileMenu->addAction(tr("&Open..."));
	connect(openAct, &QAction::triggered, this, &Tinker::openImage);
	QAction *saveAs = fileMenu->addAction(tr("&Save matrix..."));
	connect(saveAs, &QAction::triggered, this, &Tinker::saveMatrix);
	QAction *loadMatrix = fileMenu->addAction(tr("&Load matrix..."));
	connect(loadMatrix, &QAction::triggered, this, &Tinker::loadMatrix);
	
	myDialogue = NULL;
	bUnitCell = new QPushButton("Set unit cell", this);
	bUnitCell->setGeometry(0, 25, BUTTON_WIDTH, 50);
    connect(bUnitCell, SIGNAL(clicked()), this, SLOT(setUnitCellClicked()));

	bAxisOnScreen = new QPushButton("Bring axis on screen", this);
	bAxisOnScreen->setGeometry(0, 75, BUTTON_WIDTH, 50);
    connect(bAxisOnScreen, SIGNAL(clicked()), this,
            SLOT(setAxisOnScreenClicked()));

	bFixAxis = new QPushButton("Fix axis", this);
	bFixAxis->setGeometry(0, 125, BUTTON_WIDTH, 50);
    connect(bFixAxis, SIGNAL(clicked()), this,
            SLOT(fixAxisClicked()));

	bBeamCentre = new QPushButton("BC", this);
	bBeamCentre->setGeometry(55, BEAM_CENTRE_GROUP_YOFFSET + 50, 50, 50);
    connect(bBeamCentre, SIGNAL(clicked()), this,
            SLOT(setBeamCentreClicked()));

	bBeamXMinus = new QPushButton("-", this);
	bBeamXMinus->setGeometry(5, BEAM_CENTRE_GROUP_YOFFSET + 50, 50, 50);
	connect(bBeamXMinus, SIGNAL(clicked()), this,
            SLOT(beamXMinus()));

	bBeamXPlus = new QPushButton("+", this);
	bBeamXPlus->setGeometry(105, BEAM_CENTRE_GROUP_YOFFSET + 50, 50, 50);
	connect(bBeamXPlus, SIGNAL(clicked()), this,
            SLOT(beamXPlus()));

	bBeamYMinus = new QPushButton("-", this);
	bBeamYMinus->setGeometry(55, BEAM_CENTRE_GROUP_YOFFSET + 100, 50, 50);
	connect(bBeamYMinus, SIGNAL(clicked()), this,
            SLOT(beamYMinus()));

	bBeamYPlus = new QPushButton("+", this);
	bBeamYPlus->setGeometry(55, BEAM_CENTRE_GROUP_YOFFSET, 50, 50);
	connect(bBeamYPlus, SIGNAL(clicked()), this,
            SLOT(beamYPlus()));

	bDetDist = new QPushButton("Set detector distance", this);
	bDetDist->setGeometry(0, 330, BUTTON_WIDTH, 50);
    connect(bDetDist, SIGNAL(clicked()), this,
            SLOT(setDetDistClicked()));

	bWavelength = new QPushButton("Set wavelength", this);
	bWavelength->setGeometry(0, 380, BUTTON_WIDTH, 50);
    connect(bWavelength, SIGNAL(clicked()), this,
            SLOT(setWavelengthClicked()));

	bRlpSize = new QPushButton("Set rlp size", this);
	bRlpSize->setGeometry(0, 430, BUTTON_WIDTH, 50);
    connect(bRlpSize, SIGNAL(clicked()), this,
            SLOT(setRlpSizeClicked()));

	bDegrees = new QPushButton("Set degree step", this);
	bDegrees->setGeometry(0, 480, BUTTON_WIDTH, 50);
    connect(bDegrees, SIGNAL(clicked()), this,
            SLOT(setDegreeStepClicked()));

	bRefine = new QPushButton("Refine", this);
	bRefine->setGeometry(0, 530, BUTTON_WIDTH, 50);
    connect(bRefine, SIGNAL(clicked()), this,
            SLOT(refineClicked()));

	bLatPrimitive = new QPushButton("P", this);
	bLatPrimitive->setGeometry(30, BRAVAIS_LATTICE_YOFFSET, 50, 50);
	connect(bLatPrimitive, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticePrimitive); });

	bLatBase = new QPushButton("C", this);
	bLatBase->setGeometry(80, BRAVAIS_LATTICE_YOFFSET, 50, 50);
	connect(bLatBase, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticeBase); });

	bLatBody = new QPushButton("I", this);
	bLatBody->setGeometry(30, BRAVAIS_LATTICE_YOFFSET + 50, 50, 50);
	connect(bLatBody, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticeBody); });

	bLatFace = new QPushButton("F", this);
	bLatFace->setGeometry(80, BRAVAIS_LATTICE_YOFFSET + 50, 50, 50);
	connect(bLatFace, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticeFace); });


    bResolution = new QPushButton("Set resolution", this);
	bResolution->setGeometry(0, 680, BUTTON_WIDTH, 50);
	connect(bResolution, SIGNAL(clicked()), this,
			SLOT(setResolutionClicked()));

    bIdentifyHkl = new QPushButton("Identify hkl", this);
	bIdentifyHkl->setGeometry(0, 730, BUTTON_WIDTH, 50);
	connect(bIdentifyHkl, SIGNAL(clicked()), this,
			SLOT(identifyHkl()));


	_refineStage = 0;
	_fixAxisStage = 0;
    _identifyHklStage = 0;
    
	fileDialogue = NULL;

    imageLabel = new QLabel(this);
    imageLabel->setScaledContents(true);
    imageLabel->setGeometry(0, 0, DEFAULT_HEIGHT, DEFAULT_HEIGHT);
    imageLabel->show();
	bUnitCell->show();
	
	QBrush brush(Qt::transparent);
	
	_crystal.setTinker(this);

	overlayView = new PredictionView(imageLabel);
	overlay = new QGraphicsScene(overlayView);
	overlayView->setCrystal(&_crystal);
	overlayView->setDetector(&_detector);
	overlayView->setTinker(this);
	overlayView->setBackgroundBrush(brush);
	overlayView->setGeometry(0, 0, DEFAULT_HEIGHT, DEFAULT_HEIGHT);
	overlayView->setStyleSheet("background-color: transparent;");
	overlayView->setScene(overlay);

	overlayView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	overlayView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	overlayView->show();

	_detector.setCrystal(&_crystal);
}

void Tinker::resizeEvent(QResizeEvent *)
{
    int w = width();
    int h = height();
    int max_w = w;
    int max_h = h;
    
    h = (h < w) ? h : w;
    w = (w < h) ? w : h;
    
    int top = max_h / 2 - h / 2;
    int left = max_w / 2 - w / 2;
    
    if (left < BUTTON_WIDTH) left = BUTTON_WIDTH + 10;

    imageLabel->setGeometry(left, top, w, h);
	overlayView->setGeometry(0, 0, w, h);
	drawPredictions();
}

void Tinker::setup()
{

}

void Tinker::setUnitCellClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Choose unit cell (Å, º)",
    								"Enter a b c alpha beta gamma:",
    								"79.2 79.2 38.0 90 90 90",
    								"Set unit cell");
	myDialogue->setTag(DialogueUnitCell);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setAxisOnScreenClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Bring axis on screen",
    								"Enter one or two Miller index vectors:",
    								"1 0 0 0 0 1",
    								"Set axis");
	myDialogue->setTag(DialogueBringAxis);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setBeamCentreClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Set beam centre (pixels)",
    								"Enter new beam centre:",
    								"2200 2200",
    								"Set centre");
	myDialogue->setTag(DialogueBeamCentre);
    myDialogue->setTinker(this);
	myDialogue->show();
}


void Tinker::setRlpSizeClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Set rlp size (Å^-1)",
    								"Enter new rlp size:",
    								"0.002",
    								"Set rlp size");
	myDialogue->setTag(DialogueRlpSize);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::beamXPlus()
{
	changeBeamCentre(1, 0);
}

void Tinker::beamYPlus()
{
	changeBeamCentre(0, -1);

}

void Tinker::beamXMinus()
{
	changeBeamCentre(-1, 0);
}

void Tinker::beamYMinus()
{
	changeBeamCentre(0, 1);
}

void Tinker::setResolutionClicked()
{
	myDialogue = new Dialogue(this, "Set resolution (Å)",
    								"Enter new resolution:",
    								"1.8",
    								"Set resolution");
	myDialogue->setTag(DialogueResolution);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setDetDistClicked()
{
	myDialogue = new Dialogue(this, "Set distance (pixels)",
    								"Enter new detector distance:",
    								"174286",
    								"Set distance");
	myDialogue->setTag(DialogueDistance);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setWavelengthClicked()
{
	myDialogue = new Dialogue(this, "Set wavelength (Å)",
    								"Enter new wavelength:",
    								"0.0251",
    								"Set wavelength");
	myDialogue->setTag(DialogueWavelength);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setDegreeStepClicked()
{
	myDialogue = new Dialogue(this, "Set degree step (º)",
    								"Enter degrees per key press:",
    								"0.12",
    								"Set degrees");
	myDialogue->setTag(DialogueDegreeStep);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::changeBeamCentre(double deltaX, double deltaY)
{
	_detector.adjustBeamCentre(deltaX, deltaY);
	drawPredictions();
}

void Tinker::transformToDetectorCoordinates(int *x, int *y)
{
	double w = blankImage.width();
	double h = blankImage.height();
	double w2 = overlayView->width();
	double h2 = overlayView->height();
	double bx = _detector.getBeamCentre().x;
	double by = _detector.getBeamCentre().y;
	double window_bx = bx * w2 / w;
	double window_by = by * h2 / h;
	
	std::cout << *x << ", " << *y << " to ";
	
	*x = (*x - window_bx) * w / w2 + bx;
	*y = (*y - window_by) * w / w2 + by;

	std::cout << *x << ", " << *y << std::endl;
}

void Tinker::drawPredictions()
{
	_detector.calculatePositions();
	qDeleteAll(overlay->items());
	overlay->clear();
	
	double w = blankImage.width();
	double h = blankImage.height();
	double w2 = overlayView->width();
	double h2 = overlayView->height();
	double bx = _detector.getBeamCentre().x;
	double by = _detector.getBeamCentre().y;
	
	overlay->setSceneRect(overlayView->geometry());
	
	bx *= w2 / w;
	by *= h2 / h;
	
	int ellipseSize = 10;
	
	for (int i = 0; i < _crystal.millerCount(); i++)
	{
		bool onImage = _crystal.shouldDisplayMiller(i);

		if (!onImage)
		{
			continue;
		}
		
		double weight = _crystal.weightForMiller(i);
		if (weight < 0) continue;
		
		vec3 pos = _crystal.position(i);
		
		QPen pen = QPen(QColor(0, 0, 0, (1 - weight) * 255));
		bool watching = _crystal.isBeingWatched(i);
		
		if (_refineStage != 1) watching = false;
		
		QBrush brush(Qt::transparent);

		if (watching)
		{
			brush = QBrush(QColor(255, 255, 255, 50));
		}
		
		pos.x = w2 * pos.x / w + bx;
		pos.y = h2 * pos.y / h + by;
		
		if (pos.x < 20 || pos.y < 20 || pos.x > w2 - 20 || pos.y > h2 - 20)
		{
			continue;
		}

		overlay->addEllipse(pos.x - ellipseSize / 2, pos.y - ellipseSize / 2,
		 					ellipseSize, ellipseSize, pen, brush);	
	}
	
	/* Draw basis vectors for crystal in real space */
	
	mat3x3 scaled_basis = _crystal.getScaledBasisVectors();
	
	for (int i = 0; i < 3; i++)
	{
		QPen pen = QPen(QColor(255, 0, 0));
		vec3 basis_vector = mat3x3_axis(scaled_basis, i);
		basis_vector.x += bx;
		basis_vector.y += by;
		
		overlay->addLine(bx, by, basis_vector.x, basis_vector.y, pen);
	}
	
	/* Draw fixed axis, if exists */
	vec3 axis = _crystal.getFixedAxis();
	
	if (vec3_length(axis) > 0.5) // is set
	{
		QPen purple = QPen(QColor(255, 64, 255));
		vec3_mult(&axis, 100);
		overlay->addLine(-axis.x + bx, -axis.y + bx,
						  axis.x + bx, axis.y + bx, purple);
	}
}

void Tinker::receiveDialogue(DialogueType type, std::string diagString)
{
	std::cout << (diagString) << std::endl;
	
	std::vector<double> trial;
	
	while (true)
	{
		size_t pos = diagString.find(" ");
		
		if (pos >= diagString.length()) pos = diagString.length();
		
		std::string substr = diagString.substr(0, pos);
		double value = atof(substr.c_str());
		trial.push_back(value);
		
		std::cout << substr << " ";		
		if (pos >= diagString.length()) break;
		diagString = diagString.substr(pos + 1, diagString.length() - pos - 1);
	}
	
	if (type == DialogueUnitCell)
	{
		std::cout << std::endl;
		std::cout << "Unit cell has " << trial.size() << " parameters." << std::endl;
	
		if (trial.size() != 6)
		{
			return;
		}
		else
		{
			_crystal.setUnitCell(trial);
			drawPredictions();
		}
	}
	else if (type == DialogueBringAxis)
	{
		std::cout << "Axis has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() < 3)
		{
			return;
		}
		else
		{
			_crystal.bringAxisToScreen(trial);
			drawPredictions();
		}
	}
	else if (type == DialogueBeamCentre)
	{
		std::cout << "Beam centre has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 2)
		{
			return;
		}
		else
		{
			_detector.setBeamCentre(trial[0], trial[1]);
			drawPredictions();
		}
	}
	else if (type == DialogueResolution)
	{
		std::cout << "Resolution has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			return;
		}
		else
		{
			_crystal.setResolution(trial[0]);
			_crystal.populateMillers();
			drawPredictions();
		}
	}
	else if (type == DialogueDistance)
	{
		std::cout << "Distance has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			return;
		}
		else
		{
			_detector.setDetectorDistance(trial[0]);
			drawPredictions();
		}
	}
	else if (type == DialogueWavelength)
	{
		std::cout << "Wavelength has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			return;
		}
		else
		{
			_detector.setWavelength(trial[0]);
			_crystal.setWavelength(trial[0]);
			drawPredictions();
		}
	}
	else if (type == DialogueRlpSize)
	{
		std::cout << "Rlp size has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			return;
		}
		else
		{
			_crystal.setRlpSize(trial[0]);
			_crystal.populateMillers();
			drawPredictions();
		}
	}
	else if (type == DialogueDegreeStep)
	{
		std::cout << "Degree size has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			return;
		}
		else
		{
			double radians = deg2rad(trial[0]);
			overlayView->setRadiansPerKeyPress(radians);
			drawPredictions();
		}
	}

	myDialogue->cleanup();
	myDialogue->hide();
	delete myDialogue;

	myDialogue = NULL;
}

void Tinker::openImage()
{
	delete fileDialogue;
	fileDialogue = new QFileDialog(this, tr("Open images"),
									 tr("Image Files (*.png *.jpg *.tif *.bmp)"));
	fileDialogue->setFileMode(QFileDialog::AnyFile);
	fileDialogue->show();
	
	QStringList fileNames;
	if (fileDialogue->exec())
	{
    	fileNames = fileDialogue->selectedFiles();
    }
    
    std::cout << "Read " << fileNames.size() << std::endl;
    
	if (fileNames.size() >= 1)
	{
		if (!blankImage.load(fileNames[0]))
		{
			qDebug("Error loading image");
		}

		bool first = false;

		if (!imageLabel->pixmap())
		{
			first = true;
		}

		std::string filename = fileNames[0].toStdString();
		std::string newTitle = "Mandexing - " + getFilename(filename);
		
		this->setWindowTitle(newTitle.c_str());

		imageLabel->setPixmap(blankImage);
		if (first)
		{
			_detector.setBeamCentre(blankImage.width() / 2,
		   	                        blankImage.height() / 2);
		}
	}
}

void Tinker::loadMatrix()
{
	delete fileDialogue;
	fileDialogue = new QFileDialog(this, tr("Load matrix"),
									 tr("Mandexing matrix file (*.dat)"));
	fileDialogue->setFileMode(QFileDialog::AnyFile);
	fileDialogue->show();
	
	QStringList fileNames;
	if (fileDialogue->exec())
	{
    	fileNames = fileDialogue->selectedFiles();
    }
    
    if (fileNames.size() >= 1)
	{
		std::string filename = fileNames[0].toStdString();
		std::string matrix = get_file_contents(filename);
		
		std::vector<std::string> components = split(matrix, ' ');
		
		QMessageBox *msgBox = new QMessageBox(this);
		msgBox->setStandardButtons(QMessageBox::Ok);
		msgBox->setDefaultButton(QMessageBox::Ok);
		msgBox->setWindowModality(Qt::NonModal);
		msgBox->setText("Sorry no");
			
		if (components.size() < 10)
		{
			msgBox->setInformativeText("Not enough components, expecting 10 space-separated "\
			"values. Try again.");
			msgBox->exec();
		}
		else if (components[0] != "rotation")
		{
			msgBox->setInformativeText("Expecting the first component to be 'rotation'.");		
			msgBox->exec();
		}

		mat3x3 rotMat = make_mat3x3();

		for (int i = 1; i < 10; i++)
		{
			float value = atof(components[i].c_str());
			rotMat.vals[i - 1] = value;
		}
		
		_crystal.setRotation(rotMat);
		_crystal.populateMillers();
		drawPredictions();
		
		delete msgBox;
	}
}
    
void Tinker::saveMatrix()
{
	delete fileDialogue;
	fileDialogue = new QFileDialog(this, tr("Save as .dat file"),
									 tr("matrix.dat"));
	fileDialogue->setFileMode(QFileDialog::AnyFile);
	fileDialogue->setAcceptMode(QFileDialog::AcceptSave);
	fileDialogue->show();
	
	QStringList fileNames;
	if (fileDialogue->exec())
	{
    	fileNames = fileDialogue->selectedFiles();
    }
    
    if (fileNames.size() >= 1)
	{
		std::ofstream file;
		file.open(fileNames[0].toStdString().c_str());
		
		mat3x3 rot = _crystal.getRotation();
		
		file << "rotation ";
		
		for (int i = 0; i < 9; i++)
		{
			file << rot.vals[i] << " ";
		}
		
		file << std::endl;
		
		file.close();
	}
}

void Tinker::fixAxisClicked()
{
	if (_fixAxisStage == 0)
	{
		_fixAxisStage = 1;
		bFixAxis->setText("Choose two points");
		overlayView->setFixAxisStage(_fixAxisStage);
	}
	else
	{
		bFixAxis->setText("Fix axis");
		overlayView->setFixAxisStage(-1);
		_fixAxisStage = 0;		
	}
}

void Tinker::finishFixAxis()
{
	_fixAxisStage = 0;
	bFixAxis->setText("Fix axis");	
	drawPredictions();
}

void Tinker::changeLattice(BravaisLatticeType type)
{
	_crystal.setBravaisLattice(type);
	_crystal.populateMillers();
	drawPredictions();
}

void Tinker::startRefinement()
{
	_refineStage = 2;
	bRefine->setText("Refining...");
	
	NelderMeadPtr mead = NelderMeadPtr(new NelderMead());
	mead->setEvaluationFunction(Crystal::ewaldSphereClosenessScore, &_crystal);
	mead->addParameter(&_crystal, Crystal::getHorizontal, Crystal::setHorizontal, 0.002, 0.0002);
	mead->addParameter(&_crystal, Crystal::getVertical, Crystal::setVertical, 0.002, 0.0002);
	mead->setCycles(15);
	mead->refine();
	
	_crystal.clearUpRefinement();
	_refineStage = 0;
	bRefine->setText("Refine");
	
//	QtConcurrent::run(RefinementStrategy::run, &*mead);
}

void Tinker::refineClicked()
{
	if (_refineStage == 0)
	{
		_detector.prepareLookupTable();
		_refineStage = 1;
		bRefine->setText("Choose good spots");
		overlayView->setRefineStage(_refineStage);
	}
	else
	{
		bRefine->setText("Refine");
		overlayView->setRefineStage(0);
		_refineStage = 0;		
	}
}


void Tinker::identifyHkl()
{
    if (_identifyHklStage == 0)
    {
		_detector.prepareLookupTable();
        _identifyHklStage = 1;
        //Button name.
        bIdentifyHkl->setText("Choose refl");
        overlayView->setIdentifyHklStage(_identifyHklStage);
    }
    else
    {
        bIdentifyHkl->setText("Identify hkl");
        overlayView->setIdentifyHklStage(0);
        _identifyHklStage = 0;
    }
}

Tinker::~Tinker()
{
	delete bUnitCell;
	delete imageLabel;
}
